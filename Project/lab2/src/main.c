
#include <stdio.h>
#include "bsp.h"
#include "intercom.h"
#include "timers.h"
#include <string.h>


#define GUI_PRIORITY					tskIDLE_PRIORITY+1
#define BACKEND_PRIORITY			tskIDLE_PRIORITY+1
#define READ_BUTTON_PRIORITY 	tskIDLE_PRIORITY+3

// The definition of TCBs for each task
xTaskHandle hUpdateTime;
xTaskHandle hUpdateGUI;
xTaskHandle hReadButtons;

portTASK_FUNCTION_PROTO(vUpdateTime, pvParameters);
portTASK_FUNCTION_PROTO(vUpdateGUI, pvParameters);
portTASK_FUNCTION_PROTO(vReadButtons, pvParameters);

void active_default_state(evt const event, GUI_super_state_typedef *const super_state);
void dormant_default_state(evt const event, GUI_super_state_typedef *const super_state);
void super_active_state (evt const event, GUI_super_state_typedef *const super_state);


// ============================================================================
int main(void) {
	
	// Initializing task intercommuniccation tools
	INTERCOM_init();
	
	// Initializing the device drivers
	BSP_init();

	/* Creating three tasks for background operations, user-interface and reading pushbuttons */
	xTaskCreate(vUpdateTime, "KeepTime", configMINIMAL_STACK_SIZE, NULL, BACKEND_PRIORITY, &hUpdateTime);
	xTaskCreate(vUpdateGUI,  "UpdateGUI", configMINIMAL_STACK_SIZE, NULL, GUI_PRIORITY, &hUpdateGUI);
	xTaskCreate(vReadButtons, "ReadButtons", configMINIMAL_STACK_SIZE, NULL, READ_BUTTON_PRIORITY, &hUpdateGUI);
	
	vTaskStartScheduler(); // This should never return.
	
	// Will only get here if there was insufficient memory to create
	// the idle task.
	while(1);  
}

/* The backend task which is in charge of updating and changing the core variables
*/
portTASK_FUNCTION(vUpdateTime, pvParameters) {
	evt time_to_GUI_evt;
	evt time_evt;
	mode stopwatch = {0};
	mode clock = {0};

	while(1) {
		xQueueReceive(timeQueue, &time_evt, portMAX_DELAY);
		
		switch(time_evt.sig) {
			
			case SEC_SIG:
				update_time_reg(&clock.time);
				time_to_GUI_evt.sig = UPDATE_TIME_SIG;
				time_to_GUI_evt.time = clock.time;
				xQueueSendToBack(GUIQueue, &time_to_GUI_evt, portMAX_DELAY);
				break;

			case SUB_SEC_SIG:
				if(stopwatch.status == ENABLED) {
					update_sub_time_reg(&stopwatch.time);
					time_to_GUI_evt.sig = UPDATE_SUB_TIME_SIG;
					time_to_GUI_evt.time = stopwatch.time;
					xQueueSendToBack(GUIQueue, &time_to_GUI_evt, portMAX_DELAY);
				}
				break;

			case CLOCK_TIME_RQ_SIG:
				time_to_GUI_evt.sig = UPDATE_TIME_SIG;
				time_to_GUI_evt.time = clock.time;
				xQueueSendToBack(GUIQueue, &time_to_GUI_evt, portMAX_DELAY);
				break;

			case STPWCH_TIME_RQ_SIG:
				time_to_GUI_evt.sig = UPDATE_SUB_TIME_SIG;
				time_to_GUI_evt.time = stopwatch.time;
				xQueueSendToBack(GUIQueue, &time_to_GUI_evt, portMAX_DELAY);
				break;

			case TGGLE_STPWCH_SIG:
				toggle_stpwtch_status(&stopwatch);
				break;

			case RST_STPWCH_SIG:
				if (stopwatch.status == DISABLED) {
					initialize_time_to_zero(&stopwatch.time);
				}
				break;

			
			default:
				break;
		}
	}
}

/* this task handles the graphics on the screen based on the occured events,
* it implements the main event driven heirarchial statemachine
*/
portTASK_FUNCTION(vUpdateGUI, pvParameters) {

	evt gui_evt;
	GUI_super_state_typedef super_state = SUPER_ACTIVE_ST;

	while(1) {
		xQueueReceive(GUIQueue, &gui_evt, portMAX_DELAY);

		switch(super_state) {

			case SUPER_ACTIVE_ST:
				super_active_state(gui_evt, &super_state);
				break;

			case SUPER_DORMANT_ST:
				dormant_default_state(gui_evt, &super_state);
				break;
		}
	}
}

void super_active_state (evt const event, GUI_super_state_typedef *const super_state) {
	evt rcv_gui_evt;
	time_struct display_time;

	static hour_mode_typedef hour_mode = HOUR_24;
	static GUI_state_typedef	state = INIT_ST;

	switch(state) {
		
		case INIT_ST:

			switch(event.sig) {
				
				case INIT_SIG:
					state = CLK_ST;
					rcv_gui_evt.sig = ST_ENTRY_SIG;
					xQueueSendToBack(GUIQueue, &rcv_gui_evt, portMAX_DELAY);
					break;

				default:
					break;
			}

			break;

		case CLK_ST:

			switch(event.sig) {
				
				case ST_ENTRY_SIG:
					BSP_print_mode(CLOCK);
					rcv_gui_evt.sig = CLOCK_TIME_RQ_SIG;
					xQueueSendToBack(timeQueue, &rcv_gui_evt, portMAX_DELAY);
					break;

				case UPDATE_TIME_SIG:
					display_time = check_hour_mode(event.time, hour_mode);
					BSP_print_time(display_time);
					break;

				case TOGGLE_HOUR_DIS_SIG:
					toggle_hour_mode(&hour_mode);
					rcv_gui_evt.sig = CLOCK_TIME_RQ_SIG;
					xQueueSendToBack(timeQueue, &rcv_gui_evt, portMAX_DELAY);
					break;

				case MODE_PB_SIG:
					state = STPWCH_ST;
					rcv_gui_evt.sig = ST_ENTRY_SIG;
					xQueueSendToBack(GUIQueue, &rcv_gui_evt, portMAX_DELAY);
					break;
				
				default:
					active_default_state(event, super_state);
					break;
			}

			break;

		case STPWCH_ST:

			switch(event.sig) {
				
				case ST_ENTRY_SIG:
					BSP_print_mode(STOPWATCH);
					rcv_gui_evt.sig = STPWCH_TIME_RQ_SIG;
					xQueueSendToBack(timeQueue, &rcv_gui_evt, portMAX_DELAY);
					break;

				case CNF_PB_SIG:
					rcv_gui_evt.sig = TGGLE_STPWCH_SIG;
					xQueueSendToBack(timeQueue, &rcv_gui_evt, portMAX_DELAY);
					break;

				case UPDATE_SUB_TIME_SIG:
					BSP_print_sub_time(event.time);
					break;
					
				case RST_PB_SIG:
					rcv_gui_evt.sig = RST_STPWCH_SIG;
					xQueueSendToBack(timeQueue, &rcv_gui_evt, portMAX_DELAY);
					rcv_gui_evt.sig = STPWCH_TIME_RQ_SIG;
					xQueueSendToBack(timeQueue, &rcv_gui_evt, portMAX_DELAY);
					break;
					
				case MODE_PB_SIG:
				 	state = CLK_ST;
					rcv_gui_evt.sig = ST_ENTRY_SIG;
					xQueueSendToBack(GUIQueue, &rcv_gui_evt, portMAX_DELAY);
					break;

				default:
					active_default_state(event, super_state);
					break;
			}

			break;

		default:
			break;

	}
}


void active_default_state(evt event, GUI_super_state_typedef *super_state) {

	switch(event.sig) {
		case TOGGLE_PWR_SIG:
			BSP_screen_off();
			*super_state = SUPER_DORMANT_ST;
			break;

		default:
			break;
	}
}

void dormant_default_state(evt event, GUI_super_state_typedef *super_state) {

	evt gui_evt;
	switch(event.sig) {
		case TOGGLE_PWR_SIG:
			*super_state = SUPER_ACTIVE_ST;
			gui_evt.sig = ST_ENTRY_SIG;
			xQueueSendToBack(GUIQueue, &gui_evt, portMAX_DELAY);
			break;

		default:
			break;
	}
}

/* In this task which will be get unblocked by giving a semaphore from a interrupt handler,
* the value of pushbuttons will be read and appropriate events will be issued based on the pushed button */
portTASK_FUNCTION_PROTO(vReadButtons, pvParameters) {
	static uint16_t	previous = 0;
	uint16_t current;
	uint16_t risedge;
	
	while(1) {
		xSemaphoreTake(xSemaphoreReadButton, portMAX_DELAY );
		
	  evt send_GUI_evt;
		current = GPIO_ReadInputData(GPIOE);
		
		/* Next line determines the rising edge of the pbs
		 * the bits that will be set to 1 in risedge register,
		 * indicate that a rising edge has happened for that
		 * pushbutton
		 */
		risedge = (current & previous) ^ current;
		
		// If Mode button is pressed
		if(risedge & GPIO_Pin_8) {
			send_GUI_evt.sig = MODE_PB_SIG;
			xQueueSendToBackFromISR(GUIQueue, &send_GUI_evt, &xHigherPriorityTaskWoken);
		}
		
		// If hour mode button is pressed
		if(risedge & GPIO_Pin_9) {
			send_GUI_evt.sig = TOGGLE_HOUR_DIS_SIG;
			xQueueSendToBackFromISR(GUIQueue, &send_GUI_evt, &xHigherPriorityTaskWoken);
		}
		
		// If confirm button is pressed
		if(risedge & GPIO_Pin_10) {
			send_GUI_evt.sig = CNF_PB_SIG;
			xQueueSendToBackFromISR(GUIQueue, &send_GUI_evt, &xHigherPriorityTaskWoken);
		}
		
		// If reset button is pressed
		if(risedge & GPIO_Pin_11) {
			send_GUI_evt.sig = RST_PB_SIG;
			xQueueSendToBackFromISR(GUIQueue, &send_GUI_evt, &xHigherPriorityTaskWoken);
		}
		
		// If sleep button is pressed
		if(risedge & GPIO_Pin_12) {
			send_GUI_evt.sig = TOGGLE_PWR_SIG;
			xQueueSendToBackFromISR(GUIQueue, &send_GUI_evt, &xHigherPriorityTaskWoken);
		}		
		
		previous = current;
	}
}





