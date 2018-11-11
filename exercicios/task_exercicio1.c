/*
    Grupo:
		- Márcio Rotella
		- Jéssica Camila
		- Rodrigo Rufino

*/

/* FreeRTOS.org includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo includes. */
#include "basic_io.h"

/* The two task functions. */
void vTask1( void *pvParameters );
void vTask2( void *pvParameters );

/* Used to hold the handle of Task2. */
xTaskHandle xTask2Handle;
xTaskHandle xTask1Handle;

 unsigned int cont = 0;

/*-----------------------------------------------------------*/

int main( void )
{
	/* Create the first task at priority 2.  This time the task parameter is
	not used and is set to NULL.  The task handle is also not used so likewise 
	is also set to NULL. */
	xTaskCreate( vTask1, "Task 1", 240, NULL, 1, &xTask1Handle );
         /* The task is created at priority 2 ^. */

	/* Create the second task at priority 1 - which is lower than the priority
	given to Task1.  Again the task parameter is not used so is set to NULL -
	BUT this time we want to obtain a handle to the task so pass in the address
	of the xTask2Handle variable. */
	xTaskCreate( vTask2, "Task 2", 240, NULL, 1, &xTask2Handle );
        /* The task handle is the last parameter ^^^^^^^^^^^^^ */

	/* Start the scheduler so our tasks start executing. */
	vTaskStartScheduler();	
	
	for( ;; );
	return 0;
}
/*-----------------------------------------------------------*/

void vTask1( void *pvParameters )
{
unsigned portBASE_TYPE uxPriority;

	/* This task will always run before Task2 as it has the higher priority.
	Neither Task1 nor Task2 ever block so both will always be in either the
	Running or the Ready state.

	Query the priority at which this task is running - passing in NULL means
	"return our own priority". */
	uxPriority = uxTaskPriorityGet( NULL );

	for( ;; )
	{
		/* Print out the name of this task. */
		//vPrintString( "Task1 is running\n" );
		cont++;
		vTaskDelay( 200 / portTICK_RATE_MS );

		if(cont>=60){
					vTaskResume(xTask2Handle);
				}

		if(cont>=120){
				vTaskDelete(xTask1Handle);
		}
		
		/* Setting the Task2 priority above the Task1 priority will cause
		Task2 to immediately start running (as then Task2 will have the higher 
		priority of the    two created tasks). */
		//vPrintString( "About to raise the Task2 priority\n" );
		//vTaskPrioritySet( xTask2Handle, ( uxPriority + 1 ) );
		vPrintStringAndNumber( "Task 1: ", cont);
		/* Task1 will only run when it has a priority higher than Task2.
		Therefore, for this task to reach this point Task2 must already have
		executed and set its priority back down to 0. */
	}
}

/*-----------------------------------------------------------*/

void vTask2( void *pvParameters )
{
unsigned portBASE_TYPE uxPriority;

	/* Task1 will always run before this task as Task1 has the higher priority.
	Neither Task1 nor Task2 ever block so will always be in either the
	Running or the Ready state.

	Query the priority at which this task is running - passing in NULL means
	"return our own priority". */
	uxPriority = uxTaskPriorityGet( NULL );

	for( ;; )
	{
		/* For this task to reach this point Task1 must have already run and
		set the priority of this task higher than its own.

		Print out the name of this task. */
		//vPrintString( "Task2 is running\n" );
		cont ++;
		vTaskDelay( 500 / portTICK_RATE_MS );


		if(cont>=30){
			vTaskSuspend(xTask2Handle);
		}
		if(cont>=60){
			vTaskResume(xTask2Handle);
		}
		if(cont>=120){
			vTaskDelete(xTask2Handle);
		}
		/* Set our priority back down to its original value.  Passing in NULL
		as the task handle means "change our own priority".  Setting the
		priority below that of Task1 will cause Task1 to immediately start
		running again. */
		//vPrintString( "About to lower the Task2 priority\n" );
		vPrintStringAndNumber( "Task 2: ", cont);
		//vTaskPrioritySet( NULL, ( uxPriority - 2 ) );
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


