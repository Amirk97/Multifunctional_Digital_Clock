#include "intercom.h"

xQueueHandle timeQueue;
xQueueHandle GUIQueue;
SemaphoreHandle_t xSemaphoreReadButton;
BaseType_t xHigherPriorityTaskWoken = NULL ;


void INTERCOM_init(void) {
	
	evt gui_evt;
	
	timeQueue = xQueueCreate( 20, sizeof(gui_evt));
	GUIQueue = xQueueCreate( 20, sizeof(gui_evt)); 
	
	/* This semaphor will be used to signal the occurance of a tenth of second interrupt 
	* in readbutton task */
	xSemaphoreReadButton = xSemaphoreCreateBinary();
	
	gui_evt.sig = INIT_SIG;
	xQueueSendToBack( GUIQueue, &gui_evt, portMAX_DELAY );
	
}

