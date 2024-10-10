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
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

TaskHandle_t cambio = NULL;
TaskHandle_t main_task_handle = NULL;


/*==================[internal functions declaration]=========================*/

/**
 * @brief Función que se ejecuta en la interrupción del temporizador A.
 *
 * Envía una notificación a la tarea encargada de la lectura de entrada analógica.
 * 
 * @param param Parámetro opcional (no utilizado).
 */

void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR( cambio, pdFALSE); 
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
		giro = !encendido;
	if (caracter == 'l')
		led = !hold;
	if (caracter == 'm')
		mapa=!mapa;
}

/*==================[external functions definition]==========================*/
void app_main(void){

}
/*==================[end of file]============================================*/