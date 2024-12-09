#include <stdio.h>
#include <string.h>
#include <time.h>
#include "minirisc/minirisc.h"
#include "minirisc/harvey_platform.h"
#include "support/uart.h"
#include "FreeRTOS.h"
#include "task.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

void init_video()
{
    static uint32_t frame_buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
    memset(frame_buffer, 0, sizeof(frame_buffer)); // clear frame buffer to black
    VIDEO->WIDTH = SCREEN_WIDTH;
    VIDEO->HEIGHT = SCREEN_HEIGHT;
    VIDEO->DMA_ADDR = frame_buffer;
    VIDEO->CR = VIDEO_CR_IE | VIDEO_CR_EN;
}

void hello_task(void *arg)
{
    (void)arg;
    time_t tv = time(NULL);

    while (1)
    {
        tv = time(NULL);
        printf("Hello, World!  The date is: %s", ctime(&tv));
        vTaskDelay(MS2TICKS(1000));
    }
}

void echo_task(void *arg)
{
    (void)arg;
    char buf[128];

    while (1)
    {
        int i = 0;
        char c;
        while ((c = getchar()) != '\r' && i != (sizeof(buf) - 1))
        {
            buf[i++] = c;
        }
        buf[i] = '\0';
        printf("Received: \"%s\"\n", buf);
        if (strcmp(buf, "quit") == 0)
        {
            minirisc_halt();
        }
    }
}

int main()
{
    init_uart();
    init_video();

    xTaskCreate(hello_task, "hello", 1024, NULL, 1, NULL);
    xTaskCreate(echo_task, "echo", 1024, NULL, 1, NULL);
    vTaskStartScheduler();

    return 0;
}
