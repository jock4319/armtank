#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <Wire.h>

#define CE_PIN 7
#define CSN_PIN 8

#define SERVO_L 9
#define SERVO_R 10

#define DELAY            20  // Delay per loop in ms


RF24 radio(CE_PIN, CSN_PIN);

const byte address[6] = "JOCKS";
byte cmd[5];

Servo rservo;  // create servo object to control a servo
Servo lservo;

char message[32];

void setup() {
  Serial.begin(115200);
  
  radio.begin();
  radio.enableAckPayload();
  radio.setPALevel(RF24_PA_MIN);
  
  Serial.println("Nrf24L01 Receiver Starting");
  radio.openReadingPipe(1,address);
  radio.startListening();
  memset(cmd, 0, sizeof(cmd));

  lservo.attach(SERVO_L);
  rservo.attach(SERVO_R);

  // set neutral to activate servo
  for ( int i = 0; i < 30; i++ )
  {
    delay(100);
    rservo.write(90);
    lservo.write(90);
  }
}

void loop() {
  while ( radio.available() )
  {
      radio.read( cmd, sizeof(cmd) );
      //Serial.println(String((char *)cmd));
      if ( cmd[0] == 'S' && cmd[3] == 'E' ) {
        sprintf(message, "%d, %d", (int)cmd[1], (int)cmd[2]);
        Serial.println(message);
        lservo.write((int)cmd[1]);
        rservo.write((int)cmd[2]);
      }
  }
  

  rservo.write(90);
  lservo.write(90);

  //delay(DELAY);
}
