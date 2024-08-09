/*! @mainpage Ejercicio_3
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32       |
 * |:--------------:|:--------------|
 * |    PIN_X       |   GPIO_X      |
 *
 *
 * @section changelog Changelog
 *
 * |   Date     | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation                              |
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
/*==================[macros and definitions]=================================*/
#define ON 2
#define OFF 1
#define TOGGLE 0
#define CONFIG_BLINK_PERIOD1 100
/*==================[internal data definition]===============================*/


/*==================[internal functions declaration]=========================*/
struct leds
{
      uint8_t mode;       //ON, OFF, TOGGLE
    uint8_t n_led;        //indica el número de led a controlar
    uint8_t n_ciclos;   //indica la cantidad de ciclos de ncendido/apagado
    uint16_t periodo;    //indica el tiempo de cada ciclo
} my_leds; 

void parpadeoLEd(struct leds *LED){

{
    
    switch(LED->mode){
        case ON: 
        LED->n_led=1;
        if(LED->n_led==1) 
            {LedOn(LED_1);}
        if(LED->n_led==2)
            {LedOn(LED_2);}
        if(LED->n_led==3)
            {LedOn(LED_3);}
        break;

        case OFF:
        if(LED->n_led==1) 
            {LedOff(LED_1);}
        if(LED->n_led==2)
            {LedOff(LED_2);}
        if(LED->n_led==3)
            {LedOff(LED_3);}
        break;

        case TOGGLE:
        

        for(int i=0; i<LED->n_ciclos; i++)
        {
                if(LED->n_led==1)
                    {   
                    LedToggle(LED_1);
                    }
                if(LED->n_led==2)
                    {   
                    LedToggle(LED_2);   
                    }
                if(LED->n_led==3)
                    {   
                    LedToggle(LED_3);
                    }
                int aux=LED->periodo/CONFIG_BLINK_PERIOD1;
                for(size_t j=0; j<aux; j++)
                    {   
                        {
                        vTaskDelay(CONFIG_BLINK_PERIOD1/ portTICK_PERIOD_MS);
                        }
                    }
                }
        break;
        }
}
}

/*==================[external functions definition]==========================*/

void app_main(void){

    struct leds *LED;
    LED= &my_leds;
    LedsInit();

    LED->mode=TOGGLE;
    LED->n_ciclos=1000;
    LED->periodo=500;
    LED->n_led=1;
    parpadeoLEd(LED);
    while(true){}
    }