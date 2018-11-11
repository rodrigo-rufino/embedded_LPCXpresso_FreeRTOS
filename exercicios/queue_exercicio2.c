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
static void vReceiver1( void *pvParameters );
static void vReceiver2( void *pvParameters );

xQueueHandle xQueue1;
xQueueHandle xQueue2;
int valor1=100, valor2=200;

int count = 0;

int main( void )
{
    xQueue1 = xQueueCreate( 5, sizeof( int ) );
    xQueue2 = xQueueCreate( 5, sizeof( int ) );

	if( xQueue1 != NULL && xQueue2 != NULL)
	{
		xTaskCreate( vSender1, "Sender1", 240, NULL, 1, NULL );

		xTaskCreate( vReceiver1, "Receiver1", 240, NULL, 1, NULL );
		xTaskCreate( vReceiver2, "Receiver2", 240, NULL, 1, NULL );

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
portBASE_TYPE xStatus1;
portBASE_TYPE xStatus2;
const portTickType xTicksToWait = 100 / portTICK_RATE_MS;

	for( ;; )
	{
		if (count <=40){
			xStatus1 = xQueueSendToBack( xQueue1, &count, 0);
			if( xStatus1 != pdPASS )
			{
				//vPrintString( "Observador: Fila 1 Cheia\n" );
			}
			else
			{
				vPrintStringAndNumber("Observador - ",count);
			}
		} else if (count >= 50){
			xStatus2 = xQueueSendToBack( xQueue2, &count, 0);
			if( xStatus2 != pdPASS )
			{
				//vPrintString( "Observador: Fila 1 Cheia\n" );
			}
			else
			{
				vPrintStringAndNumber("Observador - ",count);
			}
		}

		/* Allow the other sender task to execute. */
		//taskYIELD();

		count++;
		vTaskDelay(50);
	}
}

/*-----------------------------------------------------------*/

static void vReceiver1( void *pvParameters )
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
			vPrintStringAndNumber( "Tratador1 = ", valor );
			alerta++;
			if (alerta == 10){
				vPrintString("Alerta: Tratador1 recebeu 10 mensagens.");
				alerta = 0;
			}
		}
		else
		{
			//vPrintString( "Tratador1: Fila vazia\n" );
		}
		vTaskDelay(50);
	}
}
/*-----------------------------------------------------------*/

static void vReceiver2( void *pvParameters )
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

		xStatus = xQueueReceive( xQueue2, &valor, 0 );

		if( xStatus == pdPASS )
		{
			/* Data was successfully received from the queue, print out the received
			value. */
			vPrintStringAndNumber( "Tratador2 = ", valor );
			alerta++;
			if (alerta == 10){
				vPrintString("Alerta: Tratador2 recebeu 10 mensagens.");
				alerta = 0;
			}
		}
		else
		{
			//vPrintString( "Tratador2: Fila vazia\n" );
		}
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


