#ifndef __FUNCTIONS_H
#define __FUNCTIONS_H

#include <stdint.h>


typedef enum{
	DISABLED,
	ENABLED,
	END
}status_typedef;

typedef enum{
	HOUR_12,
	HOUR_24
}hour_mode_typedef;

typedef enum{
	NONE,
	AM,
	PM
}time_period_typedef;

typedef enum{
	SECOND,
	MINUTE,
	HOUR
}time_placeholder_typedef;

typedef enum{
	ON,
	OFF,
}visibility_typedef;

typedef struct{
	uint8_t mili_sec;
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	time_period_typedef time_period;
}time_struct;

typedef struct{
	time_struct time;
	status_typedef status;
}mode;

typedef enum{
	INIT_ST,
	CLK_ST,
	STPWCH_ST,
}GUI_state_typedef;

typedef enum{
	SUPER_ACTIVE_ST,
	SUPER_DORMANT_ST,
}GUI_super_state_typedef;


typedef enum{
	CLOCK,
	STOPWATCH,
}page_mode_typedef;



time_struct check_hour_mode( time_struct const, int const);
void update_time_reg(time_struct *const);
void update_sub_time_reg(time_struct *const);
void toggle_hour_mode(hour_mode_typedef* hour_mode);
void toggle_power_mode(status_typedef *const);
void toggle_stpwtch_status(mode *const);
void initialize_time_to_zero(time_struct *const);


	
#endif
