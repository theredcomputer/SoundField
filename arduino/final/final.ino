#include <avr/io.h>
#include <avr/interrupt.h>

#define BAUD_RATE 115000
#define INCH_SCALAR 148.0
#define CM_SCALAR 58.0

#define NUM_SENSORS 8
#define FILT_SIZE 4
#define SENSOR_DELAY 70
#define PULSE_TIMEOUT 60000
#define LOW_THRESHOLD 1
#define HIGH_THRESHOLD 150

volatile uint16_t counter = 0;
volatile short printVals = 1;

typedef struct ca {
  uint8_t data[FILT_SIZE];
  uint8_t head;
  uint8_t total;
} circ_array;

circ_array ca[NUM_SENSORS];

uint8_t triggerSensor(uint8_t sensorNumber) {

  PORTC &= ~(0x30);
  PORTC |= (sensorNumber & 0x06) << 3;\

  PORTB &= ~(0x20);
  PORTB |= (sensorNumber & 0x01) << 5;
  
  digitalWrite(12, HIGH);
  delayMicroseconds(10);
  digitalWrite(12, LOW);
  
  return 0; 
}


void setup() {
  Serial.begin(BAUD_RATE);

  // All echo pins
  for (int i = 2; i < 12; i++)
  //for (int i = 2; i < 7; i++)
    pinMode(i, INPUT);

  // Two trigger pins, mux shared I/O pin, mux control pins
  for (int i = 12; i < 14; i++)
  //for (int i = 7; i < 12; i++)
    pinMode(i,  OUTPUT);
  DDRC |= 0x30;
  PORTC &= ~(0x30);
  
  // Initialize sensor value store
  for (uint8_t j = 0; j < NUM_SENSORS; j++) {
    for (uint8_t i = 0; i < FILT_SIZE; i++)
      ca[j].data[i] = 0;
    ca[j].head = 0;
    ca[j].total = 0;
  }
}

void loop() {

  uint8_t pulse = 0;
  
  for (int i = 0; i < NUM_SENSORS; i++) {

    // Get value from sensor
    //*
    triggerSensor(i);
    pulse = (int) (pulseIn(i + 2, HIGH, PULSE_TIMEOUT) / INCH_SCALAR);
    //*/
    /*
    digitalWrite(i + 7, HIGH);
    delayMicroseconds(10);
    digitalWrite(i + 7, LOW);
    pulse = (int) (pulseIn(i + 2, HIGH, PULSE_TIMEOUT) / INCH_SCALAR);
    //*/

    // If bad pulse value, add the average to maintain stability
    if (pulse < LOW_THRESHOLD || pulse > HIGH_THRESHOLD)
      pulse = ca[i].total / FILT_SIZE;

    // Add the new value to the averager
    ca[i].total += (pulse - ca[i].data[ca[i].head]);
    ca[i].data[ca[i].head] = pulse;
    ca[i].head = (ca[i].head + 1) % FILT_SIZE;
  
    delay(SENSOR_DELAY);
  }
  /*
    //triggerSensor(0);
    digitalWrite(7, HIGH);
    delayMicroseconds(1f0);
    digitalWrite(7, LOW);
    pulseDuration[0] = pulseIn(2, HIGH);
    delay(100);
  //*/
  for (int i = 0; i < NUM_SENSORS; i++) {
    Serial.print(ca[i].total / FILT_SIZE);
    Serial.print(" ");
  }
  Serial.println();
  //*/
}


