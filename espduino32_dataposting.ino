/**
 * WQMS HttpClient for data posting
 * By Josiah Kotey
 * Created on: 23.05.2019
 * Project: Water Quality Monitoring and Notification System
 */

////////////////// for Sensors and ADS1115/////////////////////
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





/////////////////// for http Client///////////////////
#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#define MY_SERIAL Serial

WiFiMulti wifiMulti;


void setup() {
  
    MY_SERIAL.begin(115200);

    MY_SERIAL.println();
    MY_SERIAL.println();
    MY_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        MY_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        MY_SERIAL.flush();
        delay(1000);
    }

    wifiMulti.addAP("WorkSHop", "inf12345");
    wifiMulti.addAP("J-THEORY 3878", "98?J365o");

    while (wifiMulti.run()!= WL_CONNECTED) { //Check for the connection
    delay(1000);
    MY_SERIAL.println("Connecting to WiFi..");
  }
 
   MY_SERIAL.println("Connected to the WiFi network");
 


////////////////////Setup for the sensors and ads1115//////////////////    
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
 
  ads.begin(); //  enables the ADC1115 
  MY_SERIAL.println("Initializing All Sensors.....................");


 delay(3000);   



}

void loop() {
      
      float mtemp,turb,ph,level;  /// variables to hold sensor values(data)
      mtemp= mytemp();         // hold temperature data
      turb=myturb();         //  hold turbidity data
      ph=myph();             //  hold pH data     
      level=mylevel();       //  hold water level data 
     
  
    // wait for WiFi connection
    if((wifiMulti.run() == WL_CONNECTED)) {
       
        MY_SERIAL.println(" ...after if statement");
        HTTPClient http;
       // MY_SERIAL.println(" ...immediately after httpClient object");
        MY_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        http.begin("https://wqms.herokuapp.com/postData"); //HTTP
        //http.begin("http://10.10.64.99:5050/postData"); //HTTP

        /// defining a variabble to hold all values from sensors
        String ourdata = String(mtemp)+","+String(turb)+","+String(ph)+","+String(level);
       //M MY_SERIAL.println(" helloo");
        MY_SERIAL.println(ourdata);

        MY_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        http.addHeader("Content-Type","text/plain");        
        int httpCode = http.POST(ourdata);

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            MY_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                MY_SERIAL.println(payload);
            }
        }
        
        else {
            MY_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(30000);
}

///////////////////Turbidity Sensor//////////////////////
// increased turbidity---=> Decreased voltage

float myturb(){
  int16_t adc1;  // we read from the ADC, we have a sixteen bit integer as a result
 
 adc1 = ads.readADC_SingleEnded(1);
  float voltage = (adc1 * 0.1875)/1000; //converting analog reading to voltage (digital value)
  senseTurbidity= voltage+1; // conveting sensor voltage to 5V 
//Serial.println(senseRawValue);
//senseRawValue = analogRead(senseInput); //Read input raw value fromt the sensor
//senseTurbidity = senseRawValue * (5.0 / 1024.0); //Convert analog data from 0 -1024 to voltage 0 - 5v;
  return senseTurbidity;
  MY_SERIAL.print("TURBIDITY VALUE:  "); //Print the output data to the serial
  MY_SERIAL.println(senseTurbidity);
  MY_SERIAL.print("\n");
  delay(1000);

  if (senseTurbidity>=3.90 ){
     MY_SERIAL.println("\t Water is clear \n");
    
    }
   
  if (senseTurbidity<3.90 && senseTurbidity>=3.30 ){
     MY_SERIAL.println("\t Water is not clear \n");
   
    }

  else if(senseTurbidity<3.30)
    MY_SERIAL.println("\t Warning. Water is muddy or very cloudy!!!!!!! \n");
  
  }

  
//////////////////////Ultrasonic Sensor//////////////////////
  float mylevel(){
  
      // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    /*
    // Calculating the distance for my tank
    mydistance= duration/58;
    distance= tankheight-mydistance;
    */
     distance=duration / 58; 
     return distance;
        MY_SERIAL.println(distance);
    // Prints the distance on the Serial Monitor
        MY_SERIAL.print("Distance: ");
        MY_SERIAL.println(distance);
      if (distance<=10&& distance>=5){
        MY_SERIAL.println("The water level: FULL");
       
       
    }

      else if (distance>10 && distance<=16){
        MY_SERIAL.println("The water level: NORMAL");
      
      
    }

     else if (distance>16){
        MY_SERIAL.println("The water level: LOW");
      
        
        
    }
    
    delay(1000);


  }

///////////////////// pH Sensor ///////////////////////
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
  
  
  MY_SERIAL.print("Sensor = ");
  MY_SERIAL.println(phval);

   MY_SERIAL.print("Voltage = ");
  MY_SERIAL.println(phvol);
  delay(1000);

  if (phval <=1 || phval>13.90){
    MY_SERIAL.print("Check the pH meter");
    return 13.89 ;
    }
  return phval;
  }


  ////////////////////Temperature Sensor//////////////////
   float mytemp(){
    float temp;
  DS18B20.requestTemperatures(); 
  temp=DS18B20.getTempCByIndex(0);
  MY_SERIAL.print("Temperature: ");
  MY_SERIAL.println(temp);
  if (temp<=0){
   float rtemp=random(253,262)/10.0; // generate a number for testing
   return rtemp;
   }
  else{
    return temp;
  }
  delay(1000);
  }


/*   void buzz(){
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

*/
