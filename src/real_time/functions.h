#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_ //header guard to prevent adding it multiple times



//MPU communication
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include <stdint.h>
//C++ mathematical functions 
#include <cmath>


//Define pins por I2C COMMUNICATION WITH MPU6050
#define I2C_SCL 8
#define I2C_SDA 9

//Define pins for ADC read (MyoWare1 - MyoWare2 - MyoWare3)
#define pin_MW1 0
#define pin_MW2 2
#define pin_MW3 3

#define TIME             5    //seconds
#define FREQUENCY        1000 //Hz 
#define DATA_PER_SENSOR  TIME*FRECUENCY

extern float   mw1_arr[40];
extern float   mw2_arr[40];
extern float   mw3_arr[40];

extern float   ax_arr[40];
extern float   ay_arr[40];
extern float   az_arr[40];

extern float   gx_arr[40];
extern float   gy_arr[40];
extern float   gz_arr[40];

extern sensors_event_t a, g, temp;

float *prelim_collection();
float *normalization_execution(float mw1[], float mw2[], float mw3[], float ax[], float ay[], float az[], float gx[], float gy[], float gz[]);
float *get_features(float mw1[], float mw2[], float mw3[], float ax[], float ay[], float az[], float gx[], float gy[], float gz[]);

#endif  // FUNCTIONS_H_
