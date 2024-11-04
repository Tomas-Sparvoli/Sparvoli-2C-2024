/*! @mainpage Examen_EPRO
 *
 * @section genDesc Descripción General
 *
 * Programa correspondiente al examen de EPRO
 * 
 * @section hardConn Conexión de Hardware
 *
 * | Periférico    | ESP32   |
 * |:-------------:|:-------:|
 * | Entrada A/D   | GPIO_0  |
 * | Salida D/A    | GPIO_1  |
 *
 * @section changelog Historial de Cambios
 *
 * | Fecha        | Descripción                                  |
 * |:------------:|:---------------------------------------------|
 * | 04/11/2024   | Creación del documento                       |
 *
 * @section author Autor
 *
 * Tomás Mateo Sparvoli (tomas.sparvoli@ingenieria.uner.edu.ar)
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
#include "timer_mcu.h"
#include "uart_mcu.h"
/*==================[macros and definitions]=================================*/
/**
 * @brief Valor de distancia medida por el sensor HC-SR04.
 */
uint32_t distancia = 0;

/**
 * @brief Handle para la tarea de medición de distancia.
 */
TaskHandle_t Mido = NULL;

/**
 * @brief Handle para la tarea de muestra de distancia.
 */
TaskHandle_t Muestro = NULL;

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/**
 * @brief Función que se ejecuta en la interrupción del temporizador A.
 * 
 * @param param Parámetro opcional (no utilizado en esta función).
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR( Mido, pdFALSE);    /* Envía una notificación a la tarea asociada al LED_1 */
}

/**
 * @brief Controla el estado de los LEDs en función de la distancia medida.
 * 
 * Esta función enciende o apaga los LEDs basándose en el valor de `distancia`.
 */
void Leds()
{
	uint16_t distancia_referencia=distancia/100; //paso la unida de cm a m
	if (distancia_referencia > 5)
	{
		LedOn(LED_1);	//solo enciendo el led verde 
		LedOff(LED_2);
		LedOff(LED_3);
	}
	if (distancia_referencia <= 5 && distancia_referencia > 3)
	{
		LedOn(LED_1);	//enciendo led verde
		LedOn(LED_2);	//enciendo led amarillo
		LedOff(LED_3);
	}
	if (distancia_referencia <= 3)
	{
		LedOn(LED_1);	//enciendo led verde
		LedOn(LED_2);	//enciendo led amarillo
		LedOn(LED_3);	//enciendo led rojo
	}
}

/**
 * @brief Tarea de FreeRTOS que mide la distancia utilizando el sensor HC-SR04.
 * 
 * Esta función lee continuamente la distancia del sensor y actualiza la variable `distancia`.
 * 
 * @param pvParameter Parámetro opcional (no se utiliza en esta función).
 */
void medirdistancia(void *pvParameter)
{
	while (true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); 
		distancia=HcSr04ReadDistanceInCentimeters();
		Leds();

	}
}

/*==================[external functions definition]==========================*/
void app_main(void){

	timer_config_t timer_sensor = {
		.timer = TIMER_A,
		.period = 1000000,		//muestro la distancia cada 1 segundo
		.func_p = FuncTimerA,
		.param_p = NULL
	};
	TimerInit(&timer_sensor);

HcSr04Init(GPIO_3, GPIO_2);
xTaskCreate(&medirdistancia, "mide",2048,NULL,5,&Mido);
TimerStart(timer_sensor.timer);
}
/*==================[end of file]============================================*/