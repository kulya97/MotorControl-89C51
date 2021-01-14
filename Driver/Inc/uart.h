#ifndef __UART_H_
#define __UART_H_
#include "commond.h"
void uart_init(u8 baud);
void send8bit(u8 dat);
void send(u8 *str,u16 length);

#endif
