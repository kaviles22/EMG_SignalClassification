#include "servo.h"

ISR_servo_t servo_mov  = {-1, PIN_SERVO_MOV};
ISR_servo_t servo_turn = {-1, PIN_SERVO_TURN};

void setup_servo(){
  
  ESP32_ISR_Servos.useTimer(USE_ESP32_TIMER_NO);
  servo_mov.servoIndex  = ESP32_ISR_Servos.setupServo(servo_mov.servoPin, MIN_MICROS, MAX_MICROS);
  servo_turn.servoIndex = ESP32_ISR_Servos.setupServo(servo_turn.servoPin, MIN_MICROS, MAX_MICROS);

}

void set_servo_mov(uint8_t servo_pos){
  ESP32_ISR_Servos.setPosition(servo_mov.servoIndex, servo_pos);  
}

void set_servo_turn(uint8_t servo_pos){
  ESP32_ISR_Servos.setPosition(servo_turn.servoIndex, servo_pos);  
}

int get_servo_mov(){
  return ESP32_ISR_Servos.getPosition(servo_mov.servoIndex);
  }

int get_servo_turn(){
  return ESP32_ISR_Servos.getPosition(servo_turn.servoIndex);
  }
