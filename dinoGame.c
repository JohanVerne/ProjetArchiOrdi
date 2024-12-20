#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "minirisc/minirisc.h"
#include "minirisc/harvey_platform.h"
#include "support/uart.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "xprintf/xprintf.h"
#include "task.h"
#include "font.h"
#include "sprite.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 480

#define MAX_CACTUSES 5
#define MAX_PTEROS 3

typedef struct
{
    int x, y;     // Position
    int isActive; // 1 if active, 0 if inactive
} Obstacle;

Obstacle cactuses[MAX_CACTUSES];
Obstacle pteros[MAX_PTEROS];

extern const sprite_t dinoSpriteStatic;
extern const sprite_t dinoSpriteCrouched1;
extern const sprite_t dinoSpriteCrouched2;
extern const sprite_t dinoSpriteMoving1;
extern const sprite_t dinoSpriteMoving2;
extern const sprite_t dinoSpriteDead;
extern const sprite_t Cactus1;
extern const sprite_t Ptero1;
extern const sprite_t Ptero2;

static SemaphoreHandle_t videoSem = NULL;

static uint32_t frame_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

static int velocity = 0; // initial vertical velocity is 0
static int deltaVelocity = -60;
static int gravity = 5;          // gravitational force
static int is_on_ground = 1;     // check if the dino is on the ground or not
static int is_crouched = 0;      // check if the dino is crouched or not (double gravity and change sprite)
static int dinoSpriteNumber = 1; // select correct dino Sprite number depending on is_crouched
int spriteUpdate = 5;            // number of frames we change the dino sprite
int spriteUpdatePtero = 15;      // number of frames we change the ptero sprite

uint32_t color = 0xFFFFFFFF;
int dinoLength = 96;
int dinoHeight = 98;
volatile int dinoX, dinoY;

int cactusLength = 58;
int cactusHeight = 103;

int PteroLength = 100;
int PteroHeight = 73;

void init_video()
{

    memset(frame_buffer, 0, sizeof(frame_buffer)); // clear frame buffer to black
    VIDEO->WIDTH = SCREEN_WIDTH;
    VIDEO->HEIGHT = SCREEN_HEIGHT;
    VIDEO->DMA_ADDR = frame_buffer;
    VIDEO->CR = VIDEO_CR_IE | VIDEO_CR_EN;
}

void init_variables()
{
    dinoX = SCREEN_WIDTH / 6;
    dinoY = SCREEN_HEIGHT;
    videoSem = xSemaphoreCreateBinary();

    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        frame_buffer[i] = 0;
    }

    // Initialize cactuses
    for (int i = 0; i < MAX_CACTUSES; i++)
    {
        cactuses[i].x = -1; // Start offscreen
        cactuses[i].y = SCREEN_HEIGHT - cactusHeight;
        cactuses[i].isActive = 0;
    }

    // Initialize pterodactyls
    for (int i = 0; i < MAX_PTEROS; i++)
    {
        pteros[i].x = -1;                  // Start offscreen
        pteros[i].y = SCREEN_HEIGHT - 200; // Approximate height for flying
        pteros[i].isActive = 0;
    }
}

void keyboard_interrupt_handler()
{
    uint32_t kdata;
    while (KEYBOARD->SR & KEYBOARD_SR_FIFO_NOT_EMPTY)
    {
        kdata = KEYBOARD->DATA;
        if (kdata & KEYBOARD_DATA_PRESSED)
        {
            // xprintf("key code: %d\n", KEYBOARD_KEY_CODE(kdata));
            switch (KEYBOARD_KEY_CODE(kdata))
            {
            case 113: // Q
                minirisc_halt();
                break;
            case 122: // Z
            // case 32:                        // Spacebar
            case 82:                        // UP arrow
                if (dinoY == SCREEN_HEIGHT) // if the dino ins't already jumping
                {
                    velocity = deltaVelocity; //
                    is_on_ground = 0;         // reset the is_on_ground flag
                }
                break;
            case 115: // S
            case 81:  // DOWN arrow
                is_crouched = 1;
                break;
            }
        }
        else // Key released
        {
            switch (KEYBOARD_KEY_CODE(kdata))
            {
            case 115: // S key released (uncrouch)
            case 81:
                is_crouched = 0;
                break;
            }
        }
    }
}

void video_interrupt_handler()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(videoSem, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    VIDEO->SR = 0;
}

void printScreen()
{
    int frameCounter = 0;
    char score[100];
    while (1)
    {
        xSemaphoreTake(videoSem, portMAX_DELAY);
        frameCounter++;

        update_game(); // mise à jour du jeu à chaque frame

        spawn_obstacles(frameCounter);
        move_obstacles();
        check_collisions();

        memset(frame_buffer, 0, sizeof(frame_buffer)); // clear

        if (!is_on_ground)
        {
            draw_sprite(dinoX, dinoY - dinoHeight, &dinoSpriteStatic);
        }
        else if (!is_crouched)
        {
            if (frameCounter % (2 * spriteUpdate) < spriteUpdate)
            {
                draw_sprite(dinoX, dinoY - dinoHeight, &dinoSpriteMoving1);
            }
            else
            {
                draw_sprite(dinoX, dinoY - dinoHeight, &dinoSpriteMoving2);
            }
        }
        else
        {
            if (frameCounter % (2 * spriteUpdate) < spriteUpdate)
            {
                draw_sprite(dinoX, dinoY - dinoHeight, &dinoSpriteCrouched1);
            }
            else
            {
                draw_sprite(dinoX, dinoY - dinoHeight, &dinoSpriteCrouched2);
            }
        }
        // Draw score
        sprintf(score, "Score: %d", frameCounter / spriteUpdate);
        font_16x32_draw_text(20, 20, score, 0xfffffffff, 0x00000000);

        // Draw active cactuses
        for (int i = 0; i < MAX_CACTUSES; i++)
        {
            if (cactuses[i].isActive)
            {
                draw_sprite(cactuses[i].x, cactuses[i].y, &Cactus1);
            }
        }

        // Draw active pterodactyls
        for (int i = 0; i < MAX_PTEROS; i++)
        {
            if (pteros[i].isActive)
            {
                if (frameCounter % (2 * spriteUpdatePtero) < spriteUpdatePtero)
                {
                    draw_sprite(pteros[i].x, pteros[i].y, &Ptero1);
                }
                else
                {
                    draw_sprite(pteros[i].x, pteros[i].y, &Ptero2);
                }
            }
        }
    }
}

void update_game()
{
    // Apply gravity to the dino
    if (!is_on_ground)
    {
        velocity += gravity;
        dinoY += velocity;

        if (dinoY >= SCREEN_HEIGHT)
        {
            dinoY = SCREEN_HEIGHT;
            velocity = 0;
            is_on_ground = 1;
        }
    }
    else
    {
        // If the dinosaur is on the ground and the velocity is negative, it's jumping
        if (velocity < 0)
        {
            is_on_ground = 0;
        }
    }

    if (!is_crouched)
    {
        dinoSpriteNumber = 1;
        gravity = 5;
        dinoLength = 96;
        dinoHeight = 98;
    }
    else
    {
        dinoSpriteNumber = 0;
        gravity = 20;
        dinoLength = 122;
        dinoHeight = 71;
    }
}

void spawn_obstacles(int frameCounter)
{
    // Spawn cactuses
    for (int i = 0; i < MAX_CACTUSES; i++)
    {
        if (!cactuses[i].isActive && frameCounter % (100 + rand() % (250 - 100 + 1)) == 0)
        {
            cactuses[i].x = SCREEN_WIDTH; // Spawn at the right edge
            cactuses[i].isActive = 1;
            break;
        }
    }

    // Spawn pterodactyls
    for (int i = 0; i < MAX_PTEROS; i++)
    {
        if (!pteros[i].isActive && frameCounter % (200 + rand() % (500 - 200 + 1)) == 0)
        {
            pteros[i].x = SCREEN_WIDTH;
            pteros[i].isActive = 1;
            break;
        }
    }
}

void move_obstacles()
{
    // Move cactuses
    for (int i = 0; i < MAX_CACTUSES; i++)
    {
        if (cactuses[i].isActive)
        {
            cactuses[i].x -= 8 + rand() % (12 - 8 + 1); // Move left
            if (cactuses[i].x + cactusLength < 0)
            { // Offscreen
                cactuses[i].isActive = 0;
            }
        }
    }

    // Move pterodactyls
    for (int i = 0; i < MAX_PTEROS; i++)
    {
        if (pteros[i].isActive)
        {
            pteros[i].x -= 10 + rand() % (20 - 10 + 1); // Move faster than cactuses
            if (pteros[i].x + PteroLength < 0)
            { // Offscreen
                pteros[i].isActive = 0;
            }
        }
    }
}

int check_collision(int x1, int y1, int width1, int height1,
                    int x2, int y2, int width2, int height2)
{
    return !(x1 + width1 < x2 ||  // Dino's right edge is left of cactus
             x1 > x2 + width2 ||  // Dino's left edge is right of cactus
             y1 + height1 < y2 || // Dino's bottom edge is above cactus
             y1 > y2 + height2);  // Dino's top edge is below cactus
}

void check_collisions()
{
    // Dino's bounding box
    int dinoXLeft = dinoX;
    int dinoYTop = dinoY - dinoHeight;

    // Check cactus collisions
    for (int i = 0; i < MAX_CACTUSES; i++)
    {
        if (cactuses[i].isActive)
        {
            if (check_collision(dinoXLeft, dinoYTop, dinoLength, dinoHeight,
                                cactuses[i].x, cactuses[i].y, cactusLength, cactusHeight))
            {
                // Handle collision
                xprintf("Collision with cactus at index %d!\n", i);
                game_over();
            }
        }
    }

    // Check pterodactyl collisions
    for (int i = 0; i < MAX_PTEROS; i++)
    {
        if (pteros[i].isActive)
        {
            if (check_collision(dinoXLeft, dinoYTop, dinoLength, dinoHeight,
                                pteros[i].x, pteros[i].y, PteroLength, PteroHeight))
            {
                // Handle collision
                xprintf("Collision with pterodactyl at index %d!\n", i);
                game_over();
            }
        }
    }
}

void game_over()
{
    xprintf("Game Over!\n");
    minirisc_halt(); // Halt the system or reset the game state
}

int main()
{
    srand(time(NULL));
    init_variables();
    init_uart();
    init_video();

    KEYBOARD->CR |= KEYBOARD_CR_IE;

    minirisc_enable_interrupt(VIDEO_INTERRUPT | KEYBOARD_INTERRUPT);

    xTaskCreate(printScreen, "Screen", 1024, NULL, 1, NULL);

    vTaskStartScheduler();

    return 0;
}
