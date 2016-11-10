#include <avr/io.h>
#include <avr/interrupt.h> 

#define BAUD_RATE 230400
#define INCH_SCALAR 148.0
#define CM_SCALAR 58.0

// First row is trigger, second row is echo
const uint8_t PINS_LEN = 5;
const uint8_t PINS[2][PINS_LEN] = {
  { 2,4,6,8,10 },
  { 3,5,7,9,11 }
};
const uint8_t DELAY = 50;
volatile uint16_t counter = 0;
volatile short printVals = 1;

signed long pulseDuration[PINS_LEN];
signed long time[PINS_LEN];


ISR(TIMER2_OVF_vect) {
  counter++;
}


void setup() {
  Serial.begin(BAUD_RATE);
  for (int i = 0; i < PINS_LEN; i++) {
    pinMode(PINS[0][i], OUTPUT);  
    pinMode(PINS[1][i], INPUT);
  }

  // Configure interrupts
  TIMSK2 |= 1<<TOIE2;
  TCNT2 = 0x00;
  TCCR2B |= (1 << CS20) | (1 << CS22);
  sei();

  //PORTC &= ~(0x30);
  //DDRC |= 0x30;

}

void loop() {

  for (int i = 0; i < PINS_LEN; i++) {
    digitalWrite(PINS[0][i], HIGH);
    delayMicroseconds(10);
    digitalWrite(PINS[0][i], LOW);
    pulseDuration[i] = pulseIn(PINS[1][i], HIGH);
    delay(5);
  }

  /*digitalWrite(2, HIGH);
  delayMicroseconds(10);
  digitalWrite(2, LOW);
  pulseDuration[0] = pulseIn(3, HIGH);
  delay(5);
  digitalWrite(4, HIGH);
  delayMicroseconds(10);
  digitalWrite(4, LOW);
  pulseDuration[0] = pulseIn(5, HIGH);
  delay(5);
  */
  if (counter >= 2){
    counter = 0;
    for (int i = 0; i < PINS_LEN; i++) {
      Serial.print("D_");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(pulseDuration[i]/INCH_SCALAR);
      Serial.print("inches | "); 
    }
    Serial.println();
  }

  

  //delay(DELAY - (DELAY / 10));



  // Start the reading process by pulling TRIG high for 10us
  /*digitalWrite(SENSOR_0_TRIG, HIGH);
   //PINC |= 0x10;
   delayMicroseconds(10);
   digitalWrite(SENSOR_0_TRIG, LOW);
   //PINC &= ~(0x10);
   
   // The ECHO pin will return a pulse in microseconds. To convert to distance use:
   //   uS/58  = centimeters
   //   us/148 = inch
   time[0] = micros();
   pulseDuration[0] = pulseIn(SENSOR_0_ECHO, HIGH);
   time[0] = micros() - time[0];  
   
   delay(5);
   digitalWrite(SENSOR_1_TRIG, HIGH);
   //PINC |= 0x20;
   delayMicroseconds(10);
   digitalWrite(SENSOR_1_TRIG, LOW);
   //PINC &= ~(0x20);
   
   time[1] = micros();
   pulseDuration[1] = pulseIn(SENSOR_1_ECHO, HIGH);
   time[1] = micros() - time[1];
   
   Serial.print("D_0: ");
   Serial.print(pulseDuration[0]/INCH_SCALAR);
   Serial.print(" inches (");
   Serial.print(100.0 * (time[0] - pulseDuration[0])/time[0]);
   Serial.print("% time waiting) | D_1: ");
   Serial.print(pulseDuration[1]/INCH_SCALAR);
   Serial.print(" inches (");
   Serial.print(100.0 * (time[1] - pulseDuration[1])/time[1]);
   Serial.println("% time waiting)");
   */
  // Wait for a bit to not flood the serial port

}

