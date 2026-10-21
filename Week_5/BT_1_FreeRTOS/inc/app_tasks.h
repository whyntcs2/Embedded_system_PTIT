#ifndef APP_TASKS_H
#define APP_TASKS_H

#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"

extern QueueHandle_t xLedQueue[3];

void vLEDTask(void *pvParameters);
void vButtonTask(void *pvParameters);

#endif