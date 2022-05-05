#include "intercom.h"

xQueueHandle timeQueue;
xQueueHandle GUIQueue;

evt	time_evt;
evt gui_evt;
BaseType_t xHigherPriorityTaskWoken = NULL ;


void INTERCOM_init(void) {
	
	timeQueue = xQueueCreate( 20, sizeof(time_evt));
	GUIQueue = xQueueCreate( 20, sizeof(gui_evt)); 
	
	gui_evt.sig = INIT_SIG;
	
	xQueueSendToBack( GUIQueue, &gui_evt, portMAX_DELAY );
	
	
}