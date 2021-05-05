#include "Tuning.h"
#include "uart.h"
#include "Config.h"
#include <stdio.h>

static float kp = DEFAULT_KP;
static float ki = DEFAULT_KI;
static float kd = DEFAULT_KD;

void Tuning_init(void)
{
	uart3_init();
}

float Tuning_get_kp(void)
{
	return kp;
}

float Tuning_get_ki(void)
{
	return ki;
}

float Tuning_get_kd(void)
{
	return kd;
}

// protocol: p<kp float value>i<ki float value>d<kd float value>!
// you better send the whole string at once or this will lock up the micro
// untested, good luck
void Tuning_update(void)
{
	if (UART3_S1 & UART_S1_RDRF_MASK)
	{
		char c;
		int i = 0;
		char buf[50];
		do{
			c = UART3_D;
			buf[i] = c;
			uart0_putchar(c);
			i++;
		}while(c != '!');
		buf[i] = 0; // null terminate
		
		scanf(buf, "p%fi%d!", &kp, &ki, &kd);
		//uart0_put(buf);
	}
}


