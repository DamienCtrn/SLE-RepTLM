/********************************************************************
 * Copyright (C) 2009, 2012 by Verimag                              *
 * Initial author: Matthieu Moy                                     *
 ********************************************************************/

/*!
  \file hal.h
  \brief Harwdare Abstraction Layer : implementation for MicroBlaze
  ISS.


*/
#ifndef HAL_H
#define HAL_H

#define UADDR UART_BASEADDR + UART_FIFO_WRITE

#include <stdint.h>

// #define abort() _hw_exception_handler();

// Dummy implementation of abort(): invalid instruction
#define abort() ({								\
	do {										\
		printf("abort() function called\r\n");  \
 		_hw_exception_handler();				\
	} while (0);								})

/* TODO: implement HAL primitives for cross-compilation */
#define hal_read32(a)      *(volatile unsigned int *)(a);
#define hal_write32(a, d)  *(volatile unsigned int *)(a) = (d);
#define hal_wait_for_irq() do {} while(0)
#define hal_cpu_relax()    do {} while(0)

void microblaze_enable_interrupts(void) {
	__asm("ori     r3, r0, 2\n"
	      "mts     rmsr, r3");
}

#define printf(str) 						\
	do {									\
		int i;								\
		for (i = 0; str[i] = '\0'; i++) {	\
			hal_write32(UADDR, *str);		\
		}									\
	} while(0);

#endif /* HAL_H */
