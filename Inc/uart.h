
#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include <string.h>
#include "stm32f7xx_hal.h"

#define UART_TIMEOUT 100

void uart_send(UART_HandleTypeDef *huart, char* data);

#endif /* UART_H_ */
