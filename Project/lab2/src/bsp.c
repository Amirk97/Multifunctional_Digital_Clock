#include "bsp.h"
#include "intercom.h"
#include <stdio.h>

void BSP_init (void) {
	// Initialize the LEDs on the board
	GPIOD_Config();
	
	// Initialize the push buttons on the board
	GPIOE_Config();
	
	lcd_init();

	// Initialize the timer which counts the time and issues an interrupt every 1 sec
	Timer3_Config();
	
	// Initialize the timer which is used for polling and debouncing the push button
	// issues an interrupt every 0.1 sec
	Timer4_Config();
	
	// Initialize a timer which is used for counting a hundredth of second 
	Timer5_Config();
	
}

void GPIOD_Config (void) {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = 0xFF00;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}

void GPIOE_Config (void) {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = 0xFF00;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

// The timer which counts the time and issues an interrupt every 1 sec
void Timer3_Config (void) {
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIM3, ENABLE );
	
  /* The purpose of next line is to convert the clock frequency
	*  for this peripheral to to 10kHz
	*/
	uint16_t PrescalerValue = (TIM3_CLOCK_HZ/8400) - 1;

	/* Time base configuration */
	// The value of set period 
	TIM_TimeBaseStructure.TIM_Period = 10000; /* The number of clock periods required for firing interrupts */
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* Enable the TIM3 global Interrupt */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	/* Higher value for priority means lower priority */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
  NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3, ENABLE);
}

// the timer which is used for polling and debouncing the push button
// issues an interrupt every 0.1 sec
void Timer4_Config() {
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* TIM4 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	/* The purpose of next line is to convert the clock frequency
	*  for this peripheral to to 10kHz
	*/
	uint16_t PrescalerValue = (TIM4_CLOCK_HZ/8400) - 1;

	/* Time base configuration */	
	TIM_TimeBaseStructure.TIM_Period = 1000; /* The number of clock periods required for firing interrupts */
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
		/* Enable the TIM4 global Interrupt */
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	/* Higher value for priority means lower priority */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
  NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM4, ENABLE);
}

// a timer which is used for counting a hundredth of second 
void Timer5_Config(void) {
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	
	
	/* TIM5 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	
	/* The purpose of next line is to convert the clock frequency
	*  for this peripheral to to 10kHz
	*/
	uint16_t PrescalerValue = (TIM5_CLOCK_HZ/8400) - 1;

	/* Time base configuration */
	
	TIM_TimeBaseStructure.TIM_Period = 100; /* The number of clock periods required for firing interrupts */
	TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
}

/* This function enables or disables the sub second timer
 * based on the passed argument to it
*/
void BSP_Timer5_cmd(FunctionalState state) {
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable the TIM5 global Interrupt */
	TIM_ITConfig(TIM5, TIM_IT_Update, state);
	
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	/* Higher value for priority means lower priority */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = state;
	
  NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM5, state);
}

/* The handler for hundredth of second timer */
void TIM5_IRQHandler() {
	evt	time_evt;
  uint8_t TIM_flag = TIM_GetFlagStatus(TIM5, TIM_FLAG_Update);

	if(TIM_flag == SET) {
		time_evt.sig = SUB_SEC_SIG;
		xQueueSendToBackFromISR(timeQueue, &time_evt, &xHigherPriorityTaskWoken);
		TIM_ClearITPendingBit( TIM5, TIM_FLAG_Update);
	}		
}

/* The handler for a tenth of second timer, used for polling on push buttons */
void TIM4_IRQHandler() {
	
	uint8_t TIM_flag = TIM_GetFlagStatus(TIM4, TIM_FLAG_Update);
	
	if(TIM_flag == SET) {
		/* Unblock the waiting task by giving the semaphore. */
		xSemaphoreGiveFromISR(xSemaphoreReadButton, &xHigherPriorityTaskWoken);
		TIM_ClearITPendingBit(TIM4, TIM_FLAG_Update);
	}
}

/* The handler for one second timer, used for clock */
void TIM3_IRQHandler() {
	
	evt	time_evt;
  uint8_t TIM_flag = TIM_GetFlagStatus(TIM3, TIM_FLAG_Update);

	if(TIM_flag == SET) {
		time_evt.sig = SEC_SIG;
		xQueueSendToBackFromISR(timeQueue, &time_evt, &xHigherPriorityTaskWoken);
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
	}	
}

void BSP_LedToggle (void) {
	GPIO_ToggleBits(GPIOD, 0xE00);
}

uint8_t BSP_ReadInput (void) {
	return GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15);
}

void BSP_print_mode (page_mode_typedef mode) {
	
	BSP_clear_mode_row();
	lcd_move(0,1);
	switch(mode) {
		case CLOCK:
			lcd_print("Mode: clock");
			break;
		
		case STOPWATCH:
			lcd_print("Mode: stopwatch");
			break;
		
		default:
			break;
	}
}

void BSP_clear_time_row() {
	char str[17] = "                ";
	lcd_move(0,0);
	lcd_print(str);
}

void BSP_clear_mode_row() {
	char str[17] = "                ";
	lcd_move(0,1);
	lcd_print(str);
}

/* This function prints Hour, Minute and Second digits
* on the screen, For preventing from memory leak problems
*	snprintf sunction has been used instead of printf function
*/
void BSP_print_time(time_struct time) {
	char str[3];
	
	BSP_clear_time_row();
	
	lcd_move(2,0);
	lcd_print(":");
	
	lcd_move(5,0);
	lcd_print(":");
	
	snprintf(str, 3, "%d", time.hour);
	lcd_move(0,0);
	lcd_print(str);
	
	snprintf(str, 3, "%d", time.minute);
	lcd_move(3,0);
	lcd_print(str);
	
	snprintf(str, 3, "%d", time.second);
	lcd_move(6,0);
	lcd_print(str);
	
	lcd_move(10,0);
	switch(time.time_period) {
		
		case AM:
			lcd_print("AM");
			break;
		
		case PM:
			lcd_print("PM");
			break;
				
		case NONE:
			break;
						
		default:			
			break;						
	}
		
}


/* This function prints Hour, Minute and Second and hundredth of second digits
* on the screen, For preventing from memory leak problems
*	snprintf sunction has been used instead of printf function
*/
void BSP_print_sub_time(time_struct time) {
	char str[3];
	
	BSP_clear_time_row();
	
	lcd_move(2,0);
	lcd_print(":");
	
	lcd_move(5,0);
	lcd_print(":");
	
	lcd_move(8,0);
	lcd_print(":");
	
	snprintf(str, 3, "%d", time.hour);
	lcd_move(0,0);
	lcd_print(str);
	
	snprintf(str, 3, "%d", time.minute);
	lcd_move(3,0);
	lcd_print(str);
	
	snprintf(str, 3, "%d", time.second);
	lcd_move(6,0);
	lcd_print(str);
	
	snprintf(str, 3, "%d", time.mili_sec);
	lcd_move(9,0);
	lcd_print(str);
}

/* This function clears the whole lcd */
void BSP_screen_off() {
	BSP_clear_mode_row();
	BSP_clear_time_row();
}

