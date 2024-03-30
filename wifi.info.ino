#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>
#include <SoftwareSerial.h>

SoftwareSerial uno(D7,D8); //TX,RX

char auth[] = "nD_oe_OtTIY_tMqQHYj1ja44DGjAIfBl"; //Auth code sent via Email
char ssid[] = "Wifi"; //Wifi name
char pass[] = "78912345";  //Wifi Password

SimpleTimer timer;

int temp=0;
int voltg=0;
int Power=0;
int oil=0;
int rpm=0;

int Send=0, send_old;
int stop=0;

byte Data[8]={'0','0','0','0','0','0','0'};

int cont = 0;

void sendUptime(){
Blynk.virtualWrite(V0, temp);
Blynk.virtualWrite(V1, voltg);
Blynk.virtualWrite(V2, Power);
Blynk.virtualWrite(V3, oil);
Blynk.virtualWrite(V4, rpm);
}


const int led = 2; // GPIO  4 (D2) 

void setup() {
pinMode(led, OUTPUT);
Serial.begin(9600);//Starting serial comunication
uno.begin(9600);
delay(1000);


Blynk.begin(auth, ssid, pass);
timer.setInterval(2000, sendUptime);
digitalWrite(led, HIGH);
Serial.println("Welcome");
}



void loop(){
if(uno.available()>6){
    Data[0]=uno.read();                 
    if (Data[0]=='A'){                      
     Data[1]=uno.read();  
     Data[2]=uno.read(); 
     Data[3]=uno.read(); 
     Data[4]=uno.read();  
     Data[5]=uno.read(); 
     Data[6]=uno.read();                                                                               
}

temp  = Data[1];
voltg = Data[2];
Power = Data[3];
oil   = Data[4];
rpm   = Data[5];
Send  = Data[6];


Serial.print(temp);  Serial.print(" \t ");
Serial.print(voltg); Serial.print(" \t ");
Serial.print(Power);   Serial.print(" \t ");
Serial.print(oil);   Serial.print(" \t ");
Serial.print(rpm);   Serial.print(" \t ");
Serial.println(Send); 


if(Send==send_old){}
else{stop=0;}

if(Send==1 && stop==0){Blynk.notify("Over Temperature"); stop=1;}
if(Send==2 && stop==0){Blynk.notify("Over RPM"); stop=1;}
if(Send==3 && stop==0){Blynk.notify("Over Load"); stop=1;}
if(Send==4 && stop==0){Blynk.notify("Over voltag"); stop=1;}
if(Send==5 && stop==0){Blynk.notify("Under voltag"); stop=1;}
if(Send==6 && stop==0){Blynk.notify("Over Vibration"); stop=1;}
if(Send==7 && stop==0){Blynk.notify("Fual Level Low"); stop=1;}
if(Send==8 && stop==0){Blynk.notify("Load Unbalanced"); stop=1;}
send_old = Send;
} 

  
Blynk.run();
timer.run(); 
}
