// dry temperature NTC probe
#define TD_THERMISTORPIN A0    

// wet temperature NTC probe
#define TW_THERMISTORPIN A1    
     
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000
      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
   
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5

// The beta coefficient of the thermistor (usually 3000-4000)
#define BCOEFFICIENT 3950

// the value of the 'other' resistor
#define SERIESRESISTOR 10000    

// RH N coefficient
#define RH_N 0.6687451584    
 
uint16_t td_samples[NUMSAMPLES];
uint16_t tw_samples[NUMSAMPLES];
 
void setup(void) {
  Serial.begin(9600);
  //analogReference(EXTERNAL);
}
 
void loop(void) {
  uint8_t i;
  float td_average;
  float tw_average;
 
  // take N samples in a row, with a slight delay
  for (i=0; i< NUMSAMPLES; i++) {
   td_samples[i] = analogRead(TD_THERMISTORPIN);
   delay(10);
   tw_samples[i] = analogRead(TW_THERMISTORPIN);
   delay(10);
  }
 
  // average all the samples out
  td_average = 0;
  tw_average = 0;
  for (i=0; i< NUMSAMPLES; i++) {
     td_average += td_samples[i];
     tw_average += tw_samples[i];
  }
  td_average /= NUMSAMPLES;
  tw_average /= NUMSAMPLES;
 
  Serial.print("Dry temp Average : "); 
  Serial.print(td_average);
  Serial.print("   Wet temp Average : "); 
  Serial.println(tw_average);
 
  // convert the value to resistance
  td_average = 1023 / td_average - 1;
  td_average = SERIESRESISTOR / td_average;
  Serial.print("Dry temp Thermistor resistance "); 
  Serial.print(td_average);
 
  float td_steinhart;
  td_steinhart = td_average / THERMISTORNOMINAL;     // (R/Ro)
  td_steinhart = log(td_steinhart);                  // ln(R/Ro)
  td_steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  td_steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  td_steinhart = 1.0 / td_steinhart;                 // Invert
  td_steinhart -= 273.15;                         // convert to C
 
  Serial.print("Dry Temperature :  "); 
  Serial.print(td_steinhart);
  Serial.println(" *C");

  // convert the value to resistance
  tw_average = 1023 / tw_average - 1;
  tw_average = SERIESRESISTOR / tw_average;
  Serial.print("Wet Thermistor resistance "); 
  Serial.print(tw_average);

  float tw_steinhart;
  tw_steinhart = tw_average / THERMISTORNOMINAL;     // (R/Ro)
  tw_steinhart = log(tw_steinhart);                  // ln(R/Ro)
  tw_steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  tw_steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  tw_steinhart = 1.0 / tw_steinhart;                 // Invert
  tw_steinhart -= 273.15;                         // convert to C
 
  Serial.print("  Wet Temperature :  "); 
  Serial.print(tw_steinhart);
  Serial.println(" *C");

  Serial.print("Relative Humidity :  "); 
  Serial.print(rh_cal_func(td_steinhart,tw_steinhart)*100);
  Serial.println(" % "); 
 
  delay(1000);
}

float rh_cal_func(float td_temp, float tw_temp){
  float ed, ew, rh; 
  ed = 6.112 * exp((17.502*td_temp)/(240.97+td_temp));
  ew = 6.112 * exp((17.502*tw_temp)/(240.97+tw_temp));
  rh = (ew - RH_N*(1+0.00115*td_temp)*(td_temp-tw_temp))/ed;
  return rh;
}

