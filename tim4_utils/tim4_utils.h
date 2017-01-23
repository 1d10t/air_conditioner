/** prevent recursive inclusion --------------------------------------------- */

#ifndef TIM4_UTILS_H
#define TIM4_UTILS_H


/** defines ----------------------------------------------------------------- */

#define MEAS_PRESCALER		TIM4_PRESCALER_32
#define MEAS_PRESCALER_US	(uint16_t) (1000000/(16000000 >> MEAS_PRESCALER))
#define MEAS_PRESCALER_MULT	(uint8_t) 24
#define MEAS_TIMER_INTERVAL	(uint16_t) (MEAS_PRESCALER_US * (uint16_t)MEAS_PRESCALER_MULT)
#define MEAS_US()			(uint16_t) ((uint16_t)delay_countdown * MEAS_TIMER_INTERVAL + TIM4->CNTR * MEAS_PRESCALER_US )


/** includes ---------------------------------------------------------------- */

#include "../stm8s_conf.h"
#include "tim4_utils.c"


/** global variables -------------------------------------------------------- */

extern volatile uint16_t delay_countdown;
extern volatile int8_t countdown_direction;


/** interrupt handlers ------------------------------------------------------ */

//extern INTERRUPT_HANDLER(IRQ_Handler_TIM4, 23);


/** functions --------------------------------------------------------------- */

void delay_ms(uint16_t ms);

void delay_us(uint16_t us);

void measure_us_stop();

void measure_us_start();

uint16_t measure_us();


#endif /* TIM4_UTILS_H */