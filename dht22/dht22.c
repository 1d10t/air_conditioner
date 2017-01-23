
/** functions --------------------------------------------------------------- */

bool dht_sensor_read(int16_t *temp, int16_t *humid){
	
	// vars
	
	uint16_t d[DHT_PACKET_LENGTH], ts;
	uint8_t crc_calc, bit, bit_pos, crc, t;
	
	(*temp) = 0;
	(*humid) = 0;
	crc = 0;
	crc_calc = 0;
	
	
	// init
	
	DHT_OUTPUT_MODE();
	DHT_HIGH();
	delay_ms(100);
	DHT_LOW();
	delay_ms(5);
	measure_us_start();
	DHT_HIGH();
	//delay_us(20);
	while((uint16_t)MEAS_US() <= (uint16_t)30);
	DHT_LOW();
	DHT_INPUT_MODE();
	
	
	// begin reading
	
	for(t = 0; t < DHT_PACKET_LENGTH; t++){
		while (!DHT_READ() && countdown_direction != 0); // wait 0
		ts = MEAS_US();//measure_us();//
		while (DHT_READ() && countdown_direction != 0); // wait 1
		d[t] = MEAS_US() - ts;//measure_us() - ts;//
		//delay_countdown = 0;
	}
	measure_us_stop();
	
	
	// normalize and calc crc
	
	for(t = DHT_PACKET_LENGTH-1; t > 0; t--){
	//for(t = 1; t < DHT_PACKET_LENGTH; t++){
		if(d[t] >= 20 && d[t] <= 30){
			bit = 0;
		}
		else if(d[t] >= 60 && d[t] <= 80){
			bit = 1;
		}else{
			PRINTF ( "bad sig\n\0" );
			return FALSE;
		}
		if(t >= SIG_TEMP_OFFSET && t < (SIG_TEMP_OFFSET+SIG_TEMP_SIZE)){
			
			//PRINTF("%u\0", bit);
		  
			bit_pos = t-SIG_TEMP_OFFSET;
			if(bit && bit_pos == 0){
				PRINTF ( "temp minus %d\n\0", (*temp) );
				(*temp) = -(*temp);
			}else{
				(*temp) += (int16_t) bit << (SIG_TEMP_SIZE-1-bit_pos);
			}
			crc_calc += bit << (SIG_CRC_SIZE-1-bit_pos%SIG_CRC_SIZE);
		}else if(t >= SIG_HUMID_OFFSET && t < (SIG_HUMID_OFFSET+SIG_HUMID_SIZE)){
			bit_pos = t-SIG_HUMID_OFFSET;
			(*humid) += (int16_t) bit << (SIG_HUMID_SIZE-1-bit_pos);
			crc_calc += bit << (SIG_CRC_SIZE-1-bit_pos%SIG_CRC_SIZE);
		}else if(t >= SIG_CRC_OFFSET && t < (SIG_CRC_OFFSET+SIG_CRC_SIZE)){
			bit_pos = t-SIG_HUMID_OFFSET;
			crc += bit << (SIG_CRC_SIZE-1-bit_pos%SIG_CRC_SIZE);
		}
	}
	
	if(crc != crc_calc){
		PRINTF ( "bad sig %u (crc) != %u (crc_calc)\n\0", crc, crc_calc );
		return FALSE;
	}
	
	return TRUE;
}

int16_t dht_compute_heat_index(int16_t temp, int16_t humid) {
  // Using both Rothfusz and Steadman's equations
  // http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
  double hi, temperature, percentHumidity;
  temperature = ((double) temp) / 10.0;
  percentHumidity = ((double) humid) / 10.0;
  
  // F to C
  temperature = (temperature * 1.8) + 32.0;

  hi = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) + (percentHumidity * 0.094));

  if (hi > 79.0) {
    hi = -42.379;
	//	PRINTF("hi1 = %d\r\n\0", (int16_t)(hi*10));
    hi += 2.04901523 * temperature;
	//	PRINTF("hi2 = %d\r\n\0", (int16_t)(hi*10));
    hi += 10.14333127 * percentHumidity;
	//	PRINTF("hi3 = %d\r\n\0", (int16_t)(hi*10));
    hi += -0.22475541 * temperature*percentHumidity;
	//	PRINTF("hi4 = %d\r\n\0", (int16_t)(hi*10));
    hi += (-0.00683783 * pow(temperature, 2.0)); //!!!
	//	PRINTF("hi5 = %d\r\n\0", (int16_t)(hi*10));
    hi += -0.05481717 * pow(percentHumidity, 2.0);
	//	PRINTF("hi6 = %d\r\n\0", (int16_t)(hi*10));
    hi += 0.00122874 * pow(temperature, 2) * percentHumidity;
	//	PRINTF("hi7 = %d\r\n\0", (int16_t)(hi*10));
    hi += 0.00085282 * temperature*pow(percentHumidity, 2.0);
	//	PRINTF("hi8 = %d\r\n\0", (int16_t)(hi*10));
    hi += -0.00000199 * pow(temperature, 2.0) * pow(percentHumidity, 2.0);
	//	PRINTF("hi9 = %d\r\n\0", (int16_t)(hi*10));
	
	if((percentHumidity < 13.0) && (temperature >= 80.0) && (temperature <= 112.0)){
      hi -= ((13.0 - percentHumidity) * 0.25) * sqrt((17.0 - ((double)abs(temperature - 95.0))) * 0.05882);
	//	PRINTF("hi10.1 = %d\r\n\0", (int16_t)(hi*10));
	}
	else if((percentHumidity > 85.0) && (temperature >= 80.0) && (temperature <= 87.0)){
      hi += ((percentHumidity - 85.0) * 0.1) * ((87.0 - temperature) * 0.2);;
	//	PRINTF("hi10.2 = %d\r\n\0", (int16_t)(hi*10));
	}
  }

  return (int16_t)((hi - 32.0) * 5.5555);
}

