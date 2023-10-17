/*
 * dht11.c
 *
 *  Created on: 13 oct. 2023
 *      Author: sergi
 */


#include "dht11.h"
#include "gpio.h"
#include "fsl_clock.h"

dht_data_t dht11_read(void)
{
	// BUFFER TO RECEIVE
	uint8_t bits[5];
	uint8_t cnt = 7;
	uint8_t idx = 0;
	uint8_t i = 0;
	uint32_t loopCnt = 10000;
	uint32_t clk_freq = CLOCK_GetCoreSysClkFreq();
	dht_data_t output = {0};

	// EMPTY BUFFER
	for (i=0; i< 5; i++) bits[i] = 0;

	// REQUEST SAMPLE
	GPIO_PIN_MODE(9u, 0);
	GPIO_Port_Clear(GPIO_B, (1<<9U));
	SDK_DelayAtLeastUs(18500,clk_freq);
	GPIO_Port_Set(GPIO_B, (1<<9U));
	SDK_DelayAtLeastUs(40,clk_freq);
	GPIO_PIN_MODE(9u, 1);

	// ACKNOWLEDGE or TIMEOUT
	while(GPIO_PORT_READ(GPIO_B, 9u) != 0)
	{
		if (loopCnt-- == 0)
		{
			output.humidity = 0;
			output.temperature = 0;
		}
	}

	loopCnt = 10000;
	while(GPIO_PORT_READ(GPIO_B, 9u) != 1)
	{
		if (loopCnt-- == 0)
		{
			output.humidity = 0;
			output.temperature = 0;
		}
	}

	// READ OUTPUT - 40 BITS => 5 BYTES or TIMEOUT
	for (i=0; i<40; i++)
	{
		loopCnt = 10000;
		while(GPIO_PORT_READ(GPIO_B, 9u) != 0)
		{
			if (loopCnt-- == 0)
			{
				output.humidity = 0;
				output.temperature = 0;
			}
		}
		while(GPIO_PORT_READ(GPIO_B, 9u) != 1)
		{

		}

		loopCnt = 10000;
		while(GPIO_PORT_READ(GPIO_B, 9u) == 1)
		{
			if (loopCnt-- == 0)
			{
				output.humidity = 0;
				output.temperature = 0;
			}
			//Se tarda en el while 2.38us, 0.8us o 1.66us
		}

		if (loopCnt < 9960)
		{
			bits[idx] |= (1 << cnt);
		}
		if (cnt == 0)   // next byte?
		{
			cnt = 7;    // restart at MSB
			idx++;      // next byte!
		}
		else cnt--;
	}

	// WRITE TO RIGHT VARS
        // as bits[1] and bits[3] are allways zero they are omitted in formulas.
	output.humidity    = bits[0];
	output.temperature = bits[2];

	return output;
}
