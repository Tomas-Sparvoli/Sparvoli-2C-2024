/*! @mainpage Pr2-ej1
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
#include "hc_sr04.h"
#include "lcditse0803.h"
#include "switch.h"
#include "Pr2-ej1.h"

/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD1 1000
#define CONFIG_BLINK_PERIOD 500


/*==================[internal data definition]===============================*/
bool hold=false;
bool encendido = true;
int16_t distancia = 0;	
int8_t teclas;

/*==================[internal functions declaration]=========================*/
void valorteclas()
{
	while(true)
	{
		teclas=SwitchesRead();
		switch (teclas)
		{
			case SWITCH_1:
			encendido=!encendido;
			break;
	
			case SWITCH_2:
			hold=!hold;
			break;
		}
	vTaskDelay(CONFIG_BLINK_PERIOD1/portTICK_PERIOD_MS);	
	}
}
/// @brief modifica el valor de las teclas
void medirdistancia()
{
	while (true)
	{
		if(encendido)
			{distancia=HcSr04ReadDistanceInCentimeters;}
		vTaskDelay(CONFIG_BLINK_PERIOD1/portTICK_PERIOD_MS);
	}
	
}
/// @brief mide la distancia

void mostrarDistancia()
{
	while (true)
	{
		if(encendido || hold)
			{LcdItsE0803Write(distancia);}
		vTaskDelay(CONFIG_BLINK_PERIOD1/portTICK_PERIOD_MS);
	}
}
/// @brief muestra la distancia

void Leds()
{	while(true)
	{	
		if (distancia < 10)
		{
			LedOff(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}
		if (distancia > 10 && distancia < 20)
		{
			LedOn(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}
			if (distancia > 20 && distancia < 30)
		{
			LedOn(LED_1);
			LedOn(LED_2);
			LedOff(LED_3);
		}
			if (distancia >30)
		{
			LedOn(LED_1);
			LedOn(LED_2);
			LedOn(LED_3);
		}
NewFunction();
	}
}
/*==================[external functions definition]==========================*/
void NewFunction()
{
	vTaskDelay(CONFIG_BLINK_PERIOD/portTICK_PERIOD_MS);
}
void app_main(void)
{
LedsInit();
SwitchesInit();
xTaskCreate(&valorteclas, "teclas", 512, NULL, 1, NULL);
xTaskCreate(&medirdistancia, "mide", 512, NULL, 1, NULL);
xTaskCreate(&mostrarDistancia, "muestra", 512, NULL, 1, NULL);
}
/*==================[end of file]============================================*/