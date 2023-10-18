/*
 * config.h
 *
 *  Created on: 12 sep 2022
 * Author: Sonny Ceja (creditos), Sergio Borrayo
*/

#ifndef CONFIG_H_
#define CONFIG_H_

#include "stdint.h"

// MISC //
#define BP_8 8
#define BP_6 6
#define FLAG 0x0F
#define FLAG_FF 0xFF
#define FALSE 0
#define TRUE  1
#define FREQ 80000u
#define VAL_0 0U
#define VAL_1 1U
#define REG_1B 0x1B
#define REG_1C 0x1C
#define REG_1D 0x1D

// UART //
typedef enum
{
	UART_0,
	UART_1,
	UART_2,
	UART_3,
	UART_4
}uart_name_t;

#define UART_CLK_FREQ    CLOCK_GetFreq(UART0_CLK_SRC)
#define UART_CLK_FREQ_U4 CLOCK_GetFreq(UART4_CLK_SRC)
#define UART_BAUDRATE 9600
#define UART4_BAUDRATE 19200

#define TX_U0 17u
#define RX_U0 16u
#define TX_U4 15u
#define RX_U4 14u
#define PIN0  0u

typedef enum{
	ZERO, ONE, TWO, THREE, FOUR, FIVE,
	SIX, SEVEN, EIGHT, NINE, TEN, ELEVEN,
	TWELVE, THIRTEEN, FOURTEEN, FIVETEEN,
	SIXTEEN, SEVENTEEN, EIGHTEEN, NINETEEN,
	TWENTY, TWENTYONE, TWENTYTWO, TWENTYTHREE,
	TWENTYFOUR, TWENTYFIVE, TWENTYSIX,
	TWENTYSEVEN, TWENTYEIGHT, TWENTYNINE, THIRTY
}typedef_magicnumbers;

#define FIVETYONE 51
#define FIVETYTWO 52
#define FOURTYTHREE 43
#define FOURTYEIGHT 48
#define THIRTYNINE 39
#define FIVETY 50
#define TWOHUNDRED 200
#define HUNDRED 100
#define THOUSAND 1000

// I2C //
#define SCL      2u
#define SDA      3u
#define SQW      0u
#define SQW_CONF 0x90 //0001 0000
#define SQW_REG  7U

#define I2C_MASTER_CLK_SRC          I2C0_CLK_SRC
#define I2C_MASTER_CLK_FREQ         CLOCK_GetFreq(I2C0_CLK_SRC)
#define I2C_MASTER_BASEADDR 		I2C0
#define WAIT_TIME                   10U

#define I2C_MASTER_SLAVE_ADDR_7BIT 0x7EU
#define I2C_BAUDRATE               400000U
#define I2C_DATA_LENGTH            33U

#define DIRECCION_ZERO 0x00U
#define DIRECTION_FOUR 0x04U
#define DIRECCION_ADDR 0x1200
#define DIRECCION_DHT 0x71
#define RTC_TYPE 0x68U
#define DTH_TYPE 0x38U
#define CALIBRATE_COMMAMD_AC 0xAC
#define CALIBRATE_COMMAMD_33 0x33
#define CALIBRATE_COMMAMD_00 0x00
#define EEPROM_TYPE 0x50U
#define DATA_SIZE 3
#define SQW_ADDR 0x07
#define SQW_REGCONF 0x10
#define MS 100000U
#define CORECLK 21000000
#define DELAY 10000U
#define HEXA_48 48
#define HEXA_10 10

typedef struct{
	uint8_t hrs; /** it contains the received data*/
	uint8_t min; /** it contains the received data*/
	uint8_t seg; /** it contains the received data*/
} rtc_time_t;

typedef struct{
	uint8_t day;
	uint8_t month;
	uint8_t year;
}rtc_date_t;

typedef struct
{
	uint8_t hrs;
	uint8_t min;
	uint8_t seg;
	uint8_t day;
	uint8_t month;
	uint8_t year;
	uint32_t humidity;
	uint32_t temperature;
}ram_data_t;

typedef struct
{
	uint8_t load_time;
	uint8_t count_time;
	uint8_t load_caps;
	uint8_t count_caps;
}logger_data_t;

// ASCII
// Menu Inicial
#define ASCII_ONE   49u
#define ASCII_TWO   50u
#define ASCII_TREE  51u
#define ASCII_FOUR  52u
#define ASCII_FIVE  53u
#define ASCII_SIX   54u
#define ASCII_SEVEN 55u
#define ASCII_EIGHT 56u
// Y-y & N-n
#define ASCII_Y     89u
#define ASCII_y     121u
#define ASCII_N     78u
#define ASCII_n     110u
// ENTER Y ESC
#define ASCII_ESC   27u
#define ASCII_ENTER 13u

// MACHINE STATE
typedef enum{
	hrs_s,
	min_s,
	seg_s
}time_state_t;

typedef enum{
	years_s,
	months_s,
	days_s
}date_state_t;

typedef enum{
	dectemp_s,
	temp_s,
	decftemp_s,
	ftemp_s
}temp_state_t;

typedef enum{
	dechum_s,
	hum_s
}humd_state_t;

// XY de UARTS
typedef enum{
	wdechrs_s,
	whrs_s,
	wdecmin_s,
	wmin_s,
	wdecseg_s,
	wseg_s
}write_state_t;

typedef enum{
	wdecyears_s,
	wyears_s,
	wdecmonths_s,
	wmonths_s,
	wdecdays_s,
	wdays_s
}write_date_t;

typedef enum{
	wdectemp_s,
	wtemp_s,
	wdecftemp_s,
	wftemp_s
}write_temp_t;

typedef enum{
	wdechum_s,
	whum_s
}write_humd_t;

typedef enum{
	wcentcap_s,
	wdeccap_s,
	wcap_s
}write_cap_t;

typedef enum{
	wdeccaps_s,
	wcaps_s
}write_seccap_t;

#define HEXA 0x0F

// Watchdog //
#define WDOG_WCT_INSTRUCITON_COUNT (256U)
#define TIME 0xBFDU

#endif /* CONFIG_H_ */
