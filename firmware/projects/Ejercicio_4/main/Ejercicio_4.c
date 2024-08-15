/*! @mainpage Ejercicio_4
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
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/
int8_t  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number)
{
	for (int i=0; i<digits;i++)
	{
		bcd_number[i]=data%10;						/*divido y me quedo con lo que queda despues de la coma*/ 
		data=data/10;								/*divido y elimino lo que queda despues de la coma*/
	}	
	return 0;
}


/*==================[external functions definition]==========================*/
void app_main(void){
	printf("Hello world!\n");

	uint32_t data = 123;
	uint8_t digits = 3;
	uint8_t bcd_number[digits];

	convertToBcdArray (data,digits,bcd_number);
	{
		for (int i=0; i<digits;i++)
	{
		printf("el valor de cada digito es: %d/n",bcd_number[i]);
	}	
	}
}
/*==================[end of file]============================================*/