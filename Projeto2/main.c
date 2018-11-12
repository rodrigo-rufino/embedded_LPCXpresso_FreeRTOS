/*
    FreeRTOS V6.1.1 - Copyright (C) 2011 Real Time Engineers Ltd.
    This file is part of the FreeRTOS distribution.
    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.
    1 tab == 4 spaces!
    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.
    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.
    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

// Grupo:
//	Rodrigo Rufino Ribeiro
// 	Jéssica Camila
// 	Márcio Rotella

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"

#include "light.h"
#include "oled.h"
#include "rotary.h"
#include "rgb.h"

#include "basic_io.h"


static void lightSensor( void *pvParameters );
static void trimpot( void *pvParameters );
static void vLed( void *pvParameters );
static void vDisplay( void *pvParameters );

xQueueHandle xQueue;
int flagLED = 0;
static uint8_t buf[10];

xTaskHandle xTaskLedHandle;

struct sensor
{
	uint8_t sensor;
    uint32_t data;
};

static void init_ssp(void)
{
	SSP_CFG_Type SSP_ConfigStruct;
	PINSEL_CFG_Type PinCfg;

	/*
	 * Initialize SPI pin connect
	 * P0.7 - SCK;
	 * P0.8 - MISO
	 * P0.9 - MOSI
	 * P2.2 - SSEL - used as GPIO
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Funcnum = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 2;
	PINSEL_ConfigPin(&PinCfg);

	SSP_ConfigStructInit(&SSP_ConfigStruct);

	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(LPC_SSP1, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(LPC_SSP1, ENABLE);

}

static void init_i2c(void)
{
	PINSEL_CFG_Type PinCfg;

	/* Initialize I2C2 pin connect */
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);

	// Initialize I2C peripheral
	I2C_Init(LPC_I2C2, 100000);

	/* Enable I2C1 operation */
	I2C_Cmd(LPC_I2C2, ENABLE);
}

static void init_adc(void)
{
	PINSEL_CFG_Type PinCfg;

	/*
	 * Init ADC pin connect
	 * AD0.0 on P0.23
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 23;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);

	/* Configuration for ADC :
	 * 	Frequency at 1Mhz
	 *  ADC channel 0, no Interrupt
	 */
	ADC_Init(LPC_ADC, 1000000);
	ADC_IntConfig(LPC_ADC,ADC_CHANNEL_0,DISABLE);
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_0,ENABLE);

}

static void intToString(int value, uint8_t* pBuf, uint32_t len, uint32_t base)
{
    static const char* pAscii = "0123456789abcdefghijklmnopqrstuvwxyz";
    int pos = 0;
    int tmpValue = value;

    // the buffer must not be null and at least have a length of 2 to handle one
    // digit and null-terminator
    if (pBuf == NULL || len < 2)
    {
        return;
    }

    // a valid base cannot be less than 2 or larger than 36
    // a base value of 2 means binary representation. A value of 1 would mean only zeros
    // a base larger than 36 can only be used if a larger alphabet were used.
    if (base < 2 || base > 36)
    {
        return;
    }

    // negative value
    if (value < 0)
    {
        tmpValue = -tmpValue;
        value    = -value;
        pBuf[pos++] = '-';
    }

    // calculate the required length of the buffer
    do {
        pos++;
        tmpValue /= base;
    } while(tmpValue > 0);


    if (pos > len)
    {
        // the len parameter is invalid.
        return;
    }

    pBuf[pos] = '\0';

    do {
        pBuf[--pos] = pAscii[value % base];
        value /= base;
    } while(value > 0);

    return;

}

int main( void )
{
    xQueue = xQueueCreate( 7, sizeof( struct sensor ) );

	init_i2c();
	init_ssp();
	init_adc();
	oled_init();
	rotary_init();
	rgb_init();
	light_init();
	light_enable();
	light_setRange(LIGHT_RANGE_4000);

	oled_clearScreen(OLED_COLOR_WHITE);
	oled_putString(1,9,  (uint8_t*)"Light  : ", OLED_COLOR_BLACK, OLED_COLOR_WHITE);
	oled_putString(1,17, (uint8_t*)"Trimpot: ", OLED_COLOR_BLACK, OLED_COLOR_WHITE);

	if( xQueue != NULL )
	{
		// Luminosidade
		xTaskCreate( lightSensor, "Sender1", 240, NULL, 1, NULL );
		// Trimpot
		xTaskCreate( trimpot, "Sender2", 240, NULL, 2, NULL );

		xTaskCreate( vLed, "Led", 240, NULL, 2, NULL );

		xTaskCreate( vDisplay, "Display", 240, NULL, 1, NULL );

		/* Start the scheduler so the created tasks start executing. */
		vTaskStartScheduler();
	}
	else
	{
		/* The queue could not be created. */
	}

    for( ;; );
	return 0;
}

/*-----------------------------------------------------------*/
// Sensor de Luminosidade
static void lightSensor( void *pvParameters )
{
portBASE_TYPE xStatus;
const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

struct sensor lightSensor;
lightSensor.sensor = 0;

	for( ;; )
	{
		lightSensor.data = light_read();
		xStatus = xQueueSendToBack( xQueue, &lightSensor, 0);

		vTaskDelay(100);
	}
}

static void trimpot( void *pvParameters )
{
const portTickType xTicksToWait = 100 / portTICK_RATE_MS;
portBASE_TYPE xStatus;

struct sensor trimpot;
trimpot.sensor = 1;

	for( ;; )
	{
		ADC_StartCmd(LPC_ADC,ADC_START_NOW);
		while (!(ADC_ChannelGetStatus(LPC_ADC,ADC_CHANNEL_0,ADC_DATA_DONE)));
		trimpot.data = ADC_ChannelGetData(LPC_ADC,ADC_CHANNEL_0);

		xStatus = xQueueSendToBack( xQueue, &trimpot, 0);

		vTaskDelay(500);
	}
}

/*-----------------------------------------------------------*/
static void vDisplay( void *pvParameters )
{
/* Declare the variable that will hold the values received from the queue. */
struct sensor receivedSensor;
portBASE_TYPE xStatus;

	/* This task is also defined within an infinite loop. */
	for( ;; )
	{
		//if( uxQueueMessagesWaiting( xQueue ) != 0 )
		//{
		//	vPrintString( "Queue should have been empty!\r\n" );
		//}

		xStatus = xQueueReceive( xQueue, &receivedSensor, 0 );

		if( xStatus == pdPASS )
		{
			/* Data was successfully received from the queue, print out the received
			value. */
			vPrintString( "-----------------------\n" );
			vPrintStringAndNumber( "Sensor = ", receivedSensor.sensor );
			vPrintStringAndNumber( "Value = ", receivedSensor.data );

			if (receivedSensor.sensor == 0){
				intToString(receivedSensor.data, buf, 10, 10);
				oled_fillRect((1+9*6),9, 80, 16, OLED_COLOR_WHITE);
				oled_putString((1+9*6),9, buf, OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			} else {
				intToString(receivedSensor.data, buf, 10, 10);
				oled_fillRect((1+9*6),17, 80, 24, OLED_COLOR_WHITE);
				oled_putString((1+9*6),17, buf, OLED_COLOR_BLACK, OLED_COLOR_WHITE);
			}
		}
	}
}

static void vLed( void *pvParameters )
{
/* Declare the variable that will hold the values received from the queue. */
struct sensor receivedSensor;
portBASE_TYPE xStatus;

	/* This task is also defined within an infinite loop. */
	for( ;; )
	{
		//if( uxQueueMessagesWaiting( xQueue ) != 0 )
		//{
		//	vPrintString( "Queue should have been empty!\r\n" );
		//}

		xStatus = xQueueReceive( xQueue, &receivedSensor, 0 );

		if( xStatus == pdPASS )
		{
			if (receivedSensor.sensor == 1){
				flagLED = receivedSensor.sensor;
				if (receivedSensor.data >= 0.8*4096) {
					rgb_setLeds(RGB_RED);
				} else rgb_setLeds(RGB_GREEN);
			}
		}

		xStatus = xQueueSendToFront( xQueue, &receivedSensor, 0);

		vTaskDelay(50);
	}

}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* This function will only be called if an API call to create a task, queue
	or semaphore fails because there is too little heap RAM remaining. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	/* This function will only be called if a task overflows its stack.  Note
	that stack overflow checking does slow down the context switch
	implementation. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* This example does not use the idle hook to perform any processing. */
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* This example does not use the tick hook to perform any processing. */
}
