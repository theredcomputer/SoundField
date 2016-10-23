#define BAUD_RATE 115200
#define SENSOR_0_TRIG 2
#define SENSOR_0_ECHO 3
#define SENSOR_1_TRIG 12
#define SENSOR_1_ECHO 13
#define DELAY 1
#define INCH_SCALAR 148.0
#define CM_SCALAR 58.0

void setup() {
  Serial.begin(BAUD_RATE);
  pinMode(SENSOR_0_TRIG, OUTPUT);
  pinMode(SENSOR_0_ECHO, INPUT);
  pinMode(SENSOR_1_TRIG, OUTPUT);
  pinMode(SENSOR_1_ECHO, INPUT);
  //PORTC &= ~(0x30);
  //DDRC |= 0x30;
  
}

unsigned long pulseDuration[2];

void loop() {
  
  // Start the reading process by pulling TRIG high for 10us
  digitalWrite(SENSOR_0_TRIG, HIGH);
  //PINC |= 0x10;
  delayMicroseconds(10);
  digitalWrite(SENSOR_0_TRIG, LOW);
  //PINC &= ~(0x10);
  
  // The ECHO pin will return a pulse in microseconds. To convert to distance use:
  //   uS/58  = centimeters
  //   us/148 = inch
  pulseDuration[0] = pulseIn(SENSOR_0_ECHO, HIGH) / INCH_SCALAR;
  
  digitalWrite(SENSOR_1_TRIG, HIGH);
  //PINC |= 0x20;
  delayMicroseconds(10);
  digitalWrite(SENSOR_1_TRIG, LOW);
  //PINC &= ~(0x20);
  pulseDuration[1] = pulseIn(SENSOR_1_ECHO, HIGH) / INCH_SCALAR;

  Serial.print("D_0: ");
  Serial.print(pulseDuration[0]);
  Serial.print(" inches | D_1: ");
  Serial.print(pulseDuration[1]);
  Serial.println(" inches");
  
  // Wait for a bit to not flood the serial port
  delay(20);
  
  
}
