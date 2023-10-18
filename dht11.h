/*
 * dht11.h
 *
 *  Created on: 13 oct. 2023
 *      Author: Sergio Eduardo Borrayo Anzaldo
 */

#ifndef DHT11_H_
#define DHT11_H_

#include "stdint.h"

typedef struct
{
	uint32_t humidity;
	uint32_t temperature;
}dht_data_t;

dht_data_t dht11_read(void);

#endif /* DHT11_H_ */
