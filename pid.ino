#include <Servo.h>
#include <unistd.h>
#include <math.h>
using namespace std;

Servo servo;

//private variables
unsigned long startMillis;
unsigned long currentMillis;
unsigned long prevMillis;
unsigned long dampMillis;

const unsigned long period = 1000;  //the value is a number of milliseconds, ie 1 second
int m_highV = 90;
int m_lowV = 10;
int m_period = 4000;
float m_ampFactor = 0.25;
float m_dampFactor =  0.05;
float m_dampPeriod = 0.16;
float m_phaseOffset = 3.75;
int servoSpeed = 30;

float maxVel = 20.0; // in degrees per second
float maxAcc = 5.0;  // in degrees per second ^2
float stepPerDeg = 255.0/180.0; // conversion factor steps per degree
float currAngle = 0;
float currVel = 0;
float dampening = 0;

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
  dampMillis = startMillis;
  int timeElapsed;
  char newKey;
  int prevValue = 0;
  
  while(true)
  {
    //update time since last loop and calc Delta
    prevMillis = currentMillis;
    currentMillis = millis();
    float timeDelta = (currentMillis - prevMillis) / 1000.0;
    float dampTimeDelta = (currentMillis - dampMillis) / 1000.0;

    //figure out where we are in the setpoint waveform
    timeElapsed = currentMillis - startMillis;
    position = timeElapsed % (m_period*4);
    prevValue = value;
    value = highOrLow(position);
    if (prevValue != value) 
    {
      dampMillis = millis();
      int setpointDelta = value - prevValue; //need to use the amplitude of the change to scale the damping
    }
    //calc our current velocity and check that we are not above max
    currVel += maxAcc * timeDelta;
    if(currVel > maxVel) currVel = maxVel;

    //check which direction we need to go and calc the new angle, otherwise zero our velocity
    if((int)currAngle != value ){
      float distToTravel = currVel * timeDelta;
      float direction = value - currAngle;
      currAngle += copysign(distToTravel, direction);
      dampening = m_ampFactor * setpointDelta * cos((dampTimeDelta * m_dampPeriod) + m_phaseOffset) * exp(-m_dampFactor * dampTimeDelta);
    }
    else {
      currAngle = value;
      currVel = 0;
      dampening = 0;
    }
    //go to our new position
    servo.write(currAngle * stepPerDeg + dampening);
    Serial.println(currAngle * stepPerDeg + dampening);
  }
}
