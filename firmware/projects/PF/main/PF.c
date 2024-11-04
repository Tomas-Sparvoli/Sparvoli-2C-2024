/*! @mainpage PF
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
#include "analog_io_mcu.h"
#include "uart_mcu.h"
#include "timer_mcu.h"
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

TaskHandle_t cambiomotor = NULL;
TaskHandle_t main_task_handle = NULL;

bool giro = false;
bool led = false;
bool mapa = false;
uint8_t angulo = 0;
 
/*==================[internal functions declaration]=========================*/

typedef struct
{
	gpio_t pin;			/*!< GPIO pin number */
	io_t dir;			/*!< GPIO direction '0' IN;  '1' OUT*/
} gpioConf_t;


/**
 * @brief Función que se ejecuta en la interrupción del temporizador A.
 *
 * Envía una notificación a la tarea encargada de la lectura de entrada analógica.
 * 
 * @param param Parámetro opcional (no utilizado).
 */

void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR (cambiomotor, pdFALSE); 
}

/**
 * @brief Función que se ejecuta en la interrupción del temporizador B.
 *
 * Envía una notificación a la tarea encargada de mostrar la señal ECG.
 *
 * @param param Parámetro opcional (no utilizado).
 */

void FuncTimerB(void* param){
    vTaskNotifyGiveFromISR( main_task_handle, pdFALSE); 
}

/**
 * @brief Función que cambia el estado de las variables `encendido` y `hold` basándose en el input UART.
 * 
 * Esta función permite cambiar el estado de las variables mediante comandos enviados a través del puerto UART.
 * 
 * @param param Parámetro opcional (no se utiliza en esta función).
 */
void cambioEstado(void *param)
{
	uint8_t caracter;
	UartReadByte(UART_PC, &caracter);
	if (caracter == 'a')
		giro = !giro;
	if (caracter == 'l')
		led = !led;
	if (caracter == 'm')
		mapa=!mapa;
}

void motorpaso(gpioConf_t *arreglo)
{while(1)
	{		
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		{	if (giro==true)
			{
				if (arreglo[2].pin==false)
				{		for (int i = 0; i < 119; i++) //
					{
						GPIOOn(arreglo[0].pin);	
						GPIOOff(arreglo[1].pin);
						vTaskDelay(500 / portTICK_PERIOD_MS);
						GPIOOff(arreglo[0].pin);	
					}	}
				else 
				{ 
					for (int i = 0; i < 17; i++) //cantidad de pasos necesarios para que el motor gire 30 grados en ese periodo de tiempo 
					{
						GPIOOn(arreglo[0].pin);	//cmabio el estado logico de la salida del motor
						GPIOOn(arreglo[1].pin); //seleccionio el modo de giro contrario a la aguja del reloj 
						vTaskDelay(500 / portTICK_PERIOD_MS); //velocidad de giro
						GPIOOff(arreglo[0].pin); //cambio el estado de la salida del motor
					}	
				angulo = angulo + 30;
				}
			}
			giro=!giro;
		}
	}
}

/*==================[external functions definition]==========================*/

void app_main(void)
{
	timer_config_t timer_motor = 
	{
		.timer = TIMER_A,
		.period = 1000,
		.func_p = FuncTimerA,
		.param_p = NULL
	};
	TimerInit(&timer_motor);

	serial_config_t pantalla = 
	{
		.port = UART_PC,
		.baud_rate = 9600,
		.func_p = cambioEstado,
		.param_p = NULL,
	};
	UartInit(&pantalla);

		gpioConf_t arreglo[]= 	
		{
			{GPIO_2,GPIO_OUTPUT}, //pin de cantidad de pasos del motor
			{GPIO_3,GPIO_OUTPUT}, //pin de direccionamiento del motor
			{GPIO_1,GPIO_INPUT},  //pin fin de carrera
		};

	for (int i = 0; i<3; i++)
		{
			GPIOInit(arreglo[i].pin, arreglo[i].dir);
		}

xTaskCreate(&motorpaso, "cambio del motor", 2048, NULL, 5, &cambiomotor);
}
/*==================[end of file]============================================*/