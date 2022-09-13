#ifndef SERVO_HANDLER_H_
#define SERVO_HANDLER_H_ //header guard to prevent adding it multiple times

#define TIMER_INTERRUPT_DEBUG       1
#define ISR_SERVO_DEBUG             1
#define USE_ESP32_TIMER_NO          1

#define PIN_SERVO_MOV       5
#define PIN_SERVO_TURN      6

/*#define NUM_SERVOS          2 
Used to create an array of unitialized servos represented by the struct defined further below
*/

#define MIN_MICROS      544
#define MAX_MICROS      2400

#include "ESP32_C3_ISR_Servo.h"

typedef struct
{
  int     servoIndex;
  uint8_t servoPin;
} ISR_servo_t;

void setup_servo();
void set_servo_mov(uint8_t servo_pos);
void set_servo_turn(uint8_t servo_pos);
int get_servo_mov();
int get_servo_turn();
#endif 
