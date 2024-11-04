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
 * | Entrada A/D   | CH1     |
 * | Entrada A/D   | CH2     |
 * | SENSOR_TRIG   | GPIO_3  |
 * | SENSOR_ECHO   | GPIO_2  |
 * | LED_1         | GPIO_A  |
 * | LED_2         | GPIO_B  |
 * | LED_3         | GPIO_C  |
 * | Buzzer        | GPIO_12 |
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
#include "analog_io_mcu.h"
/*==================[macros and definitions]=================================*/
/**
 * @brief Valor de distancia medida por el sensor HC-SR04.
 */
uint32_t distancia = 0;

/**
 * @brief Valor de distancia convertida de cm a m.
 */
uint16_t distancia_referencia=0;

/**
 * @brief Handle para la tarea de medición de distancia.
 */
TaskHandle_t Mido = NULL;

/**
 * @brief Handle para la tarea de muestra de distancia.
 */
TaskHandle_t Cabeza = NULL;

/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/**
 * @fn FuncTimerA
 * @brief Función que se ejecuta en la interrupción del temporizador A.
 * 
 * @param param Parámetro opcional (no utilizado en esta función).
 */
void FuncTimerA(void* param){
    vTaskNotifyGiveFromISR( Mido, pdFALSE);    
}

/**
 * @fn FuncTimerB
 * @brief Función invocada en la interrupción del timer B
*/
void FuncTimerB(void* param){
    vTaskNotifyGiveFromISR(Cabeza, pdFALSE);    
}

/**
 * @fn Leds 
 * @brief Controla el estado de los LEDs en función de la distancia medida.
 * 
 * Esta función enciende o apaga los LEDs basándose en el valor de `distancia_referencia `.
 */
void Leds()
{
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
 * @fn medirdistancia
 * @brief Tarea de FreeRTOS que mide la distancia utilizando el sensor HC-SR04.
 * 
 * Esta función lee continuamente la distancia del sensor y actualiza la variable `distancia`.
 * 
 */
void medirdistancia(void *pvParameter)
{
	while (true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); 
		distancia=HcSr04ReadDistanceInCentimeters();
		distancia_referencia=distancia/100; //paso la unida de cm a m
		Leds();
	}
}


/**
 * @fn buzzer
 * @brief Tarea encargada en manejar la salida conectada al buzzer
 * 
 */
void buzzer ()
{	
	GPIOInit(GPIO_12,GPIO_OUTPUT);
	while (true)
	{
		if (distancia_referencia <= 5 && distancia_referencia > 3)	//suena el buzzer a una frecuencia de 1s
		{
			GPIOOn(GPIO_12);
			vTaskDelay(1000 / portTICK_PERIOD_MS);	
			GPIOOff(GPIO_12);
			vTaskDelay(1000 / portTICK_PERIOD_MS);	
			}
		if (distancia_referencia <= 3)							//suena el buzzer a una frecuencia de 0,5s
		{
			GPIOOn(GPIO_12);
			vTaskDelay(500 / portTICK_PERIOD_MS);	
			GPIOOff(GPIO_12);
			vTaskDelay(500 / portTICK_PERIOD_MS);	
		}
	}
}


/**
 * @fn Notificacion
 * @brief Tarea encargada en notificar via Bt si esta un situacion de precaucion o peligro
 */
void notificacion ()
{
	while(true)
	{
		if (distancia_referencia <= 5 && distancia_referencia > 3)	//envio notificacion de precaucion
		{
			UartSendString(UART_CONNECTOR, "Precaución, vehículo cerca");
		}
		if (distancia_referencia <= 3)							//envio notificacion de peligro
		{
			UartSendString(UART_CONNECTOR, "Peligro, vehículo cerca");
		}
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}


/**
 * @fn sensorCabeza
 * @brief Tarea encargada en notificar via Bt si la persona se cayo
 */
void sensorCabeza()
{
	uint16_t ejex; //genero las variables para guardar los datos de cada eje 
	uint16_t ejey;
	uint16_t ejez;

	uint16_t ejexG; //genero las variables para realizar los calculos correspondientes  
	uint16_t ejeyG;
	uint16_t ejezG;

	uint16_t total;	
	while (true)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		AnalogInputReadSingle(CH1,&ejex);
		AnalogInputReadSingle(CH2,&ejey);
		AnalogInputReadSingle(CH3,&ejez);

		ejexG=0.3*ejex+1.65;
		ejeyG=0.3*ejey+1.65;
		ejezG=0.3*ejez+1.65;
		total=ejexG+ejeyG+ejezG;

		if(total>5)
		{UartSendString(UART_CONNECTOR, "caida detectada");}

	}
	
}

/*==================[external functions definition]==========================*/
void app_main(void){

	timer_config_t timer_sensor = {
		.timer = TIMER_A,
		.period = 500000,		//muestro la distancia cada 1/2 segundo
		.func_p = FuncTimerA,
		.param_p = NULL
	};
	TimerInit(&timer_sensor);
	
	timer_config_t timer_cabeza = {
		.timer = TIMER_B,
		.period = 10000,		//frecuencia de 100hz en 100000us
		.func_p = FuncTimerB,
		.param_p = NULL
	};
	TimerInit(&timer_cabeza);

	serial_config_t Btexterno = 
	{
		.port = UART_CONNECTOR,
		.baud_rate = 9600,
		.func_p = NULL,
		.param_p = NULL,
	};
	UartInit(&Btexterno);

	// Configuración de entrada analógica
    analog_input_config_t configejex = {
        .input = CH1,
        .mode = ADC_SINGLE,
        .func_p = NULL,
        .param_p = NULL,
        .sample_frec = 0
    };
    AnalogInputInit(&configejex);

	// Configuración de entrada analógica
    analog_input_config_t configejey = {
        .input = CH2,
        .mode = ADC_SINGLE,
        .func_p = NULL,
        .param_p = NULL,
        .sample_frec = 0
    };
    AnalogInputInit(&configejey);

		// Configuración de entrada analógica
    analog_input_config_t configejez = {
        .input = CH3,
        .mode = ADC_SINGLE,
        .func_p = NULL,
        .param_p = NULL,
        .sample_frec = 0
    };
    AnalogInputInit(&configejez);

LedsInit();
HcSr04Init(GPIO_3, GPIO_2);
xTaskCreate(&medirdistancia, "mide",2048,NULL,5,&Mido);
xTaskCreate(&buzzer,"suena el buzzer",1024,NULL,5,NULL);
xTaskCreate(&notificacion,"notifica",1024,&Btexterno,5,NULL);
xTaskCreate(&sensorCabeza,"notifica",2048,&Btexterno,5,&Cabeza);
TimerStart(timer_sensor.timer);
TimerStart(timer_cabeza.timer);
}
/*==================[end of file]============================================*/