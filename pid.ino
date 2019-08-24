#include <Servo.h>
#include <unistd.h>
#include <math.h>
using namespace std;

Servo servo;

//private variables
unsigned long startMillis;
unsigned long currentMillis;
unsigned long prevMillis;

const unsigned long period = 1000;  //the value is a number of milliseconds, ie 1 second
int m_highV = 90;
int m_lowV = 10;
int m_period = 400;
int m_ampFactor = 0.25;
int m_dampFactor =  0.05;
int m_dampPeriod = 0.16;
int m_phaseOffset = 3.75;
int servoSpeed = 30;

float maxVel = 20.0; // in degrees per second
float maxAcc = 5.0;  // in degrees per second ^2
float stepPerDeg = 255.0/180.0; // conversion factor steps per degree
float currAngle = 0;
float currVel = 0;
float dampWave = 0;

//servo setup
void setup() 
{
  servo.attach(10);
  Serial.begin(9600);
}

//gives servo value based on position in function
int highOrLow(int val)
{
  if ((val < m_period) || (val > m_period*3))
  {
    return m_lowV;
  }
  else
  {
    return m_highV;
  }
}

void loop() 
{
  int position = 0;
  int value = 0;
  startMillis = millis();
  int timeElapsed;
  char newKey;
  
  while(true)
  {
    //update time since last loop and calc Delta
    prevMillis = currentMillis;
    currentMillis = millis();
    float timeDelta = currentMillis - prevMillis / 1000.0;

    //figure out where we are in the setpoint waveform
    timeElapsed = currentMillis - startMillis;
    position = timeElapsed % (m_period*4);
    value = highOrLow(position);

    //calc our current velocity and check that we are not above max
    currVel += maxAcc * timeDelta;
    if(currVel > maxVel) currVel = maxVel;

    //check which direction we need to go and calc the new angle, otherwise zero our velocity
    if((int) currAngle != value ){
      currAngle += copysign(currVel * timeDelta, value - currAngle);
      dampWave = m_ampFactor * cos((timeDelta * m_dampPeriod) + m_phaseOffset) * exp(-m_dampFactor * timeDelta);
    }
    else {
      currAngle = value;
      currVel =0;
    }
    //go to our new position
    servo.write(currAngle * stepPerDeg + dampWave);
    Serial.println(currAngle * stepPerDeg + dampWave);
  }
}
