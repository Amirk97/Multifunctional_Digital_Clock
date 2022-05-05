#include "functions.h"
#include "bsp.h"

/* Updating the variables that hold the time for clock functions
 * All update calculations will be applied to 24 hour mode
 */
void update_time_reg(time_struct *time) {
	
	if(time->second == 59) {
		time->second = 0;
		
		if(time->minute == 59) {
			time->minute = 0;
			
			if(time->hour == 23) {
				time->hour = 0;
				
			}else {
				time->hour ++;
			}
			
		}else {
			time->minute ++;
		}
		
	}else {
		time->second ++;
	}
}

/* Updating the variables that hold the time for stopwatch functions
 */
void update_sub_time_reg(time_struct *time) {
	if(time->mili_sec == 99) {
		
		time->mili_sec = 0;
		if(time->second == 59) {
			
			time->second = 0;
			if(time->minute == 59) {
				
				time->minute = 0;
				time->hour ++;

			}else {
				time->minute ++;
			}

		}else {
			time->second ++;
		}
	}else {
		time->mili_sec +=1;
	}
}

void toggle_hour_mode(hour_mode_typedef* hour_mode) {
	
	if(*hour_mode == HOUR_12) {
		*hour_mode = HOUR_24;
	}else {
		*hour_mode = HOUR_12;
	}
}
/* This function converts the time from 12 to 24 (or vice versa)
 * and prepares the time for printing according to that */
time_struct check_hour_mode( time_struct time, int hour_mode) {
	
	if(hour_mode == HOUR_24) {
		time.time_period = NONE;
		return time;
		
	}else {
		
		if(time.hour < 12) {
			time.time_period = AM;
			if(time.hour == 0) {
				time.hour = 12;
			}	
		}else {
			time.time_period = PM;
			if(time.hour != 12) {
				time.hour -= 12;
			}	
		}
		return time;
	}
}

/* This function toggles the stopwatch status
 * and disable or enables the stopwatch timer based on that */
void toggle_stpwtch_status(mode *stpwch) {
	if(stpwch->status == DISABLED ) {
		stpwch->status = ENABLED;
		BSP_Timer5_cmd(ENABLE);
	} else if (stpwch->status == ENABLED ) {
		stpwch->status = DISABLED;
		BSP_Timer5_cmd(DISABLE);
	}
}

void initialize_time_to_zero(time_struct *time) {
	time->mili_sec = 0;
	time->second = 0;
	time->minute = 0;
	time->hour = 0;
}

/* This function toggles the power mode of the device and
 * clears the screen when its put in DISABLED status */
void toggle_power_mode(status_typedef *screen_status) {
	if(*screen_status == DISABLED ) {
		*screen_status = ENABLED;
	} else if (*screen_status == ENABLED ) {
		*screen_status = DISABLED;
		BSP_screen_off();
	}
}

