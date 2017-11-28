#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//#include "WiiChuckClass.h" //most likely its WiiChuck.h for the rest of us.
#include "WiiChuck.h"

// Setup for nRF24L01+
#define CE_PIN 7
#define CSN_PIN 8

#define RAD2DEG(r)  ((r)*57.2958)

#define JOY_X_MIN     -90
#define JOY_X_MAX     90
#define JOY_X_MIN_RANGE   JOY_X_MIN //(JOY_X_MIN*0.9)
#define JOY_X_MAX_RANGE   JOY_X_MAX //(JOY_X_MAX*0.9)
#define JOY_Y_MIN     -90
#define JOY_Y_MAX     90
#define SERVO_MIN     0  //40
#define SERVO_MAX     180

#define DELAY   20

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "JOCKS";

WiiChuck chuck = WiiChuck();

int valX, valY;
int valL, valR;

byte cmd[5];

void setup() {
  //nunchuck_init();
  Serial.begin(115200);

  radio.begin();
  radio.enableAckPayload();
  radio.setPALevel(RF24_PA_MIN);
  Serial.println("Nrf24L01 Transmitter Starting");
  radio.openWritingPipe(address);
  radio.stopListening();
  memset(cmd, 0, sizeof(cmd));
  cmd[0] = 'S';
  cmd[3] = 'E';
  
  chuck.begin();
  chuck.update();
  chuck.calibrateJoy();

  // set neutral to activate servo
  int i;
  for ( i = 0; i < 10; i++ )
  {
    chuck.update();
    delay(100);
    chuck.calibrateJoy();
  }
}

void loop() {
  delay(DELAY);
  chuck.update(); 
  
  valX = chuck.readJoyX();
  Serial.print(valX);
  Serial.print(", ");
  valY = chuck.readJoyY();
  Serial.print(valY);
  Serial.print(", ");
  
  float thu = sqrt(valX*valX+valY*valY);
  //thu = pow(thu/SERVO_MAX, 2)*SERVO_MAX;
  thu = thu*thu/JOY_X_MAX;
  float rad, deg;
  /*if ( thu < 5 )
  {
    valL = 90;
    valR = 90;
  }
  else*/ if ( valY >= 0 )
  {
    rad = atan2(abs(valY), abs(valX));
    //rad = sqrt(rad/PI*2)*PI/2;
    /*if ( deg <= 30 && thu < 30 )
    {
      valL = 90;
      valR = 90;
    }
    else*/
    {
      if ( valX >= 0 )
      {
        valL = (int) constrain(map((int)thu, JOY_X_MIN, JOY_X_MAX, SERVO_MIN, SERVO_MAX), SERVO_MIN, SERVO_MAX);
        valR = (int) constrain(map((int)(thu*sin(rad)), JOY_X_MIN, JOY_X_MAX, SERVO_MIN, SERVO_MAX), SERVO_MIN, SERVO_MAX);
      }
      else if ( valX < 0 )
      {
        valR = (int) constrain(map((int)thu, JOY_X_MIN, JOY_X_MAX, SERVO_MIN, SERVO_MAX), SERVO_MIN, SERVO_MAX);
        valL = (int) constrain(map((int)(thu*sin(rad)), JOY_X_MIN, JOY_X_MAX, SERVO_MIN, SERVO_MAX), SERVO_MIN, SERVO_MAX);
      }
    }
  }
  else if ( valY < 0 )
  {
    rad = atan2(abs(valY), abs(valX));
    //rad = sqrt(rad/PI*2)*PI/2;
    /*if ( deg > 60 && thu < 30 )
    {
      valL = 90;
      valR = 90;
    }
    else*/
    {
      if ( valX >= 0 )
      {
        valR = (int) constrain(map((int)(-1*thu), JOY_X_MIN, JOY_X_MAX, SERVO_MIN, 180), SERVO_MIN, SERVO_MAX);
        valL = (int) constrain(map((int)(-1.0*thu*sin(rad)), JOY_X_MIN, JOY_X_MAX, SERVO_MIN, SERVO_MAX), SERVO_MIN, SERVO_MAX);
      }
      else if ( valX < 0 )
      {
        valL = (int) constrain(map((int)(-1*thu), JOY_X_MIN, JOY_X_MAX, SERVO_MIN, SERVO_MAX), SERVO_MIN, SERVO_MAX);
        valR = (int) constrain(map((int)(-1.0*thu*sin(rad)), JOY_X_MIN, JOY_X_MAX, SERVO_MIN, SERVO_MAX), SERVO_MIN, SERVO_MAX);
      }
    }
  }
  else
  {
    valL = 90;
    valR = 90;
  }
  Serial.print(" ^");
  Serial.print(thu);
  Serial.print(" ~");
  Serial.print(RAD2DEG(rad));
  Serial.print("  | ");
  Serial.print(valL);
  //Serial.print("-");
  Serial.print(valL == valR ? "=" : (valL > valR ? ">" : "<"));
  Serial.println(valR);
  //rservo.write(valR);
  //lservo.write(valL);
  cmd[1] = valL;
  cmd[2] = valR;
  radio.write(&cmd, sizeof(cmd));

  
#if 0
  if (chuck.buttonZ) {
     Serial.print("Z");
  } else  {
     Serial.print("-");
  }

    Serial.print(", ");  

  if (chuck.buttonC) {
     Serial.print("C");
  } else  {
     Serial.print("-");
  }

    Serial.println();
#endif
}
