/*
 * gpio.c
 *      Author:
 *      Sonny Ceja Celis
 *      Sergio Borrayo Anzaldo
 */

//#include "MK64F12.h"
#include "gpio.h"

#include "fsl_port.h"
#include "fsl_clock.h"
#include "config.h"

volatile static gpio_interrupt_flags_t g_intr_status_flag = {FALSE};

static void (*gpio_C_callback)(uint32_t flags) = FALSE;
static void (*gpio_A_callback)(uint32_t flags) = FALSE;
static void (*gpio_B_callback)(uint32_t flags) = FALSE;
static void (*gpio_D_callback)(uint32_t flags) = FALSE;

// iniciar callbacks del gpio //
void GPIO_callback_init(gpio_name_t gpio, void (*handler)(uint32_t flags))
{
	if(GPIO_A == gpio)
	{
		gpio_A_callback = handler;
	}
	if(GPIO_C == gpio)
	{
		gpio_C_callback = handler;
	}
	if(GPIO_D == gpio)
	{
		gpio_D_callback = handler;
	}
	else
	{
		gpio_B_callback = handler;
	}
}

// HANDLER del puerto C //
void PORTC_IRQHandler(void)
{
	uint32_t irq_status = FALSE;

	irq_status = GPIO_PortGetInterruptFlags(GPIOC);
	if(gpio_C_callback)
	{
		gpio_C_callback(irq_status);
	}

	GPIO_PortClearInterruptFlags(GPIOC, irq_status);

}

// HANDLER del puerto A //
void PORTA_IRQHandler(void)
{
	uint32_t irq_status = FALSE;

	irq_status = GPIO_PortGetInterruptFlags(GPIOA);
	if(gpio_A_callback)
	{
		gpio_A_callback(irq_status);
	}

	GPIO_PortClearInterruptFlags(GPIOA, irq_status);
}

// HANDLER del puerto B //
void PORTB_IRQHandler(void)
{
	uint32_t irq_status = FALSE;

	irq_status = GPIO_PortGetInterruptFlags(GPIOB);
	if(gpio_B_callback)
	{
		gpio_B_callback(irq_status);
	}

	GPIO_PortClearInterruptFlags(GPIOB, irq_status);
}

//HANDLER DEL PUERTO D//
void PORTD_IRQHandler(void)
{
	uint32_t irq_status = FALSE;

	irq_status = GPIO_PortGetInterruptFlags(GPIOD);
	if(gpio_D_callback)
	{
		gpio_D_callback(irq_status);
	}

	GPIO_PortClearInterruptFlags(GPIOD, irq_status);
}

// iniciar puertos GPIO //
void GPIO_init(void)
{
	gpio_pin_config_t gpio_input_config = {
			        kGPIO_DigitalInput,
			        FALSE,
			    };

	gpio_pin_config_t gpio_output_config = {
			        kGPIO_DigitalOutput,
			        TRUE,
			    };

	const port_pin_config_t button_config = {
			kPORT_PullUp,                                            /* Internal pull-up resistor is enabled */
			kPORT_FastSlewRate,                                      /* Fast slew rate is configured */
			kPORT_PassiveFilterEnable,                              /* Passive filter is disabled */
			kPORT_OpenDrainDisable,                                  /* Open drain is disabled */
			kPORT_HighDriveStrength,                                 /* High drive strength is configured */
			kPORT_MuxAsGpio,                                         /* Pin is configured as PTA4 */
			kPORT_UnlockRegister                                     /* Pin Control Register fields [15:0] are not locked */
		  };

		CLOCK_EnableClock(kCLOCK_PortB);

		PORT_SetPinMux(PORTB, 9u, kPORT_MuxAsGpio);
		GPIO_PinInit(GPIOB, 9u, &gpio_output_config);

		PORT_SetPinMux(PORTB, 23u, kPORT_MuxAsGpio);
		GPIO_PinInit(GPIOB, 23u, &gpio_output_config);
}

void GPIO_PIN_MODE(uint32_t pin, uint8_t mode)
{
	gpio_pin_config_t gpio_input_config = {
					kGPIO_DigitalInput,
					FALSE,
				};

	gpio_pin_config_t gpio_output_config = {
					kGPIO_DigitalOutput,
					TRUE,
				};
	const port_pin_config_t input_config = {
			kPORT_PullUp,                                            /* Internal pull-up resistor is enabled */
			kPORT_FastSlewRate,                                      /* Fast slew rate is configured */
			kPORT_PassiveFilterEnable,                              /* Passive filter is disabled */
			kPORT_OpenDrainDisable,                                  /* Open drain is disabled */
			kPORT_HighDriveStrength,                                 /* High drive strength is configured */
			kPORT_MuxAsGpio,                                         /* Pin is configured as PTA4 */
			kPORT_UnlockRegister                                     /* Pin Control Register fields [15:0] are not locked */
		  };

	if(mode)
	{
		PORT_SetPinConfig(PORTB, pin, &input_config); //modificar puerto y pin
		GPIO_PinInit(GPIOB, pin, &gpio_input_config);
	}
	else
	{
		PORT_SetPinMux(PORTB, pin, kPORT_MuxAsGpio);
		GPIO_PinInit(GPIOB, pin, &gpio_output_config);
	}
}

// Limpiar la bandera del interrupcion //
void GPIO_clear_irq_status(gpio_name_t gpio)
{
	if(GPIO_A == gpio)
	{
		g_intr_status_flag.flag_port_a = FALSE;
	}
	if(GPIO_C == gpio)
	{
		g_intr_status_flag.flag_port_c = FALSE;
	}
	if(GPIO_D == gpio)
	{
		g_intr_status_flag.flag_port_d = FALSE;
	}
	else
	{
		g_intr_status_flag.flag_port_b = FALSE;
	}
}

// Obtener el status de la interrupcion //
uint8_t GPIO_get_irq_status(gpio_name_t gpio)
{
	uint8_t status = false;

	if(GPIO_A == gpio)
	{
		status = g_intr_status_flag.flag_port_a;
	}
	if(GPIO_C == gpio)
	{
		status = g_intr_status_flag.flag_port_c;
	}
	if(GPIO_D == gpio)
	{
		status = g_intr_status_flag.flag_port_d;
	}
	else
	{
		status = g_intr_status_flag.flag_port_b;
	}

	return(status);
}

// Limpiar puertos //
void GPIO_Port_Clear(gpio_name_t gpio, uint32_t mask)
{
	switch(gpio)
	{
	case(GPIO_B):
		GPIO_PortClear(GPIOB, mask);
	break;
	case(GPIO_C):
		GPIO_PortClear(GPIOC, mask);
	break;
	case(GPIO_D):
		GPIO_PortClear(GPIOD, mask);
	break;
	case(GPIO_E):
		GPIO_PortClear(GPIOE, mask);
	break;
	default:
		GPIO_PortClear(GPIOA, mask);
	break;
	}

}

// Establecer puerto //
void GPIO_Port_Set(gpio_name_t gpio, uint32_t mask)
{
	switch(gpio){
		case(GPIO_B):
			GPIO_PortSet(GPIOB, mask);
		break;
		case(GPIO_C):
			GPIO_PortSet(GPIOC, mask);
		break;
		case(GPIO_D):
			GPIO_PortSet(GPIOD, mask);
		break;
		case(GPIO_E):
			GPIO_PortSet(GPIOE, mask);
		break;
		default:
			GPIO_PortSet(GPIOA, mask);
		break;
	}

}

uint32_t GPIO_PORT_READ(gpio_name_t gpio, uint32_t mask)
{
	uint32_t salida;

	switch(gpio){
		case(GPIO_B):
				salida = GPIO_PinRead(GPIOB, mask);
		break;
		case(GPIO_C):
		salida = GPIO_PinRead(GPIOC, mask);
		break;
		case(GPIO_D):
		salida = GPIO_PinRead(GPIOD, mask);
		break;
		case(GPIO_E):
		salida = GPIO_PinRead(GPIOE, mask);
		break;
		default:
			salida = GPIO_PinRead(GPIOA, mask);
		break;
	}

	return (salida);
}

