/** defines ----------------------------------------------------------------- */

#define DHT_PORT		GPIOC
#define DHT_PIN			GPIO_PIN_3
#define HEAT_PORT		GPIOD
#define HEAT_PIN		GPIO_PIN_4
#define COOL_PORT		GPIOD
#define COOL_PIN		GPIO_PIN_5
#define LED_PORT		GPIOB
#define LED_PIN			GPIO_PIN_5

#define PRINTF_EN		0
#define PRINTF			if(PRINTF_EN) xprintf

#define max_val			(min_val+max_step)
#define	cool_val		(min_val+cool_step)


/** includes ---------------------------------------------------------------- */

#include "stm8s_conf.h"
#include "xprintf/xprintf.h"
#include "tim4_utils/tim4_utils.h"
#include "wakeup/wakeup.h"
#include "dht22/dht22.h"
#include <stdio.h>


/** global variables -------------------------------------------------------- */

volatile int16_t min_val = 230;
volatile int16_t max_step = 10;
volatile int16_t cool_step = 50;
volatile uint16_t vent_humid = 700;

volatile bool use_hic = TRUE;


/** functions --------------------------------------------------------------- */

void led_setup(){
  GPIO_DeInit(LED_PORT);
  GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_HIGH_SLOW);
  GPIO_WriteHigh(LED_PORT, LED_PIN); // off
}
void led_on(){ GPIO_WriteLow(LED_PORT, LED_PIN); }
void led_off(){ GPIO_WriteHigh(LED_PORT, LED_PIN); }
void led_digit(uint8_t dig){
  led_off(); // off
  delay_ms(1500);
  for(uint8_t i=0; i<dig; i++){
    if(i>0){
      led_off(); // off
      delay_ms(500);
    }
    led_on();
    delay_ms(500);
  }
  led_off(); // off
}

void relay_setup(){
  GPIO_DeInit(HEAT_PORT);
  if(HEAT_PORT != COOL_PORT) GPIO_DeInit(COOL_PORT);
  
  GPIO_Init(HEAT_PORT, HEAT_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(COOL_PORT, COOL_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
}
void heat_on(){ GPIO_WriteLow(HEAT_PORT, HEAT_PIN); }
void heat_off(){ GPIO_WriteHigh(HEAT_PORT, HEAT_PIN); }
void cool_on(){ GPIO_WriteLow(COOL_PORT, COOL_PIN); }
void cool_off(){ GPIO_WriteHigh(COOL_PORT, COOL_PIN); }

void xputchar(char c){
  putchar(c);
}

int system_init(void){
  CLK_DeInit(); 
  CLK_HSICmd(ENABLE);
  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  
  // awu
  wakeup_setup(AWU_TIMEBASE_30S);
  
  // led
  led_setup();
  
  // relay
  relay_setup();
  
  // xprintf
  xdev_out(xputchar);
  
  // interrupts
  enableInterrupts();
  
  return 0;
}


int main( void ){
	
	system_init();
	heat_off();
	cool_off();
	
	
	uint8_t i = 0, first_iteration = 1;
	int16_t temp, humid;
	bool dht_status;
	
  
	while(1){
		
		dht_status = TRUE;
		
		do{
			
			temp = 0;
			humid = 0;
			
			if(dht_status == FALSE){
				delay_ms(100);
				dht_status = TRUE;
			}
			
			dht_status = dht_sensor_read(&temp, &humid);
			
		}while(dht_status == FALSE);

			
		PRINTF ( "temp = %d\n\0", temp );
		PRINTF ( "humid = %d\n\0", humid );


		if(use_hic == TRUE){
			temp = dht_compute_heat_index(temp, humid);
			PRINTF ( "hic = %d\n\0", temp );
		}
		
		
		if(temp < min_val){
			heat_on();
			if(humid > vent_humid){
				cool_on();
				PRINTF("too wet. vent, pls.\n\0");
			}else{
				cool_off();
			}
		}else if(temp >= max_val && temp < cool_val){
			heat_off();
			if(humid > vent_humid){
				cool_on();
				PRINTF("too wet. vent, pls.\n\0");
			}else{
				cool_off();
			}
			PRINTF("comfortable temp.\n\0");
		}else if (temp >= cool_val) {
			heat_off();
			cool_on();
			PRINTF("too hot. cool, pls\n\0");
		}
		
		if(first_iteration){
			led_digit((uint8_t)(temp / 100));
			led_digit((uint8_t)((temp % 100) / 10));
			first_iteration = 0;
			
			// if button pressed
				// go setup (led on 3s)
					// read current settings
					// if short press (<0.5s)
						// exit setup
					// if long press (2s)
						// wait input (fast blink 100ms on tim4)
			// ...	
		}
		
		// MCU in sleep mode (sim-salabim)
		halt();
	}// while
  
}
/**/
