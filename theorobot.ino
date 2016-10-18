#include "types.h"
#include "ring.h"
#include "assert.h"

#include "ring.c"

// connect motor controller pins to Arduino digital pins
// motor one
const int enA = 6;
const int in1 = 9;
const int in2 = 8;
// motor two
const int enB = 5;
const int in3 = 7;
const int in4 = 10;

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

u8 going = 0;
u8 stopped = 1;
long last_tick = 0;
i8 trim = 0;
u8 last_cm = 0;

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
        if(distance != last_cm) {
        BTSerial.print(distance);
        BTSerial.println(" cm");
        Serial.print(distance);
        Serial.println(" cm");
        last_cm = distance;
        }
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

