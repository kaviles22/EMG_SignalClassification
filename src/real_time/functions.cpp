#include "functions.h"
#include <iostream>

#define SAMPLING_FREC 1000
#define WINDOW_SIZE   2000 //Window size in ms
#define WINDOW_RMS    50
#define WINDOW_FTS    10


Adafruit_MPU6050 mpu;

float   mw1_arr[40];
float   mw2_arr[40];
float   mw3_arr[40];

float   ax_arr[40];
float   ay_arr[40];
float   az_arr[40];

float   gx_arr[40];
float   gy_arr[40];
float   gz_arr[40];

struct  Sensor{
  float min_val;
  float max_val;
 };
float features[12];
sensors_event_t a, g, temp;

float get_rms(float sum_square, uint8_t num_values){
  return sqrt(sum_square/num_values);
  }

float reg_normalization(float norm_objective, float maximum, float minimum ){
    return (norm_objective - minimum)/(maximum - minimum);
  }

void get_min_max(float arr[], struct Sensor *s){
  float maxVal = arr[0];
  float minVal = arr[0];
  for (int i = 0; i < WINDOW_SIZE/WINDOW_RMS; i++) {
      maxVal = max(arr[i],maxVal);
      minVal = min(arr[i],minVal);
   }
   s->min_val = minVal;
   s->max_val = maxVal;
  }

float* prelim_collection(){
  
  float *final_features;
  int   mw1_sum_sq, mw2_sum_sq, mw3_sum_sq; 
  float ax_sum_sq, ay_sum_sq, az_sum_sq, gx_sum_sq, gy_sum_sq, gz_sum_sq;
  
  for (int arr_idx = 0; arr_idx < WINDOW_SIZE/WINDOW_RMS; arr_idx++){
   
    mw1_sum_sq = 0, mw2_sum_sq = 0, mw3_sum_sq = 0, ax_sum_sq = 0.0, ay_sum_sq = 0.0, az_sum_sq = 0.0, gx_sum_sq = 0.0, gy_sum_sq = 0.0, gz_sum_sq = 0.0;
    
    for(int i=0; i<WINDOW_RMS; i++){
      mpu.getEvent(&a, &g, &temp);
      
      mw1_sum_sq += pow(analogRead(pin_MW1),2);
      mw2_sum_sq += pow(analogRead(pin_MW2),2);
      mw3_sum_sq += pow(analogRead(pin_MW3),2);
  
      ax_sum_sq += pow(a.acceleration.x,2);
      ay_sum_sq += pow(a.acceleration.y,2);
      az_sum_sq += pow(a.acceleration.z,2);
  
      gx_sum_sq += pow(g.gyro.x,2);
      gy_sum_sq += pow(g.gyro.y,2);
      gz_sum_sq += pow(g.gyro.z,2);
      delay(1);
    }
    mw1_arr[arr_idx] = get_rms(mw1_sum_sq, WINDOW_RMS);
    mw2_arr[arr_idx] = get_rms(mw2_sum_sq, WINDOW_RMS);
    mw3_arr[arr_idx] = get_rms(mw3_sum_sq, WINDOW_RMS);

    ax_arr[arr_idx]  = get_rms(ax_sum_sq, WINDOW_RMS);
    ay_arr[arr_idx]  = get_rms(ay_sum_sq, WINDOW_RMS);
    az_arr[arr_idx]  = get_rms(az_sum_sq, WINDOW_RMS);

    gx_arr[arr_idx]  = get_rms(gx_sum_sq, WINDOW_RMS);
    gy_arr[arr_idx]  = get_rms(gy_sum_sq, WINDOW_RMS);
    gz_arr[arr_idx]  = get_rms(gz_sum_sq, WINDOW_RMS); 
  }

   return normalization_execution(mw1_arr, mw2_arr, mw3_arr, ax_arr, ay_arr, az_arr, gx_arr, gy_arr, gz_arr);
}

float* normalization_execution(float mw1[], float mw2[], float mw3[], float ax[], float ay[], float az[], float gx[], float gy[], float gz[]){
  
  Sensor mw1_st, mw2_st, mw3_st, ax_st, ay_st, az_st, gx_st, gy_st, gz_st;
  get_min_max(mw1, &mw1_st);
  get_min_max(mw2, &mw2_st);
  get_min_max(mw3, &mw3_st);

  get_min_max(ax, &ax_st);
  get_min_max(ay, &ay_st);
  get_min_max(az, &az_st);

  get_min_max(gx, &gx_st);
  get_min_max(gy, &gy_st);
  get_min_max(gz, &gz_st);
  
  for(int i = 0;i<WINDOW_SIZE/WINDOW_RMS;i++){
    mw1[i] = reg_normalization(mw1[i], mw1_st.max_val, mw1_st.min_val);
    mw2[i] = reg_normalization(mw2[i], mw2_st.max_val, mw2_st.min_val);
    mw3[i] = reg_normalization(mw3[i], mw3_st.max_val, mw3_st.min_val);

    ax[i] = reg_normalization(ax[i], ax_st.max_val, ax_st.min_val);
    ay[i] = reg_normalization(ay[i], ay_st.max_val, ay_st.min_val);
    az[i] = reg_normalization(az[i], az_st.max_val, az_st.min_val);

    gx[i] = reg_normalization(gx[i], gx_st.max_val, gx_st.min_val);
    gy[i] = reg_normalization(gy[i], gy_st.max_val, gy_st.min_val);
    gz[i] = reg_normalization(gz[i], gz_st.max_val, gz_st.min_val);
    }
  return get_features(mw1, mw2, mw3, ax, ay, az, gx, gy, gz);
  }

float* get_features(float mw1[], float mw2[], float mw3[], float ax[], float ay[], float az[], float gx[], float gy[], float gz[]){
  
  float mw1_sum_sq, mw2_sum_sq, mw3_sum_sq, ax_sum_sq, ay_sum_sq, az_sum_sq, gx_sum_sq, gy_sum_sq, gz_sum_sq;
  
  for(int x = 0; x < WINDOW_SIZE/(WINDOW_RMS*WINDOW_FTS); x++){
    mw1_sum_sq = 0.0, mw2_sum_sq = 0.0, mw3_sum_sq = 0.0, ax_sum_sq = 0.0, ay_sum_sq = 0.0, az_sum_sq = 0.0, gx_sum_sq = 0.0, gy_sum_sq = 0.0, gz_sum_sq = 0.0;

    for(int i = 0 ; i<WINDOW_FTS;i++){
      mw1_sum_sq += pow(mw1[i+x*WINDOW_FTS],2);
      mw2_sum_sq += pow(mw2[i+x*WINDOW_FTS],2);
      mw3_sum_sq += pow(mw3[i+x*WINDOW_FTS],2);

      ax_sum_sq += pow(ax[i+x*WINDOW_FTS],2);
      ay_sum_sq += pow(ay[i+x*WINDOW_FTS],2);
      az_sum_sq += pow(az[i+x*WINDOW_FTS],2);

      gx_sum_sq += pow(gx[i+x*WINDOW_FTS],2);
      gy_sum_sq += pow(gy[i+x*WINDOW_FTS],2);
      gz_sum_sq += pow(gz[i+x*WINDOW_FTS],2);

      }
     features[x]   = get_rms(mw1_sum_sq, WINDOW_FTS);
     features[x+4] = get_rms(mw2_sum_sq, WINDOW_FTS);
     features[x+8] = get_rms(mw3_sum_sq, WINDOW_FTS);
    }
    /*Serial.print("FTS : ");
    for (int i = 0; i < 12; i++){
      Serial.print(features[i]);
      Serial.print(",");
      }
    Serial.println("");*/
    return features;
 }
