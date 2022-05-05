#ifndef INTERCOM_H
#define INTERCOM_H

//#include "functions.h"
#include "FreeRTOS.h"

#include "semphr.h"
#include "task.h"
#include "queue.h"
#include "functions.h"


typedef struct{
	uint8_t sig;
	time_struct time;
}evt;

typedef enum{
	SEC_SIG,						/* One second passed event */
	SUB_SEC_SIG,				/* A hundreth of second passed event */
	CLOCK_TIME_RQ_SIG,	/* A request for clock time registers event */
	STPWCH_TIME_RQ_SIG, /* A request for stopwatch time registers event */
	TGGLE_STPWCH_SIG,		/* An event commanding a change in stopwatch status */
	RST_STPWCH_SIG			/* An event for clearing the stopwatch time registers */
}time_sig_typedef;

typedef enum{
	INIT_SIG,			/* The initialization event */
	ST_ENTRY_SIG, /* The state entry event */
	UPDATE_TIME_SIG, /* UPDATE TIME ON SCREEN EVENT */
	UPDATE_SUB_TIME_SIG, 	/* UPDATE TIME ON SCREEN EVENT */
	TOGGLE_HOUR_DIS_SIG,  /* Toggle time convention event */
	MODE_PB_SIG,	/* The mode pushbutton press event */
	UPDATE_STPWCH_SIG, /* UPDATE STOPWATCH ON SCREEN EVENT */
	CNF_PB_SIG,		/* The confirm pushbutton press event */
	RST_PB_SIG,		/* The reset pushbutton press event */
	FLCKR_TIMOUT_SIG, /* This event indicates that flicker timer has timed out */
	UP_PB_SIG,			/* The up pushbutton press event */
	DWN_PB_SIG,			/* The down pushbutton press event */
	TOGGLE_PWR_SIG 	/* Toggle the power mode event */
}gui_sig_typedef;


void INTERCOM_init(void);

/* The only global variables which to be shared because their purpose 
 * is to establish communication and synchronization between different
 * source files */
extern xQueueHandle GUIQueue;
extern xQueueHandle timeQueue;
extern BaseType_t xHigherPriorityTaskWoken;
extern SemaphoreHandle_t xSemaphoreReadButton;

#endif
