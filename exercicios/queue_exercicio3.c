/*
    Grupo:
		- Márcio Rotella
		- Jéssica Camila
		- Rodrigo Rufino

*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Demo includes. */
#include "basic_io.h"


static void vSender1( void *pvParameters );
static void vSender2( void *pvParameters );
static void vReceiver( void *pvParameters );

xQueueHandle xQueue1;
xQueueHandle xQueue2;
int valor1=100, valor2=200;

int count = 0;

int main( void )
{
    xQueue1 = xQueueCreate( 10, sizeof( int ) );
    xQueue2 = xQueueCreate( 10, sizeof( int ) );

	if( xQueue1 != NULL && xQueue2 != NULL)
	{
		xTaskCreate( vSender1, "Sender1", 240, NULL, 1, NULL );
		xTaskCreate( vSender2, "Sender1", 240, NULL, 1, NULL );

		xTaskCreate( vReceiver, "Receiver", 240, NULL, 1, NULL );

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

static void vSender1( void *pvParameters )
{
long lValueToSend;
portBASE_TYPE xStatus;
const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

	for( ;; )
	{
		xStatus = xQueueSendToBack( xQueue1, &count, 0);

		count++;
		vTaskDelay(50);
	}
}

static void vSender2( void *pvParameters )
{
long lValueToSend;
portBASE_TYPE xStatus;
const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

	for( ;; )
	{
		xStatus = xQueueSendToBack( xQueue2, &count, 0);

		count++;
		vTaskDelay(50);
	}
}

/*-----------------------------------------------------------*/

static void vReceiver( void *pvParameters )
{
/* Declare the variable that will hold the values received from the queue. */
int valor;
int alerta = 0;
portBASE_TYPE xStatus;

	/* This task is also defined within an infinite loop. */
	for( ;; )
	{
		//if( uxQueueMessagesWaiting( xQueue ) != 0 )
		//{
		//	vPrintString( "Queue should have been empty!\r\n" );
		//}

		xStatus = xQueueReceive( xQueue1, &valor, 0 );

		if( xStatus == pdPASS )
		{
			/* Data was successfully received from the queue, print out the received
			value. */
			vPrintStringAndNumber( "Buffer1 = ", valor );
		}


		xStatus = xQueueReceive( xQueue2, &valor, 0 );

		if( xStatus == pdPASS )
		{
			/* Data was successfully received from the queue, print out the received
			value. */
			vPrintStringAndNumber( "Buffer2 = ", valor );
		}
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


