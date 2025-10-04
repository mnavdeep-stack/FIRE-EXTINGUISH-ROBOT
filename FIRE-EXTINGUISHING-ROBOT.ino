/*------ Arduino Fire Fighting Robot ---- */
#include <SoftwareSerial.h>   // Include SoftwareSerial.h library
#include <Servo.h>            // Include servo.h library  

// --- Definitions ---
Servo myservo;
int pos = 0;
boolean fire = false;
int bt_data; // Variable to receive data from the serial port
int mode = 1;

// --- Pin Assignments ---
#define rxPinBluetooth 2
#define txPinBluetooth 3

#define GAS_SENSOR 11    // Gas sensor
#define Left 10          // Left sensor
#define Right 8          // Right sensor
#define Forward 9        // Front sensor

#define RM1 4            // Right motor 1
#define RM2 5            // Right motor 2
#define LM1 6            // Left motor 1
#define LM2 7            // Left motor 2
#define pump 12

SoftwareSerial BT_Serial(rxPinBluetooth, txPinBluetooth);

void setup() {
  BT_Serial.begin(9600);
  Serial.begin(9600);

  pinMode(Left, INPUT);
  pinMode(Right, INPUT);
  pinMode(Forward, INPUT);
  pinMode(GAS_SENSOR, INPUT);
  pinMode(RM1, OUTPUT);
  pinMode(RM2, OUTPUT);
  pinMode(LM1, OUTPUT);
  pinMode(LM2, OUTPUT);
  pinMode(pump, OUTPUT);

  myservo.attach(13);
  myservo.write(90); 

  digitalWrite(pump, HIGH);   
}

void loop() {
  myservo.write(90); // Keep servo centered

  if (BT_Serial.available() > 0) {     
    bt_data = BT_Serial.read();  
  }

  // --- Mode Selection ---
  if (bt_data == 8) {
    mode = 1; // Auto mode
  } else if (bt_data == 9) {
    mode = 0; // Manual mode
  }

  // --- Manual Control ---
  if (mode == 0) {
    if      (bt_data == 1) { forward(); }
    else if (bt_data == 2) { backward(); }
    else if (bt_data == 3) { turnLeft(); }
    else if (bt_data == 4) { turnRight(); }
    else if (bt_data == 5) { Stop(); }
    else if (bt_data == 6) { put_off_fire(); }
    delay(10);
  }
  // --- Automatic Control ---
  else {
    if (digitalRead(Left) == 1 && digitalRead(Right) == 1 && digitalRead(Forward) == 1) {
      delay(500);
      Stop();
      delay(500);
    }
    else if (digitalRead(Forward) == 0) {
      forward();
      fire = true;
    }
    else if (digitalRead(Left) == 0) {
      turnLeft();
    }
    else if (digitalRead(Right) == 0) {
      turnRight();
    }
    delay(200); // Change this value to change the distance

    if (digitalRead(GAS_SENSOR) == 0) {
      Serial.println("Gas is Detected.");
      put_off_fire();
    }

    while (fire == true) {
      put_off_fire();
      Serial.println("Fire Detected.");
    }
  }
}

// --- Fire Extinguishing Routine ---
void put_off_fire() {
  Stop();
  digitalWrite(pump, LOW);
  delay(300);
 
  for (pos = 50; pos <= 130; pos += 1) { 
    myservo.write(pos); 
    delay(10);  
  }
  for (pos = 130; pos >= 50; pos -= 1) { 
    myservo.write(pos); 
    delay(10);
  }
  digitalWrite(pump, HIGH);
  myservo.write(90); 
  fire = false;
}

// --- Movement Routines ---
void forward() {
  digitalWrite(RM1, HIGH);
  digitalWrite(RM2, LOW);
  digitalWrite(LM1, HIGH);
  digitalWrite(LM2, LOW);
}

void backward() {
  digitalWrite(RM1, LOW);
  digitalWrite(RM2, HIGH);
  digitalWrite(LM1, LOW);
  digitalWrite(LM2, HIGH);
}

void turnRight() {
  digitalWrite(RM1, LOW);
  digitalWrite(RM2, HIGH);
  digitalWrite(LM1, HIGH);
  digitalWrite(LM2, LOW);
}

void turnLeft() {
  digitalWrite(RM1, HIGH);
  digitalWrite(RM2, LOW);
  digitalWrite(LM1, LOW);
  digitalWrite(LM2, HIGH);
}

void Stop() {
  digitalWrite(RM1, LOW);
  digitalWrite(RM2, LOW);
  digitalWrite(LM1, LOW);
  digitalWrite(LM2, LOW);
}
