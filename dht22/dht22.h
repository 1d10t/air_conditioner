/** prevent recursive inclusion --------------------------------------------- */

#ifndef DHT22_H
#define DHT22_H


/** defines ----------------------------------------------------------------- */

#define DHT_PORT			GPIOC
#define DHT_PIN				GPIO_PIN_3     // what digital pin we're connected to

#define DHT_PACKET_LENGTH	41

#define SIG_HUMID_SIZE		16
#define SIG_TEMP_SIZE		16
#define SIG_CRC_SIZE		8
#define SIG_HUMID_OFFSET	1
#define SIG_TEMP_OFFSET		17
#define SIG_CRC_OFFSET		33
 
#define DHT_INPUT_MODE()	DHT_PORT->DDR &= ~DHT_PIN
#define DHT_OUTPUT_MODE()	DHT_PORT->DDR |=  DHT_PIN
#define DHT_LOW()			DHT_PORT->ODR &= (uint8_t)(~DHT_PIN)
#define DHT_HIGH()			DHT_PORT->ODR |= (uint8_t)DHT_PIN
#define DHT_READ()			(DHT_PORT->IDR & DHT_PIN)

#ifndef PRINTF_EN
	#define PRINTF_EN		1
	#define PRINTF			if(PRINTF_EN) xprintf
#endif /* PRINTF_EN */


/** includes ---------------------------------------------------------------- */

#include <math.h>
#include "../stm8s_conf.h"
#include "../tim4_utils/tim4_utils.h"

#ifndef _STRFUNC
	#include "../xprintf/xprintf.h"
#endif /* _STRFUNC */

#include "dht22.c"


/** functions --------------------------------------------------------------- */

bool dht_sensor_read(int16_t *temp, int16_t *humid);

int16_t dht_compute_heat_index(int16_t temp, int16_t humid);


#endif /* DHT22_H */