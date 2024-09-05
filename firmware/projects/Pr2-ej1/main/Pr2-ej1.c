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
#define CONFIG_BLINK_PERIOD 100


/*==================[internal data definition]===============================*/
bool hold=false;
bool encendido = true;
uint16_t distancia = 0;	


/*==================[internal functions declaration]=========================*/
static void medirdistancia(void *pvParameter)
{
	while (true)
	{
		if(encendido)
			{
				distancia=HcSr04ReadDistanceInCentimeters();
			}		
		vTaskDelay(CONFIG_BLINK_PERIOD1/portTICK_PERIOD_MS);
	}
	
}
/// @brief mide la distancia

static void valorteclas(void *pvParameter)
{
	uint8_t teclas;
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
	vTaskDelay(CONFIG_BLINK_PERIOD/portTICK_PERIOD_MS);	
	}
}
/// @brief modifica el valor de las teclas

void Leds()
{
	
			if (distancia < 10)
			{
				LedOff(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			}
			if (distancia >= 10 && distancia < 20)
			{
				LedOn(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			}
				if (distancia >= 20 && distancia < 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
			}
				if (distancia >= 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOn(LED_3);
	
			}
}

/// @brief enciende los leds segun la distancia medida

static void mostrarDistancia(void *pvParameter)
{
	/*int aux=HcSr04ReadDistanceInCentimeters;*/
	while (true)
	{	
		if(encendido)
			{
				Leds();
			if (!hold)
				{
					LcdItsE0803Write(distancia);
				}
			}
			 else 
				{
					LcdItsE0803Off();
					LedsOffAll();
				}
		vTaskDelay(CONFIG_BLINK_PERIOD1/portTICK_PERIOD_MS);
	}
}
/// @brief muestra la distancia

/*==================[external functions definition]==========================*/
void app_main(void)
{
LedsInit();
SwitchesInit();
LcdItsE0803Init();
HcSr04Init(GPIO_3,GPIO_2);

xTaskCreate(&medirdistancia, "mide", 2048, NULL, 5, NULL);
xTaskCreate(&valorteclas, "teclas", 512, NULL, 5, NULL);
xTaskCreate(&mostrarDistancia,"muestra",512,NULL,5,NULL);
}
/*==================[end of file]============================================*/