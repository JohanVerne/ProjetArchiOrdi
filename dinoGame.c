#include <stdio.h>
#include <string.h>
#include <time.h>
#include "minirisc/minirisc.h"
#include "minirisc/harvey_platform.h"
#include "support/uart.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "xprintf/xprintf.h"
#include "task.h"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 480

static SemaphoreHandle_t videoSem = NULL;

static uint32_t frame_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

uint32_t color = 0xFFFFFFFF;
int dinoLength = 40;
int dinoHeight = 100;
volatile int dinoX, dinoY;

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
    dinoX = SCREEN_WIDTH / 4 - dinoLength / 2;
    dinoY = SCREEN_HEIGHT - dinoHeight / 2;
    videoSem = xSemaphoreCreateBinary();

    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        frame_buffer[i] = 0;
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
                dinoY -= 50;
                break;
            case 115: // S
                dinoY += 50;
                break;
            case 32: // space
                break;
            case 82: // UP
                break;
            case 81: // DOWN
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

void printScreen(void *arg)
{
    while (1)
    {
        xSemaphoreTake(videoSem, portMAX_DELAY);
        memset(frame_buffer, 0, sizeof(frame_buffer)); // clear
        for (int y = -dinoHeight / 2; y < dinoHeight / 2; y++)
        {
            for (int x = -dinoLength / 2; x < dinoLength / 2; x++)
            {
                int pixelX = dinoX + x;
                int pixelY = dinoY + y;
                frame_buffer[pixelY * SCREEN_WIDTH + pixelX] = color;
            }
        }
        xprintf("y: %d\n", dinoY);
    }
}
int main()
{
    init_variables();
    init_uart();
    init_video();

    KEYBOARD->CR |= KEYBOARD_CR_IE;

    minirisc_enable_interrupt(VIDEO_INTERRUPT | KEYBOARD_INTERRUPT);

    xTaskCreate(printScreen, "Screen", 1024, NULL, 1, NULL);

    vTaskStartScheduler();

    return 0;
}
