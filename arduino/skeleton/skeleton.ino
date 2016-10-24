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

signed long pulseDuration[2];
signed long time[2];

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

  // Wait for a bit to not flood the serial port
  delay(45);
}
