#include <Arduino.h>
// #define debug 1

//PINS!
//H-Bridge
const int LEFT_ENABLE = 5;   //brown   //pwm
const int LEFT_BACKWARD = 2;   //grey
const int LEFT_FORWARD = 3;  //white   //pwm but we aint using it
const int RIGHT_FORWARD = 8; //green   
const int RIGHT_BACKWARD = 4; //blue
const int RIGHT_ENABLE = 6;  //purple   //pwm
//Reciever
const int RX_PIN1 = A3; //left stick x
const int RX_PIN2 = A2; //left stick y
const int RX_PIN3 = A1; //right stick y
const int RX_PIN4 = A0; //right stick x
const int RX_PIN5 = 15; //pushbutton
const int RX_PIN6 = 14; //left front knob
const int RX_PIN7 = 16; //right shoulder knob
const int RX_PIN8 = 7;  //left shoulder knob
//LiDAR motor
//const int LIDAR = 3;

const int wheelsDeadzone = 1;
int leftWheels = 0;      //left wheels speed
int rightWheels = 0;     //right wheels speed

int wheelsPreload = 0;


class Channel {
  public:
  int Pin=0;
  int Deadzone=0;
  int Min;
  int Max;
  int Raw;
  int Cap;
  int Value;
  int read();
  Channel(int pin=0, int deadzone=0, int min = 1900, int max=1900):Pin(pin),Deadzone(deadzone),Min(min),Max(max){}
};

int Channel::read()
{ 
  //read the input
  Raw = pulseIn(Pin, HIGH, 20000);
  //map them to slightly larger ranges x10 so following has more resoultion to work with
  Value = map(Raw, Min, Max, -2600, 2600);  
  //deadzones
  if ((Value < Deadzone) && (Value > -Deadzone))
    Value = 0;
  else if (Value < Deadzone)
    Value = Value + Deadzone;
  else
    Value = Value - Deadzone;

  //then cap the vaules
  if (Value > 2550)
    Value = 2550;
  else if (Value < -2550)
    Value = -2550;

  Value = Value/10; 
  return Value;
}

void drive(int speed, int steer);

Channel Ch1(RX_PIN1,250,1075,1800);
Channel Ch2(RX_PIN2,250,1041,1856);
Channel Ch3(RX_PIN3,250,1034,1835); //????
Channel Ch4(RX_PIN4,250,1040,1852);
Channel Ch5(RX_PIN5,250,1040,1852); //placeholders
Channel Ch6(RX_PIN6,250,1852,1037);
Channel Ch7(RX_PIN7,250,1040,1852); //placeholders
Channel Ch8(RX_PIN8,250,1040,1852); //placeholders

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

  pinMode(OUTPUT, LEFT_ENABLE);
  pinMode(OUTPUT, LEFT_FORWARD);
  pinMode(OUTPUT, LEFT_BACKWARD);
  pinMode(OUTPUT, RIGHT_BACKWARD);
  pinMode(OUTPUT, RIGHT_FORWARD);
  pinMode(OUTPUT, RIGHT_ENABLE);

}

void loop()
{
  
  if(Serial)
  {
    #if debug == 1
      //left stick x
      Serial.print(">ch1:");
      Serial.println(Ch1.read());
      Serial.print(">ch1raw:");
      Serial.println(Ch1.Raw);

      //left stick y
      Serial.print(">ch2:");
      Serial.println(Ch2.read());
      Serial.print(">ch2raw:");
      Serial.println(Ch2.Raw);
      
      //right stick y
      Serial.print(">ch3:");
      Serial.println(Ch3.read());
      Serial.print(">ch3raw:");
      Serial.println(Ch3.Raw);
      
      //right stick x
      Serial.print(">ch4:");
      Serial.println(Ch4.read());
      Serial.print(">ch4raw:");
      Serial.println(Ch4.Raw);
      
      //pushbutton
      Serial.print(">ch5:");
      Serial.println(Ch5.read());
      Serial.print(">ch5raw:");
      Serial.println(Ch5.Raw);
      
      //left front knob
      Serial.print(">ch6:");
      Serial.println(Ch6.read());
      Serial.print(">ch6raw:");
      Serial.println(Ch6.Raw);
      
      //right shoulder knob
      Serial.print(">ch7:");
      Serial.println(Ch7.read());
      Serial.print(">ch7raw:");
      Serial.println(Ch7.Raw);

      //left shoulder knob
      Serial.print(">ch8:");
      Serial.println(Ch8.read());
      Serial.print(">ch8raw:");
      Serial.println(Ch8.Raw);
    #endif

    Ch1.read();
    Ch2.read();
    Ch3.read();
    Ch4.read();
    Ch5.read();
    Ch6.read();
    Ch7.read();
    Ch8.read();
    Serial.print(Ch1.read());
    Serial.print("\t");
    Serial.print(Ch2.read());
    Serial.print("\t");
    Serial.print(Ch3.read());
    Serial.print("\t");
    Serial.print(Ch4.read());
    Serial.print("\t");
    Serial.print(Ch5.read());
    Serial.print("\t");
    Serial.print(Ch6.read());
    Serial.print("\t");
    Serial.print(Ch7.read());
    Serial.print("\t");
    Serial.println(Ch8.read());

    if(Serial.available())
    {
      while(Serial.available())
      {
        Serial.parseInt();
      }
    }

  }

  // delay(500);
  if((!Ch1.Raw==0)&&(!Ch2.Raw==0)&&(!Ch3.Raw==0)&& //raw values == 0 == tx is off
  (!Ch5.Raw==0)&&(!Ch6.Raw==0)&&(!Ch7.Raw==0))     //excluding ch4 & ch8. no idea why.
  {
    drive(Ch2.Value, Ch1.Value);
  }

}


void drive(int speed, int steer)
{
    // expects -255 to +255 inputs. forward +, right +
    leftWheels = 0;
    rightWheels = 0;
    //wheelsPreload = (Ch6.Value + 257) / 4;
    wheelsPreload = 110;

    #if debug == 1
        Serial.print(">speed:");
        Serial.println(speed);

        Serial.print(">steer:");
        Serial.println(steer);
    #endif
    // steering mixer
    leftWheels = speed + steer;
    rightWheels = speed - steer;

    if ((speed > 0) && (leftWheels > speed))
        leftWheels = speed;
    if ((speed > 0) && (rightWheels > speed))
        rightWheels = speed;
    if ((speed < 0) && (leftWheels < speed))
        leftWheels = speed;
    if ((speed < 0) && (rightWheels < speed))
        rightWheels = speed;

    // wheelsspeed response curve
    if (leftWheels > 0)
        leftWheels = 2 + (2 * (pow(2, (float(leftWheels) / 32.0)))); // change linear stick movement to curve response
    else
        leftWheels = 2 - (2 * (pow(2, (float(abs(leftWheels) / 32.0))))); // because the mafs don't work for negative values

    if (rightWheels > 0)
        rightWheels = 2 + (2 * (pow(2, (float(rightWheels) / 32.0)))); // change linear stick movement to curve response
    else
        rightWheels = 2 - (2 * (pow(2, (float(abs(rightWheels) / 32.0))))); // because the mafs don't work for negative values

    // cap max wheels speed
    leftWheels = (leftWheels < 255) ? leftWheels : 255;   // cap max values
    leftWheels = (leftWheels > -255) ? leftWheels : -255; // cap max values
    // leftWheels = (leftWheels > 240) ? 255: leftWheels;  //top deadzone

    rightWheels = (rightWheels < 255) ? rightWheels : 255;
    rightWheels = (rightWheels > -255) ? rightWheels : -255;
    // rightWheels = (rightWheels > 240) ? 255: rightWheels;

    
    #if debug == 1
        Serial.print(">wheelsDeadZone:");
        Serial.println(wheelsDeadzone);
        Serial.print(">negwheelsDeadZone:");
        Serial.println(-wheelsDeadzone);
    #endif

    if ((leftWheels < wheelsDeadzone) && (leftWheels > -wheelsDeadzone))
    {
        leftWheels = 0;
        analogWrite(LEFT_ENABLE, 255);
        digitalWrite(LEFT_FORWARD, 0);
        digitalWrite(LEFT_BACKWARD, 0);
        #if debug == 1 
          Serial.println(">debug:1"); 
        #endif
    }
    else
    {
        if (leftWheels > -wheelsDeadzone)
        {
            digitalWrite(LEFT_FORWARD, 1);
            digitalWrite(LEFT_BACKWARD, 0);
            #if debug == 1 
              Serial.println(">debug:2"); 
            #endif
        }
        else if (leftWheels < wheelsDeadzone)
        {
            digitalWrite(LEFT_FORWARD, 0);
            digitalWrite(LEFT_BACKWARD, 1);
            #if debug == 1 
              Serial.println(">debug:3"); 
            #endif
        }
        leftWheels = map(abs(leftWheels), 0, 255, wheelsPreload, 255);
        analogWrite(LEFT_ENABLE, abs(leftWheels));
        #if debug == 1 
          Serial.println(">debug:4"); 
        #endif
    }

    if (rightWheels < wheelsDeadzone && rightWheels > -wheelsDeadzone)
    {
        rightWheels = 0;
        analogWrite(RIGHT_ENABLE, 0);
        digitalWrite(RIGHT_FORWARD, 0);
        digitalWrite(RIGHT_BACKWARD, 0);
        #if debug == 1 
          Serial.println(">debug:5"); 
        #endif
    }
    else
    {
        if (rightWheels > -wheelsDeadzone)
        {
            digitalWrite(RIGHT_FORWARD, 1);
            digitalWrite(RIGHT_BACKWARD, 0);
            #if debug == 1 
              Serial.println(">debug:6"); 
            #endif
        }
        else if (rightWheels < wheelsDeadzone)
        {
            digitalWrite(RIGHT_FORWARD, 0);
            digitalWrite(RIGHT_BACKWARD, 1);
            #if debug == 1 
              Serial.println(">debug:7"); 
            #endif
        }
        rightWheels = map(abs(rightWheels), 0, 255, wheelsPreload, 255);
        analogWrite(RIGHT_ENABLE, abs(rightWheels));
        #if debug == 1 
          Serial.println(">debug:8"); 
        #endif
    }

    
    #if debug == 1
    
        Serial.print(">wheelsPreload:");
        Serial.println(wheelsPreload);
        Serial.print(">leftWheels:");
        Serial.println(leftWheels);
        Serial.print(">rightWheels:");
        Serial.println(rightWheels);
    #endif
    /*
        if(rightWheels == 0) {
            analogWrite(RIGHT_wheels_ENABLE_PIN, 0);
            digitalWrite(RIGHT_wheels_FORWARDS_PIN, 0);
            digitalWrite(RIGHT_wheels_BACKWARDS_PIN, 0);
        } else {

        }
        */
}
