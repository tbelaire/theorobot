#include "types.h"
#include "ring.h"
#include "assert.h"

#include "ring.c"

// connect motor controller pins to Arduino digital pins
// motor one
const int enA = 10;
const int in1 = 9;
const int in2 = 8;
// motor two
const int enB = 5;
const int in3 = 7;
const int in4 = 6;

const int sonar1_trig = 12;
const int sonar1_echo = 11;


#include <SoftwareSerial.h>
SoftwareSerial BTSerial(2, 3); // RX | TX
// Connect the HC-05 TX to Arduino pin 2 RX. 
// Connect the HC-05 RX to Arduino pin 3 TX through a voltage divider.
// 

#define QUEUE_SIZE 32

char inbuf[QUEUE_SIZE];
char outbuf[QUEUE_SIZE];

struct ringbuff in_ring;
struct ringbuff out_ring;

void setup()
{
    // set all the motor control pins to outputs
    pinMode(enA, OUTPUT);
    pinMode(enB, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);

    pinMode(sonar1_trig, OUTPUT);
    pinMode(sonar1_echo, INPUT);
    Serial.begin(9600);
    BTSerial.begin(9600);

    pinMode(LED_BUILTIN, OUTPUT);

    ringbuff_init(&in_ring, inbuf, QUEUE_SIZE);
    ringbuff_init(&out_ring, outbuf, QUEUE_SIZE);
}
void demoOne()
{
    // this function will run the motors in both directions at a fixed speed
    // turn on motor A
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    // set speed to 200 out of possible range 0~255
    analogWrite(enA, 200);
    // turn on motor B
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
    // set speed to 200 out of possible range 0~255
    analogWrite(enB, 200);
    delay(2000);
    // now change motor directions
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);  
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH); 
    delay(2000);
    // now turn off motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);  
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}
void demoTwo()
{
    // this function will run the motors across the range of possible speeds
    // note that maximum speed is determined by the motor itself and the operating voltage
    // the PWM values sent by analogWrite() are fractions of the maximum speed possible 
    // by your hardware
    // turn on motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);  
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH); 
    // accelerate from zero to maximum speed
    for (int i = 0; i < 256; i++)
    {
        analogWrite(enA, i);
        analogWrite(enB, i);
        delay(20);
    } 
    // decelerate from maximum speed to zero
    for (int i = 255; i >= 0; --i)
    {
        analogWrite(enA, i);
        analogWrite(enB, i);
        delay(20);
    } 
    // now turn off motors
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);  
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);  
}

u8 going = 0;
u8 stopped = 1;
long last_tick = 0;
i8 trim = -30;

void stop_motor() {
    stopped = 1;
    going = 0;
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
}

void go_straight() {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enA, 200 + trim);
    analogWrite(enB, 200);
}

void go_left() {
    digitalWrite(in2, LOW);
    digitalWrite(in1, HIGH);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
    analogWrite(enA, 200 + trim);
    analogWrite(enB, 200);

}
void loop()
{

    long duration, distance;
    int should_go = 0;
    digitalWrite(sonar1_trig, LOW);
    delayMicroseconds(2);
    digitalWrite(sonar1_trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(sonar1_trig, LOW);
    duration = pulseIn(sonar1_echo, HIGH);
    distance = (duration/2) / 29.1;

    if (distance >= 200 || distance <= 0){
        BTSerial.println("Out of range");
        Serial.println("Out of range");
    } else {
        if (distance < 18) { // This is where the LED On/Off happens
            digitalWrite(LED_BUILTIN,HIGH);
            should_go = 0;
        } else {
            digitalWrite(LED_BUILTIN,LOW);
            should_go = 1;
        }
        BTSerial.print(distance);
        BTSerial.println(" cm");
        Serial.print(distance);
        Serial.println(" cm");
    }
    int next_char = BTSerial.read();
    if( next_char == 'S' ) {
        stop_motor();
        BTSerial.println("Stopped");
    }
    if( next_char == 'G' ) {
        stopped = 0;
        going = 0;
        should_go = 1;
        BTSerial.println("Going");
    }
    if( next_char == '+' ) {
        if(trim < 50) {
            trim += 5;
        }
        BTSerial.print("trim");
        BTSerial.print(trim);
        BTSerial.println("");
    }
    if( next_char == '-' ) {
        if(trim > -50) {
            trim -= 5;
        }
        BTSerial.print("trim");
        BTSerial.print(trim);
        BTSerial.println("");
    }
    if( !stopped ) {
        if( should_go != going ) {
            if (should_go == 1) {
                go_straight();
                BTSerial.println("Going");
                going = 1;
            } else {
                go_left();
                BTSerial.println("Turning");
                going = 0;
            }
        }
    }
    delay(50);

}

