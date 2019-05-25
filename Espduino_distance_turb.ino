

// lcd ----- SDA=D2/GPIO4,  SCL=D1/GPIO5
#define senseInput 02   //Set to A0 as Analog Read
int senseRawValue; //Some variable
float senseTurbidity; //Some floating variable
#define analogpin 04 

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

void setup(){
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  Serial.begin(115200); //Set as serial communication baud rate 9600
  Serial.println("  TURBIDITY AND ULTRASONIC SENSORS ");
  Serial.println("Initializing.................................");
  Serial.println("Maxim / Dallas DS18B20 readout program");
 
  delay(4000);
}
void loop(){
    myturb();
    delay(1000);
    mylevel();
    delay(1000);
    myph();

}

void myturb(){
Serial.println(senseRawValue);
senseRawValue = analogRead(senseInput); //Read input raw value fromt the sensor
senseTurbidity = senseRawValue * (5.0 / 1024.0); //Convert analog data from 0 -1024 to voltage 0 - 5v;
Serial.print("TURBIDITY VALUE > "); //Print the output data to the serial
Serial.println(senseTurbidity);
delay(500);

  if (senseTurbidity>4.0 ){
     Serial.println("Water is clear ");
    }
   
  if (senseTurbidity<4.0 && senseTurbidity>3.5 ){
     Serial.println("Water is not clear ");
    }

  else if (senseTurbidity<3.5 && senseTurbidity>2.7){
    Serial.println("Water is very dirt");
    }  
  else if(senseTurbidity<2.7)
    Serial.println("Warning. Water is muddy or very cloudy!!!!!!!");
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
    /*
    // Calculating the distance for the tank
    mydistance= duration/58;
    distance= tankheight-mydistance;
    */
    distance=duration / 58; 
    Serial.println(distance);
    // Prints the distance on the Serial Monitor
    Serial.print("Distance: ");
    Serial.println(distance);
          if (distance<10&& distance>=5){
    Serial.println("The water level: FULL");
    }

      else if (distance>10 && distance<16){
    Serial.println("The water level: NORMAL");
    }

      else if (distance>16){
    Serial.println("The water level: LOW");
    }
    
    delay(500);


  }

// PH FUNCTION 
  float myph(){
  
  for(int i=0;i<10;i++){
   buf[i]= analogRead(analogpin);
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
  float phvol1=(float)avgval/6;
  Serial.print(phvol1);
  float phvol=(float)avgval*(5.0/1024)/6 ;
  float phval= -3.6585*phvol+21.864;
  Serial.print("Sensor = ");
  Serial.println(phval);

   Serial.print("Voltage = ");
  Serial.println(phvol);
  delay(100);
  
  }
