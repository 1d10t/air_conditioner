
/** global variables -------------------------------------------------------- */

volatile uint16_t delay_countdown;
volatile int8_t countdown_direction;


/** interrupt handlers ------------------------------------------------------ */

INTERRUPT_HANDLER(IRQ_Handler_TIM4, 23)
{
	if(countdown_direction > 0){
		if(delay_countdown != 16383){
			delay_countdown++;
		}else{
			countdown_direction = 0;
		}
	}else if(countdown_direction < 0){
		if(delay_countdown){
			delay_countdown--;
		}else{
			countdown_direction = 0;
		}
	}
	//TIM4_ClearITPendingBit(TIM4_IT_UPDATE);
	TIM4->SR1 = (uint8_t)(~TIM4_IT_UPDATE); // faster?
}


/** functions --------------------------------------------------------------- */

void delay_ms(uint16_t ms){
	TIM4_DeInit();
    TIM4_TimeBaseInit(TIM4_PRESCALER_128, 124);
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

    delay_countdown = ms;
	countdown_direction = -1;

    TIM4_Cmd(ENABLE);       // let's go

    while(delay_countdown); // wait
}

void delay_us(uint16_t us){
	TIM4_DeInit();
    TIM4_TimeBaseInit(TIM4_PRESCALER_128, 1);//TIM4_TimeBaseInit(TIM4_PRESCALER_2, 7);//
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

    delay_countdown = us/10;
	countdown_direction = -1;

    TIM4_Cmd(ENABLE);       // let's go

    while(delay_countdown); // wait
}

void measure_us_stop(){
    TIM4_DeInit();       // stop
}

void measure_us_start(){
	TIM4_DeInit();
    TIM4_TimeBaseInit(MEAS_PRESCALER, MEAS_PRESCALER_MULT); // prescaler 64 = 4us, prescaler 128 = 8us
    TIM4_ClearFlag(TIM4_FLAG_UPDATE);
    TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE);

    delay_countdown = 0;
	countdown_direction = 1;

    TIM4_Cmd(ENABLE);       // let's go
}

uint16_t measure_us(){
	return delay_countdown * MEAS_PRESCALER_MULT;
}

