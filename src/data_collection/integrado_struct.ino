/*
Note: redefine spi communications pins in the SPI.cpp library file before the spiAttach functions as follows for ESP32C3 compatibility

_sck = 5;
_miso = 6;
_mosi = 4;
_ss = 1;

spiAttachSCK(_spi, _sck);
spiAttachMISO(_spi, _miso);
spiAttachMOSI(_spi, _mosi);

*/
//Include libraries for I2c communication with MPU6050 sensor
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

//Include libraries for SPI communication with SD card
#include "FS.h"
#include "SD.h"
#include "SPI.h"

//Include Ble libraries
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

//Define pins por I2C COMMUNICATION WITH MPU6050
#define I2C_SCL 8
#define I2C_SDA 9

//Define pins for ADC read (MyoWare1 - MyoWare2 - MyoWare3)
#define pin_MW1 0
#define pin_MW2 2
#define pin_MW3 3

//Define Ble variables
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define TIME             5    //seconds
#define FRECUENCY        1000 //Hz 
#define DATA_PER_SENSOR  TIME*FRECUENCY

Adafruit_MPU6050 mpu;
int              count;
unsigned long    Start;
//unsigned long    Start1;

uint8_t          task_number;
uint8_t          subj_number;
uint8_t          exp_number;
uint8_t          rep_number;
char             file_name_dat[25];

struct emg_data {
    uint16_t  mw1;
    uint16_t  mw2;
    uint16_t  mw3;
    float     ax;
    float     ay;
    float     az;
    float     gx;
    float     gy;
    float     gz;
};

emg_data        emg_array[DATA_PER_SENSOR];

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();
      //Serial.println(value.length());
      if (value.length() == 4) {
        task_number = (uint8_t)value[0];
        subj_number = (uint8_t)value[1];
        exp_number  = (uint8_t)value[2];
        rep_number  = (uint8_t)value[3];
        sprintf(file_name_dat, "/DAT/Task%d/S%d.%d_R%d.dat", task_number, subj_number, exp_number, rep_number);
        Serial.println(file_name_dat); 
        Start = millis();
        //Start1 = millis();
        while(count<DATA_PER_SENSOR){
          sensors_event_t a, g, temp;
          mpu.getEvent(&a, &g, &temp);

          emg_array[count].mw1  = analogRead(pin_MW1);
          emg_array[count].mw2  = analogRead(pin_MW2);
          emg_array[count].mw3  = analogRead(pin_MW3);

          emg_array[count].ax   = a.acceleration.x;
          emg_array[count].ay   = a.acceleration.y;
          emg_array[count].az   = a.acceleration.z;

          emg_array[count].gx   = g.gyro.x;
          emg_array[count].gy   = g.gyro.y;
          emg_array[count].gz   = g.gyro.z;
          count++;
          delay(1);
        }
        Serial.println(millis() - Start);           
        File file_dat = SD.open(file_name_dat, FILE_WRITE);
        if(!file_dat){
          Serial.println("Failed to open file for writing");
        }
        Serial.println(DATA_PER_SENSOR);
        for (int i = 0; i < DATA_PER_SENSOR; i++) {
          file_dat.write((const uint8_t *)&emg_array[i], sizeof(emg_array[i]));
        }
        file_dat.close();
        Serial.println("-- OVER --");
        //Serial.println(millis() - Start1);
      }
      pCharacteristic->setValue("File saved");
      count = 0;
    }
};

void setup_BLE(){
  BLEDevice::init("MyESP32");
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE 
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello World");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  }

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void setup_MPU6050(){
    while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(400000);

  if (!mpu.begin(0x68, &Wire, 0)) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  
}

void setup_ADC(uint8_t pin1, uint8_t pin2, uint8_t pin3){
  pinMode(pin1, INPUT);
  pinMode(pin2, INPUT);
  pinMode(pin3, INPUT);
  analogSetClockDiv(1);
}

void setup_SD(){
  if(!SD.begin(1)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }else{
    Serial.println("SD card found");
    }
  
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void setup(void) {
  Serial.begin(115200);

  //Setup for BLE
  setup_BLE();
  
  //Setup for MPU6050 (I2C)
 //setup_MPU6050();  

  //Setup for ADC sensors
  setup_ADC(pin_MW1, pin_MW2, pin_MW3);

  //Setup for SD card (SPI)
  setup_SD();
  
  count = 0;
  delay(100);
}

void loop() {

  delay(50000);
}
