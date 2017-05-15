
#include "uart.h"

void uart_send(UART_HandleTypeDef *huart, char* data)
{
	HAL_UART_Transmit(huart, (uint8_t*)data, strlen(data)+1, UART_TIMEOUT);
}
