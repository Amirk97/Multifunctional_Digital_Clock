#include "functions.h"

void update_time_reg(time_struct *time) {
	
	if (time->second == 59)
	{
		time->second = 0;
		
		if (time->minute == 59)
		{
			time->minute = 0;
			
			if (time->hour == 23)
			{
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