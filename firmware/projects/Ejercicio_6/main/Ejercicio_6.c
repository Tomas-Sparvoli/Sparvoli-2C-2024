/*! @mainpage Ejercicio_6
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
#include "math.h"
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
	for (int i=0;i<digits;i++)
	{
		bcd_number[i]=data%10;						/*divido y me quedo con lo que queda despues de la coma*/ 
		data=data/10;								/*divido y elimino lo que queda despues de la coma*/
	}	
	return 0;
}

typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;

void mapearbits(gpioConf_t *arreglo, uint8_t digits)
{	printf("llegue hasta antes del if ");
	for(int i=0;i<4;i++)
	{
		if(digits&1<<i)
			{GPIOOn(arreglo[i].pin);				/* si la comparacion = TRUE pongo en alto el pin */
			}
		else
			{GPIOOff(arreglo[i].pin);				/* si la comparacion = FALSe pongo en bajo el pin */
			}
	}
}

void multipelxor (uint32_t numero, uint8_t digito, gpioConf_t *arreglo, gpioConf_t *mult)
{
	uint8_t vector[3];
	convertToBcdArray(numero,digito,vector);
	for (int i=0;i<3;i++)			/*el for lo recorro al reves ya que yo odrene el numero menos singificativo en el primer dato */
	{
		mapearbits(arreglo,vector[i]);
		printf ("digito; %d",vector[i]);
		GPIOOn(mult[i].pin);
		GPIOOff(mult[i].pin);
	}
}


/*==================[external functions definition]==========================*/
void app_main(void)
{
	printf("Hello world!\n");

	uint32_t data = 123;
	uint8_t digits = 3;
	uint8_t bcd_number[digits];

	/*convertToBcdArray (data,digits,bcd_number);
	{
		for (int i=0; i<digits;i++)
	{
		printf("el valor de cada digito es:%d ",bcd_number[i]);
	}	
	}  ejercicio 4*/

	gpioConf_t arreglo[]= 	{										/*inicializo un arreglo colocndo los pines que utilizo 
																	y las delaro como salidas */
								{GPIO_20,GPIO_OUTPUT},
								{GPIO_21,GPIO_OUTPUT},
								{GPIO_22,GPIO_OUTPUT},
								{GPIO_23,GPIO_OUTPUT},
							};

	gpioConf_t mult[]=	{
								{GPIO_9,GPIO_OUTPUT},
								{GPIO_18,GPIO_OUTPUT},
								{GPIO_19,GPIO_OUTPUT},
							};

	for (int i = 0; i<4; i++)
		{
			GPIOInit(arreglo[i].pin, arreglo[i].dir);
		}
	
	for (int i = 0; i<3; i++)
		{
			GPIOInit(mult[i].pin,mult[i].dir);
		}
	multipelxor(data,digits,arreglo,mult);
}
/*==================[end of file]============================================*/