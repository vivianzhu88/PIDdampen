#include <Servo.h>
#include <unistd.h>
using namespace std;


Servo servo;

//private variables
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 1000;  //the value is a number of milliseconds, ie 1 second
int m_highV = 90;
int m_lowV = 10;
int m_period = 300;
int accelerationFactor = 10;

//servo setup
void setup() 
{
  servo.attach(10);
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
  int count = 0;
  int position = 0;
  int value = 0;
  int i;
  startMillis = millis();
  char newKey;
  
  while(true)
  {
    //newKey = waitKey(100);
    currentMillis = millis();
    position = (currentMillis - startMillis) % (period*4);
    value = highOrLow(position);
    for (i = 0; i <=  value; i++)
    {
      servo.write(i);
      delay(accelerationFactor);
    }
  }
  //} while(newKey != 's');
  
}
