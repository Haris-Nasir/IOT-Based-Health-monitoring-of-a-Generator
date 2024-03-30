#include <Wire.h>
#include <MPU6050.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(3,4,5,6,7,8);

#define echopin 11 // echo pin
#define trigpin 12 // Trigger pin

float Voltage1 = A1;           //Defining and initializing the voltage
float Current1 = A2;          // Defining and initializing the current

MPU6050 mpu;

int16_t ax, ay, az;
int16_t gx, gy, gz;

long X, Y, Z;

const int sensor=A3; 
int temp =0;  

int relay = 10;

int power = 0;
int old_power=0;
int V1 = 0;
float I1 = 0;

int dist;
long duration;

int volum = A0;
int pwm_pin = 9;
int pwm = 0;

// read the hall effect sensor on pin 2
const int hallPin=2;
const unsigned long sampleTime=3000;
int rpm;

int buz = 13;

int timer=0, timer1=0;
int tolerance = 10;
int Send=0;

void setup() {
 pinMode(Voltage1,INPUT);        //Set voltage as input pin
 pinMode(Current1,INPUT);        //Set current as input pin 
 pinMode(sensor,INPUT); 
 pinMode(volum,INPUT);  

 pinMode(hallPin,INPUT);

 pinMode (trigpin, OUTPUT);
 pinMode (echopin, INPUT);
 
 pinMode(relay, OUTPUT);
 pinMode(pwm_pin, OUTPUT);

 pinMode(buz, OUTPUT);

 Wire.begin();
 mpu.initialize();
 Serial.begin(9600);
    
 lcd.begin(16, 4);
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("     WELCOME    ");
 lcd.setCursor(0,1);
 lcd.print("     ");
 delay(2000);
 lcd.clear();

digitalWrite(relay,HIGH);
}

void loop(){    
mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
X = map(ax, -17000, 17000, 0, 90);
Y = map(ay, -17000, 17000, 0, 90);
Z = map(az, -17000, 17000, 0, 90);    

V1= ((analogRead(Voltage1)*(4.5/1023)*60));
I1= ((analogRead(Current1)*(4.5/1023)*1));
power = V1*I1;

dist = data();
dist = map(dist, 5, 35, 100, 0);
if(dist>100){dist=100;}
if(dist<0){dist=0;}

pwm = (analogRead(volum));
pwm = pwm/4; 
if(pwm>255){pwm=255;}
analogWrite(pwm_pin, pwm); 

rpm=getRPM();

temp=readTemp();

lcd.setCursor(0,0);lcd.print("V=");lcd.print(V1);lcd.print("   ");
lcd.setCursor(8,0);lcd.print("C=");lcd.print(I1);lcd.print("   ");
lcd.setCursor(0,1);lcd.print("P=");lcd.print(power);lcd.print("w   ");
lcd.setCursor(8,1);lcd.print("T=");lcd.print(temp); lcd.print("c   ");
lcd.setCursor(0,2);lcd.print("RPM=");lcd.print(rpm); lcd.print("   ");
lcd.setCursor(10,2);lcd.print("S=");lcd.print(pwm); lcd.print("   ");
lcd.setCursor(0,3);lcd.print("Oil=");lcd.print(dist); lcd.print("%   ");
lcd.setCursor(10,3);lcd.print("Y=");lcd.print(Y); lcd.print("   ");


timer1 = timer1+1;
if(timer1>10){timer = 0;}

if(power - old_power > tolerance){timer = timer+1; timer1= 0;}

Send = 0;
if(temp>50){ Send=1;
digitalWrite(buz,HIGH);    
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Temp Over");
delay(2000);
digitalWrite(buz,LOW);  
}

if(rpm>200){ Send=2;
digitalWrite(buz,HIGH);   
digitalWrite(relay,LOW);  
lcd.clear();
lcd.setCursor(0,0);
lcd.print(" RPM Over  ");
lcd.setCursor(0,1);
lcd.print("Load  OFF  ");
delay(2000);
digitalWrite(buz,LOW);  
}

if(power>250){ Send=3;
digitalWrite(buz,HIGH);   
digitalWrite(relay,LOW);  
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Over Load ");
lcd.setCursor(0,1);
lcd.print("Load  OFF ");
delay(2000);
digitalWrite(buz,LOW);  
}

if(V1>250){ Send=4;
digitalWrite(buz,HIGH);   
digitalWrite(relay,LOW);  
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Over voltag ");
lcd.setCursor(0,1);
lcd.print(" Load OFF   ");
delay(2000);  
}

if(V1<180){ Send=5;
digitalWrite(buz,HIGH);   
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Under voltag ");
delay(2000);  
}


if((Y<35)||(Y>60)){ Send=6;
digitalWrite(buz,HIGH);   
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Over Vibration ");
delay(2000); 
digitalWrite(buz,LOW); 
}


if(dist<20){ Send=7;
digitalWrite(buz,HIGH);   
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Fual Level Low");
delay(2000); 
digitalWrite(buz,LOW);   
}

if(timer>=4){ Send=8;
digitalWrite(buz,HIGH);   
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Load Unbalanced ");
lcd.setCursor(0,1);
lcd.print(" Fual");
delay(2000);
digitalWrite(buz,LOW);      
}

old_power = power;
wifi();
}

long data(){
 digitalWrite(trigpin,LOW);
 delayMicroseconds(2);
 digitalWrite(trigpin,HIGH);
 delayMicroseconds(10);
 duration=pulseIn (echopin,HIGH);
 return duration / 29 / 2; 
}

int readTemp() {  // get the temperature and convert it to celsius
  temp = analogRead(sensor);
  return temp * 0.48828125;
}

int getRPM(){
  // sample for sampleTime in millisecs
  int kount=0;
  boolean kflag=LOW;
  unsigned long currentTime=0;
  unsigned long startTime=millis();
  while (currentTime<=sampleTime)
  {
    if (digitalRead(hallPin)==HIGH)
    {
      kflag=HIGH;
    }
    if (digitalRead(hallPin)==LOW && kflag==HIGH)
    {
      kount++;
      kflag=LOW;
    }
    currentTime=millis()-startTime;
  }
  int kount2rpm = int(60000./float(sampleTime))*kount;
  return kount2rpm;
}


void wifi(){
Serial.write("A");
Serial.write(temp);
Serial.write(V1);
Serial.write(power); 
Serial.write(dist);
Serial.write(rpm);
Serial.write(Send); 
}
