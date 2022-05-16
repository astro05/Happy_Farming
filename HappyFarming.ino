#include <Wire.h>
#include<Servo.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include "DHT.h"

 
#include <SPI.h>        // Include SPI library (needed for the SD card)
#include <SD.h>         // Include SD library
File dataFile;



//Timer DS1307
uint8_t second, minute, hour, wday, day, month, year, ctrl;

//Test Pin
int TestLed=13;

//LDR Pin
int ldr = A0;
int LdrValue=0;

//Rain
int Rain =7;
int rain;

//Servo
Servo WindowServo;
int WindowServoAngle=0;
Servo HuTempServo;
int HuTempServoAngle=0;
Servo FlameGasServo;


//DHT22
#define DHTPIN 22            // DHT22 data pin is connected to Arduino pin 8
LiquidCrystal HumTempLcd(23, 24, 25, 26, 27, 28);  // LCD module connections (RS, E, D4, D5, D6, D7) 
#define DHTTYPE DHT22       // DHT22 sensor is used
DHT dht(DHTPIN, DHTTYPE);   // Initialize DHT library
char temperature[] = "Temp = 00.0 C";
char humidity[]    = "RH   = 00.0 %";
//MOTOR driver
const int IN1 = 34;
const int IN2 = 33;
const int IN3 = 32;
const int IN4 = 31;

const int ENA = 35;
const int ENB = 30;


//Flame & Smoke
int flame = 41;
int smoke = 40;
int FlameLedRed = 37;
int FlameLedGreen = 38;
int FlameBuzzer = 36;


//UltraSonic Grooming Brush
int trigpin =2;
int echopin=3;
long duration;
int distance;
int BrushMotor = 4;

//PH & Mostiure
#define Waterecho 6
#define Watertrigger 5
#define tank_pump 43
#define watering_pump 42
#define moisture_sensor A1
LiquidCrystal PHlcd(44,45,46,47,48,49);



//Optimum values
float optemperature = 35;
float ophumidity = 50;
float opmoisture = 75;



//Variables
int Rhumidity,Rtemperature;
float tRhumidity,tRtemperature;
int FlameValue,SmokeValue;

long Wduration;
int Wdistance;
int moisture_value;
int distance_percent;
int moist_percent;




void setup() {
Serial.begin(9600);   // Setting the baud rate of GSM Module  
Serial.setTimeout(10);

pinMode(TestLed, OUTPUT);
WindowServo.attach(9); // attach the servo in arduino pin 
HuTempServo.attach(29); // attach the servo in arduino pin 

//Rain
pinMode (Rain , INPUT);



//DHT22 Humidity Temperature
HumTempLcd.begin(20,4);
dht.begin();
//Motor Driver
pinMode (IN1, OUTPUT);
pinMode (IN2, OUTPUT);
pinMode (IN3, OUTPUT);
pinMode (IN4, OUTPUT);

pinMode (ENA, OUTPUT);
pinMode (ENB, OUTPUT);



//Flame & Smoke
pinMode (flame, INPUT) ;
pinMode (smoke, INPUT) ;
pinMode (FlameLedRed, OUTPUT) ;
pinMode (FlameLedGreen, OUTPUT) ;
pinMode (FlameBuzzer, OUTPUT) ;
FlameGasServo.attach(39);
FlameGasServo.write(0);


//UltraSonic Grooming Brush
pinMode(trigpin,OUTPUT);  
pinMode(echopin,INPUT);
pinMode(BrushMotor,OUTPUT);  


//PH & Moistuere
PHlcd.begin(20,4);
pinMode(Waterecho,INPUT);
pinMode(moisture_sensor,INPUT);
pinMode(Watertrigger,OUTPUT);
digitalWrite(Watertrigger,LOW);
pinMode(watering_pump,OUTPUT);
pinMode(tank_pump,OUTPUT);
digitalWrite(watering_pump,LOW);
digitalWrite(tank_pump,LOW);

PHlcd.setCursor(0,1);
PHlcd.print("  Happy Farming");
PHlcd.setCursor(0,2);
PHlcd.print("Md.Moniruzzaman JOY");
PHlcd.setCursor(0,3);
PHlcd.print("      WELCOME");

delay(500);
PHlcd.clear(); 


//MMC
Serial.print("Initializing SD card...");
  if (!SD.begin()) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");


//open the file for reading:
  dataFile = SD.open("temp.txt");
  if (dataFile) {
   
    // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      Serial.write(dataFile.read());
       //optemperature =  Serial.parseFloat();
    }
    // close the file:
    dataFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening OptimumTepm.csv");
  }

Serial.println(optemperature);


//open the file for reading:
  dataFile = SD.open("ophum.txt");
  if (dataFile) {
 
    // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      Serial.write(dataFile.read());
      // ophumidity =  Serial.parseFloat();
    }
    // close the file:
    dataFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening ophum.txt");
  }

Serial.println(ophumidity);




//open the file for reading:
  dataFile = SD.open("opmoist.txt");
  if (dataFile) {
 
    // read from the file until there's nothing else in it:
    while (dataFile.available()) {
      Serial.write(dataFile.read());
       //opmoisture =  Serial.parseFloat();
    }
    // close the file:
    dataFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening opmoist.txt");
  }

Serial.println(opmoisture);










//Setup END
}

uint16_t line = 1;


void loop() {
//Flame & Gas 
FlameGas();

//Rain
Rainy();
 
//DHT22 Humidity Temperature
HumTempDht22();
tRhumidity = Rhumidity/10.0;
tRtemperature = Rtemperature/10.0;

if(tRhumidity < ophumidity && tRtemperature < optemperature)
  {
     analogWrite(ENA, 255);
     analogWrite(ENB, 255); 

     digitalWrite(IN1, LOW);
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);

     HuTempServoAngle = 5;
     HuTempServo.write(HuTempServoAngle);
     Serial.print("1HUTempServo Angle:");
     Serial.println(HuTempServoAngle);
     
     Serial.print("1HU:");
     Serial.println(tRhumidity);
           Serial.print("1tem:");
     Serial.println(tRtemperature);
       
  }
  else if(tRhumidity < ophumidity && tRtemperature > optemperature)
  {
     analogWrite(ENA, 255);
     analogWrite(ENB, 255); 

     digitalWrite(IN1, LOW);
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);

    HuTempServoAngle = 30;
     HuTempServo.write(HuTempServoAngle);
     Serial.print("2HUTempServo Angle:");
     Serial.println(HuTempServoAngle);
     
     Serial.print("2HU:");
     Serial.println(tRhumidity);
           Serial.print("2tem:");
     Serial.println(tRtemperature);
    
  }
  else if(tRhumidity > ophumidity && tRtemperature < optemperature)
  {
     analogWrite(ENA, 255);
     analogWrite(ENB, 255); 

     digitalWrite(IN1, LOW);
     digitalWrite(IN2, HIGH);
     digitalWrite(IN3, LOW);
     digitalWrite(IN4, LOW);

     HuTempServoAngle = 0;
     HuTempServo.write(HuTempServoAngle);
     Serial.print("3HUTempServo Angle:");
     Serial.println(HuTempServoAngle);
     
     Serial.print("3HU:");
     Serial.println(tRhumidity);
           Serial.print("3tem:");
     Serial.println(tRtemperature);
    
  }
  else
  {
     analogWrite(ENA, 255);
     analogWrite(ENB, 255); 

     digitalWrite(IN1, HIGH);
     digitalWrite(IN2, LOW);
     digitalWrite(IN3, HIGH);
     digitalWrite(IN4, LOW);

     HuTempServoAngle = 0;
     HuTempServo.write(HuTempServoAngle);
     Serial.print("4HUTempServo Angle:");
     Serial.println(HuTempServoAngle);
     
     Serial.print("4HU:");
     Serial.println(tRhumidity);
           Serial.print("4tem:");
     Serial.println(tRtemperature);
    
  }

LdrValue = analogRead(ldr);
   Serial.print("LDR value:");
   Serial.println(LdrValue);

  HumTempLcd.setCursor(0, 1);
  HumTempLcd.print("LDR Value:");
  HumTempLcd.print(LdrValue);



//Time
if (read_ds1307())
{
    print_time();
} else {
    Serial.println("Timer DS1307 ISN'T working");
  }

  
if((hour<23))
{
  
   digitalWrite(TestLed,HIGH);
  // delay(400);

  if(rain ==0)
  {
 
    if(!(LdrValue>300 && LdrValue<500))
     {
   
    if(WindowServoAngle >= 0 && WindowServoAngle < 360)
      {
       Serial.print("Servo Angle:");
       Serial.println(WindowServoAngle);
       WindowServoAngle = WindowServoAngle +20;
       WindowServo.write(WindowServoAngle);
       LdrValue = analogRead(ldr);
       Serial.print("Ldr Value:");
       Serial.println(LdrValue);
      }
   if(WindowServoAngle >= 360)
     {
      WindowServoAngle =0;
      WindowServo.write(WindowServoAngle);
      Serial.print("Servo Angle:");
      Serial.println(WindowServoAngle);
     }
   }
  
  }
  if(rain == 1)
  {
      WindowServoAngle =0;
      WindowServo.write(WindowServoAngle);
      Serial.print("Servo Angle:");
      Serial.println(WindowServoAngle);
    
  }  

/////////
}
else {
      digitalWrite(TestLed,LOW);
}


//UltraSonic Grooming Brush
UltraSonicGroomingBrush();



//PH & Moisture

 // LEVEL SENSOR
 digitalWrite(Watertrigger,LOW);
 delayMicroseconds(2);
 digitalWrite(Watertrigger,HIGH);
 delayMicroseconds(10); 
 digitalWrite(Watertrigger,LOW);
 Wduration=pulseIn(Waterecho,HIGH);
 Wdistance=Wduration*0.017; 
 distance_percent=map( Wdistance,0,1023,0,100);
 moisture_value= analogRead(moisture_sensor);
moist_percent=map(moisture_value,0,1023,0,100);
PHandMoisture();


//MMC Data
//MMCData();
////////////

  dataFile = SD.open("t.csv", FILE_WRITE);
// if the file opened okay, write to it:
  if (dataFile) {
   // Serial.print(line);
  //  Serial.print(":    Temperature = ");

    Serial.print(day);
    Serial.print("/");
    Serial.print(month);
    Serial.print("/");
    Serial.print(year);
    Serial.print(",");
    
    Serial.print(rain);
    Serial.print(",");
    Serial.print(LdrValue);
    Serial.print(",");
    Serial.print(tRtemperature);
    Serial.print(",");
    Serial.print(tRhumidity);
    Serial.print(",");
    Serial.print(FlameValue);
    Serial.print(",");
    Serial.print(SmokeValue);
    Serial.print(",");
    Serial.print(distance_percent);
    Serial.print(",");
   Serial.println(moist_percent);


    dataFile.print(day);
    dataFile.print("/");
    dataFile.print(month);
    dataFile.print("/");
    dataFile.print(year);
    dataFile.print(",");
    
    dataFile.print(rain);
    dataFile.print(",");
    dataFile.print(LdrValue);
    dataFile.print(",");
    dataFile.print(tRtemperature);
    dataFile.print(",");
    dataFile.print(tRhumidity);
    dataFile.print(",");
    dataFile.print(FlameValue);
    dataFile.print(",");
    dataFile.print(SmokeValue);
    dataFile.print(",");
    dataFile.print(distance_percent);
    dataFile.print(",");
    dataFile.println(moist_percent);
   
  


    
    dataFile.close();
    
  }
  // if the file didn't open, print an error:
  else
    Serial.println("error opening t.txt");
  
 


  


//Loop END
}

//Rain
void Rainy()
{
  rain = digitalRead(Rain);

if(rain==1)
{
  Serial.println("Its Raining");
}
else if(rain==0)
{
  Serial.println("No Rain");
}

  
}



bool read_ds1307()
{
  // Iniciar el intercambio de información con el DS1307 (0xD0)
  Wire.beginTransmission(0x68);

  // Escribir la dirección del segundero
  Wire.write(0x00);

  // Terminamos la escritura y verificamos si el DS1307 respondio
  // Si la escritura se llevo a cabo el metodo endTransmission retorna 0
  if (Wire.endTransmission() != 0)
    return false;

  // Si el DS1307 esta presente, comenzar la lectura de 8 bytes
  Wire.requestFrom(0x68, 8);

  // Recibimos el byte del registro 0x00 y lo convertimos a binario
  second = bcd2bin(Wire.read());
  minute = bcd2bin(Wire.read()); // Continuamos recibiendo cada uno de los registros
  hour = bcd2bin(Wire.read());
  wday = bcd2bin(Wire.read());
  day = bcd2bin(Wire.read());
  month = bcd2bin(Wire.read());
  year = bcd2bin(Wire.read());

  // Recibir los datos del registro de control en la dirección 0x07
  ctrl = Wire.read();

  // Operacion satisfactoria, retornamos verdadero
  return true;
}

uint8_t bcd2bin(uint8_t bcd)
{
  // Convertir decenas y luego unidades a un numero binario
  return (bcd / 16 * 10) + (bcd % 16);
}
void print_time()
{
  Serial.print("Date: ");
  Serial.print(day);
  Serial.print('/');
  Serial.print(month);
  Serial.print('/');
  Serial.print(year);

  Serial.print("  Hour: ");
  Serial.print(hour);
  Serial.print(':');
  Serial.print(minute);
  Serial.print(':');
  Serial.print(second);

  Serial.println();

  HumTempLcd.setCursor(0, 0);
    HumTempLcd.print("Date:");
  HumTempLcd.print(day);
  HumTempLcd.print("/");
  HumTempLcd.print(month);
  HumTempLcd.print("/");
  HumTempLcd.print(year);

  if(rain == 0)
  {
    HumTempLcd.print(" NO Rain");
  }
  
  if(rain == 1)
  {
    HumTempLcd.print(" Raining");
  }
  
  
}


void HumTempDht22()
{
  // Read humidity
  Rhumidity = dht.readHumidity() * 10;
  //Read temperature in degree Celsius
  Rtemperature = dht.readTemperature() * 10;
  
  // Check if any reads failed and exit early (to try again)
  if (isnan(Rhumidity) || isnan(Rtemperature)) {
    HumTempLcd.clear();
    HumTempLcd.setCursor(5, 0);
    HumTempLcd.print("Error:DHT22");
    return;
  }
 
  if(Rtemperature < 0){
    temperature[6] = '-';
    Rtemperature = abs(Rtemperature);
  }
  else
    temperature[6] = ' ';
  temperature[7]   = (Rtemperature / 100) % 10  + 48;
  temperature[8]   = (Rtemperature / 10)  % 10  + 48;
  temperature[10]  =  Rtemperature % 10 + 48;
  temperature[11]  = 223;        // Degree symbol ( °)
  if(Rhumidity >= 1000)
    humidity[6]    = '1';
  else
    humidity[6]    = ' ';
  humidity[7]      = (Rhumidity / 100) % 10 + 48;
  humidity[8]      = (Rhumidity / 10) % 10 + 48;
  humidity[10]     =  Rhumidity % 10 + 48;
  HumTempLcd.setCursor(0, 2);
  HumTempLcd.print(temperature);
  HumTempLcd.setCursor(0, 3);
  HumTempLcd.print(humidity);
  
}


void FlameGas()
{
  FlameValue = digitalRead (flame) ;
  SmokeValue = digitalRead(smoke);
  Serial.print("Flame = ");
  Serial.println(FlameValue);
  Serial.print("Smoke = ");
  Serial.println(SmokeValue);

  if ( SmokeValue == HIGH or FlameValue == HIGH)
  {
    Serial.println(" WARNING! ");
    digitalWrite(FlameLedRed, HIGH);
    digitalWrite(FlameBuzzer, HIGH);
    digitalWrite(FlameLedGreen, LOW);
    FlameGasServo.write(90);
  }
  else
  {
    Serial.println(" SAFE ");
    digitalWrite(FlameLedRed, LOW);
    digitalWrite(FlameLedGreen, HIGH);
    digitalWrite(FlameBuzzer, LOW);
    FlameGasServo.write(0);
  }
  
}


void UltraSonicGroomingBrush()
{
  digitalWrite(trigpin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigpin,HIGH);
  delayMicroseconds(10);


  digitalWrite(trigpin,LOW);

  duration = pulseIn(echopin,HIGH);
  distance = duration*0.034/2;  // sound travel in per miliseconds is 0.034m

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println("cm");

  if(distance<300)
  {
    digitalWrite(BrushMotor,HIGH);
  }
  else
  {
    digitalWrite(BrushMotor,LOW);
  }
 
  
}




void PHandMoisture(){

if (distance_percent>65 &&moist_percent<opmoisture){
PHLCD_3();
digitalWrite(tank_pump,LOW);
digitalWrite(watering_pump,HIGH);

}
else if (distance_percent<65 &&moist_percent>opmoisture)
{
PHLCD_2();
digitalWrite(tank_pump,HIGH);
digitalWrite(watering_pump,LOW);

}
else if (distance_percent>65 &&moist_percent>opmoisture)
{

PHLCD_4();
digitalWrite(tank_pump,LOW);
digitalWrite(watering_pump,LOW);

}

else if (distance_percent<65 &&moist_percent<opmoisture)
{
PHLCD_1();
digitalWrite(tank_pump,HIGH);
digitalWrite(watering_pump,HIGH);

}

}

void PHLCD_1()
  {
  PHlcd.clear();
  PHlcd.setCursor(0,0);
  PHlcd.print("TANK LEVEL=  ");
  PHlcd.print(distance_percent);
  PHlcd.print("%");
  PHlcd.setCursor(0,1);
  PHlcd.print("MOIST CONTENT= ");
  PHlcd.print(moist_percent);
  PHlcd.print("%");
  PHlcd.setCursor(0,2);
  PHlcd.print("W-PUMP STATUS  ");
  PHlcd.print("  ON");
  PHlcd.setCursor(0,3);
  PHlcd.print("T-PUMP STATUS  ");
  PHlcd.print("  ON");
  }

void PHLCD_2(){
  PHlcd.clear();
  PHlcd.setCursor(0,0);
  PHlcd.print("TANK LEVEL=  ");
  PHlcd.print(distance_percent);
  PHlcd.print("%");
  PHlcd.setCursor(0,1);
  PHlcd.print("MOIST CONTENT= ");
  PHlcd.print(moist_percent);
  PHlcd.print("%");
  PHlcd.setCursor(0,2);
  PHlcd.print("W-PUMP STATUS ");
  PHlcd.print("  OFF");
  PHlcd.setCursor(0,3);
  PHlcd.print("T-PUMP STATUS ");
  PHlcd.print("   ON");
  }
  void PHLCD_3(){
  PHlcd.clear();
  PHlcd.setCursor(0,0);
  PHlcd.print("TANK LEVEL=  ");
  PHlcd.print(distance_percent);
  PHlcd.print("%");
  PHlcd.setCursor(0,1);
  PHlcd.print("MOIST CONTENT= ");
  PHlcd.print(moist_percent);
  PHlcd.print("%");
  PHlcd.setCursor(0,2);
  PHlcd.print("W-PUMP STATUS ");
  PHlcd.print("  ON");
  PHlcd.setCursor(0,3);
  PHlcd.print("T-PUMP STATUS ");
  PHlcd.print("  OFF");
  }

  void PHLCD_4(){
  PHlcd.clear();
  PHlcd.setCursor(0,0);
  PHlcd.print("TANK LEVEL=  ");
  PHlcd.print(distance_percent);
  PHlcd.print("%");
  PHlcd.setCursor(0,1);
  PHlcd.print("MOIST CONTENT= ");
  PHlcd.print(moist_percent);
  PHlcd.print("%");
  PHlcd.setCursor(0,2);
  PHlcd.print("W-PUMP STATUS");
  PHlcd.print("  OFF");
  PHlcd.setCursor(0,3);
  PHlcd.print("T-PUMP STATUS");
  PHlcd.print("  OFF");
  }







  
