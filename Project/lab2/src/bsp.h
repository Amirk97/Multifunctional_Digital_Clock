#ifndef BSP_H
#define BSP_H

#include "stm32f4xx.h"
#include <stm32f4_discovery.h>
#include "utils.h"
#include "lcd.h"
#include "functions.h"
#include "intercom.h"


#define APB1_CLOCK_HZ 42000000
#define APB2_CLOCK_HZ 84000000
#define TIM3_CLOCK_HZ APB2_CLOCK_HZ
#define TIM4_CLOCK_HZ APB2_CLOCK_HZ
#define TIM5_CLOCK_HZ APB2_CLOCK_HZ

void BSP_init (void);
void BSP_LedToggle(void);
uint8_t BSP_ReadInput(void);

void GPIOD_Config (void);
void GPIOE_Config (void);

void Timer3_Config(void);
void Timer4_Config(void);
void Timer5_Config(void);

void BSP_Timer5_cmd(FunctionalState const);

void BSP_print_mode(page_mode_typedef const);
void BSP_print_time(time_struct const);

void BSP_clear_time_row(void);
void BSP_clear_mode_row(void);
	
void BSP_print_sub_time(time_struct const);
void BSP_screen_off(void);


#endif
