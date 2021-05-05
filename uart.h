#ifndef UART_H
#define UART_H
#include "MK64F12.h"

/**
* Synchronously read a character from the uart0 bus.
* return read character
*/
uint8_t uart0_getchar(void);

/**
* Configure uart0 peripheral for synchronous operation.
*/
void uart0_init(void);

/**
* Synchronously write to the uart0 bus.
* char [in] - character to write on the bus
*/
void uart0_putchar(char ch);

/**
* Synchronously write a string to the uart0 bus.
* ptr_str [in] - pointer to beginning of null-terminated string
*/
void uart0_put(char* ptr_str);

/**
* Synchronously read a character from the uart3 bus.
* return read character
*/
uint8_t uart3_getchar(void);

/**
* Configure uart3 peripheral for synchronous operation.
*/
void uart3_init(void);

/**
* Synchronously write to the uart0 bus.
* char [in] - character to write on the bus
*/
void uart3_putchar(char ch);

/**
* Synchronously write a string to the uart0 bus.
* ptr_str [in] - pointer to beginning of null-terminated string
*/
void uart3_put(char* ptr_str);

#endif // UART_H
