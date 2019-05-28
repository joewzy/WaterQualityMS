/*created May 24,2019
 * By Josiah
 * On Espduino-32
 */

#include <Wire.h>
#include <Adafruit_ADS1015.h>

Adafruit_ADS1115 ads(0x48);
float Voltage = 0.0;

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 18               // GPIO pin on which the DS18B20 is connected :D5 on esp12e

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);


// lcd ----- SDA=D2/GPIO4,  SCL=D1/GPIO5
#define senseInput    //Set to A0 as Analog Read
int senseRawValue; //Some variable
float senseTurbidity; //Some floating variable
#define analogpin  

const int trigPin = 12;
const int echoPin = 13;
// defines variables
long duration;
int distance;
int tankheight=27;
int mydistance;
int buf[10],temp;
int sensorval=0;
long int avgval;

int buzzer = 19;
int red = 23;
int green = 14;


void setup(){
  // for the ads1115(analog extender)  
  ads.begin();
  ///////  sensor setups or requirements
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(115200); //Set as serial communication baud rate 9600
  Serial.println("  TURBIDITY AND ULTRASONIC SENSORS ");
  Serial.println("Initializing.................................");
  Serial.println("Maxim / Dallas DS18B20 readout program"); 

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(buzzer,OUTPUT);
 
  digitalWrite(red,LOW);
  greenled();
  
  delay(4000);


  
}
void loop(){
    myturb();
    delay(1000);
    mylevel();
    delay(1000);
    myph();
    delay(1000);
    mytemp();

}

void myturb(){
  int16_t adc1;  // we read from the ADC, we have a sixteen bit integer as a result
 
 adc1 = ads.readADC_SingleEnded(1);
  float voltage = (adc1 * 0.1875)/1000;
  senseTurbidity= voltage+1;
//Serial.println(senseRawValue);
//senseRawValue = analogRead(senseInput); //Read input raw value fromt the sensor
//senseTurbidity = senseRawValue * (5.0 / 1024.0); //Convert analog data from 0 -1024 to voltage 0 - 5v;
Serial.print("TURBIDITY VALUE --> "); //Print the output data to the serial
Serial.println(senseTurbidity);
delay(1000);

  if (senseTurbidity>=3.90 ){
     Serial.println("\t The Water is clear \n");
     greenled();
    }
   
  if (senseTurbidity<3.90 && senseTurbidity>=3.30 ){
     Serial.println("\t The Water is a bit clear \n");
     buzz();
    }

  else if(senseTurbidity<3.30){
    Serial.println("\t Warning. Water is very cloudy,Change or Treat Water!!! \n");
    redled();
    buzz();
    delay(2);
    buzz();
  }
  }



  // increased turbidity, our voltage drops 

  void mylevel(){
  
      // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
     //distance=duration / 58;  // Measure Level of unknown distance(height)
    // Calculating the distance for the tank
    mydistance= duration/58;
    distance= tankheight-mydistance;
    
    //distance=duration / 58; 
    Serial.println(distance);
    // Prints the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(distance);
          if (distance<=10&& distance>=5){
    Serial.println("The water level: FULL");
    greenled();
    }

    else if (distance>10 && distance<=16){
    Serial.println("The water level: NORMAL");
  greenled();

    }

    else if (distance>16){
      Serial.println("The water level: LOW");
      buzz();
      redled();
      delay(2);
      buzz();
    }
    
    delay(1000);


  }

// PH FUNCTION 
  float myph(){

//////// using the ads1115 for the ph meter
 int16_t adc0;  // we read from the ADC, we have a sixteen bit integer as a result

  adc0 = ads.readADC_SingleEnded(0);
 
  
  for(int i=0;i<10;i++){
   //buf[i]= analogRead(analogpin);
   buf[i]= adc0;
    delay(100);
    }
  for(int i=0;i<9;i++){
    for(int j=i;j<10;j++){
      if(buf[i]>buf[j]){
        temp=buf[j];
        buf[i]=buf[j];
        buf[j]=temp;       
        
        }      
      }
       
    }  
  avgval=0;
  for(int i=2;i<8;i++){avgval+=buf[i];    }

 // float phvol=(float)avgval*(5.0/1024)/6 ;
  float ads_avg= avgval/6;
  float phvol= (ads_avg * 0.1875)/1000;
  float phval= -3.7429*phvol + 15.791;
  Serial.print("Sensor = ");
  Serial.println(phval);

   Serial.print("Voltage = ");
  Serial.println(phvol);
  delay(100);

  if (phval <=0 || phval>13.90){
    
    redled();
    buzz();
    delay(1);
    buzz();
    }
  
  }


    //////////temperature Sensor//////////////////
   float mytemp(){
    float temp;
  DS18B20.requestTemperatures(); 
  temp=DS18B20.getTempCByIndex(0);
  Serial.print("Temperature: ");
  if (temp<=0){
   float rtemp=26.25;
   Serial.println(rtemp);
   return rtemp;
   }
  else{
    Serial.println(temp);
    return temp;
  }
  delay(1000);
  }


  void buzz(){
   int i;
 for(i=0;i<80;i++)//output a voice of one frequency
{ 
    digitalWrite(buzzer,HIGH);//have voice 
    delay(1);//delay 1ms 
    digitalWrite(buzzer,LOW);//do not have voice 
    delay(1);//delay 1ms
 

}
for(i=0;i<100;i++)//output a voice of another frequency
{ 
    digitalWrite(buzzer,HIGH);//have voice 
    delay(2);//delay 2ms 
    digitalWrite(buzzer,LOW);//without voice 
    delay(2);//delay 2ms
}
    digitalWrite(buzzer,LOW);
  }
  

///////////////RGB---Red led ON function//////////////////////
void redled(){

    digitalWrite(red,HIGH);
    delay(1000);
    digitalWrite(red,LOW);
}


////////////////RGB---Green led ON function///////////////////
void greenled(){
  digitalWrite(green,HIGH);
  delay(1000);
  digitalWrite(green,LOW);
}
