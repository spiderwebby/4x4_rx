#include <Arduino.h>


//PINS!
//H-Bridge
const int LEFT_ENABLE = 5;   //brown
const int LEFT_FORWARD = 2;   //grey
const int LEFT_BACKWARD = 3;  //white
const int RIGHT_BACKWARD = 8; //green
const int RIGHT_FORWARD = 4; //blue
const int RIGHT_ENABLE = 6;  //purple
//Reciever
const int RX_PIN1 = A3;
const int RX_PIN2 = A2;
const int RX_PIN3 = A1;
const int RX_PIN4 = A0;
const int RX_PIN5 = 15;
const int RX_PIN6 = 14;
const int RX_PIN7 = 16;
const int RX_PIN8 = 7;
//LiDAR motor
//const int LIDAR = 3;

class Channel {
  public:
  int Pin=0;
  int Deadzone=0;
  int Min;
  int Max;
  int Raw;
  int Cap;
  int read();
  Channel(int pin=0, int deadzone=0, int min = 1900, int max=1900):Pin(pin),Deadzone(deadzone),Min(min),Max(max){}
};

int Channel::read()
{
  int value; 
  //read the input
  Raw = pulseIn(Pin, HIGH, 200000);
  //map them to slightly larger ranges x10 so following has more resoultion to work with
  value = map(Raw, Min, Max, -2600, 2600);  
  //deadzones
  if ((value < Deadzone) && (value > -Deadzone))
    value = 0;
  else if (value < Deadzone)
    value = value + Deadzone;
  else
    value = value - Deadzone;

  //then cap the vaules
  if (value > 2550)
    value = 2550;
  else if (value < -2550)
    value = -2550;

  value = value/10; 
  return value;
}

Channel Ch1(RX_PIN1,250,1045,1810);
Channel Ch2(RX_PIN2,250);
Channel Ch3(RX_PIN3,250);
Channel Ch4(RX_PIN4,250);
Channel Ch5(RX_PIN5,250);
Channel Ch6(RX_PIN6,250);
Channel Ch7(RX_PIN7,250);
Channel Ch8(RX_PIN8,250);

void setup() {

  Serial.begin(115200);
  pinMode(INPUT, RX_PIN1);
  pinMode(INPUT, RX_PIN2);
  pinMode(INPUT, RX_PIN3);
  pinMode(INPUT, RX_PIN4);
  pinMode(INPUT, RX_PIN5);
  pinMode(INPUT, RX_PIN6);
  pinMode(INPUT, RX_PIN7);
  pinMode(INPUT, RX_PIN8);

}

void loop()
{
  
  Serial.print(Ch1.read());
  Serial.print(", ");
  Serial.print(Ch1.Raw);
  Serial.print(",\t");

  Serial.print(Ch2.read());
  Serial.print(", ");
  Serial.print(Ch2.Raw);
  Serial.print(",\t");
  
  Serial.print(Ch3.read());
  Serial.print(", ");
  Serial.print(Ch3.Raw);
  Serial.print(",\t");
  
  Serial.print(Ch4.read());
  Serial.print(", ");
  Serial.print(Ch4.Raw);
  Serial.print(",\t");
  
  Serial.print(Ch5.read());
  Serial.print(", ");
  Serial.print(Ch5.Raw);
  Serial.print(",\t");

  Serial.print(Ch6.read());
  Serial.print(", ");
  Serial.print(Ch6.Raw);
  Serial.print(",\t");

  Serial.print(Ch7.read());
  Serial.print(", ");
  Serial.print(Ch7.Raw);
  Serial.print(",\t");

  Serial.print(Ch8.read());
  Serial.print(", ");
  Serial.print(Ch8.Raw);
  Serial.println();
  delay(500);
}
