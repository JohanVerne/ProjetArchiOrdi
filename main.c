#include <stdint.h>
#include <stdlib.h>

#define VIDEO_CONTROLLER_BASE 0x22020000
typedef struct
{
    volatile uint32_t CR;
    volatile uint32_t SR;
    volatile uint32_t DMA_ADDR;
    volatile uint32_t WIDTH;
    volatile uint32_t HEIGHT;
    volatile uint32_t BPP;
    volatile uint32_t PITCH;
    volatile uint32_t RED_MASK;
    volatile uint32_t GREEN_MASK;
    volatile uint32_t BLUE_MASK;
} video_controller_t;

#define VIDEO_CONTROLLER ((volatile video_controller_t *)VIDEO_CONTROLLER_BASE)

void init()
{
    VIDEO_CONTROLLER->WIDTH = 640;
    VIDEO_CONTROLLER->HEIGHT = 480;
    uint32_t *framebuffer = malloc(VIDEO_CONTROLLER->WIDTH * VIDEO_CONTROLLER->HEIGHT * sizeof(uint32_t));
    VIDEO_CONTROLLER->DMA_ADDR = (uint32_t)framebuffer;
    VIDEO_CONTROLLER->CR = 1; // machine convertit en binaire et met 1 au champ EN
}

uint32_t make_color(uint8_t red, uint8_t green, uint8_t blue)
{
    return ((uint32_t)red << 16) | ((uint32_t)green << 8) | (uint32_t)blue;
}

int main()
{
    init();

    uint32_t *framebuffer = (uint32_t *)VIDEO_CONTROLLER->DMA_ADDR;
    int square_x = 0;
    int square_y = VIDEO_CONTROLLER->HEIGHT / 2 - 25;
    int square_size = 50;
    int dx = 5;
    int dy = 5;

    while (1)
    {

        while (!(VIDEO_CONTROLLER->SR & 0x02))
        {
        }
        VIDEO_CONTROLLER->SR = 0;

        for (int y = 0; y < square_size; y++)
        {
            for (int x = 0; x < square_size; x++)
            {
                int pixel_x = square_x + x;
                int pixel_y = square_y + y;
                if (pixel_x >= 0 && pixel_x < (int)VIDEO_CONTROLLER->WIDTH &&
                    pixel_y >= 0 && pixel_y < (int)VIDEO_CONTROLLER->HEIGHT)
                {
                    uint8_t red = (uint8_t)((pixel_x * 255) / VIDEO_CONTROLLER->WIDTH);
                    uint8_t green = (uint8_t)((pixel_y * 255) / VIDEO_CONTROLLER->HEIGHT);
                    uint8_t blue = (uint8_t)(((pixel_x + pixel_y) * 255) / (VIDEO_CONTROLLER->WIDTH + VIDEO_CONTROLLER->HEIGHT));
                    uint32_t color = make_color(red, green, blue);

                    framebuffer[pixel_y * (int)VIDEO_CONTROLLER->WIDTH + pixel_x] = color;
                }
            }
        }

        square_x += dx;
        square_y += dy;

        if (square_x <= 0 || square_x + square_size >= (int)VIDEO_CONTROLLER->WIDTH)
        {
            dx = -dx;
        }
        if (square_y <= 0 || square_y + square_size >= (int)VIDEO_CONTROLLER->HEIGHT)
        {
            dy = -dy;
        }
    }
    return 0;
}