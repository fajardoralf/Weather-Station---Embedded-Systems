/*
 * Eksamen 2017
 * Laget av Ralf Laurent Fajardo
 * 
 * Valgte å dokumentere koden på engelsk
 * Så alle kan forstå koden
 */
 
//When you use my program
//Remember to uncomment setRTCTime();
//And set the current time,date and day of week
//Run the program
//then comment setRTCTime();



#include <Adafruit_ST7735.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include "Sodaq_DS3231.h"
#include <Sodaq_DS3231.h>
#include <SPI.h>
#include "DHT.h"
#define DHTTYPE DHT11

#define TFT_CS     10
#define TFT_RST    8                      
#define TFT_DC     9
#define DHTPIN 2
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

#define TFT_SCLK 13
#define TFT_MOSI 11


//To print in TFT
char timeChar[100];
char dateChar[50];
char dayChar[50];
char lights[50];
char tempString[50];
char humString[50];
char lightString[50];
char waterStr[50];
char waterSens[50];
char humidity[50];
char temperature[50];

//Leds
int LED1 = 7;//Temp light
int LED2 = 6;//Humidity light
int LED3 = 5;//Green light
int LED4 = 4;//Water light
int LED5 = 12;

//Buzzer
int buzzerPin = 3;
int tones[] = {440};



//Light sensor
const int numReadings = 10;
int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average
int lightSensor = A0;

//For time and date
String dateString;
String dayString;
int minuteNow=0;
int minutePrevious=0;

// values for the sensors
int nowTemp;
int previousTemp = 0;
int nowHum = 0;
int previousHum = 0;
int nowWater;
int previousWater = 0;
int previousAverage = 0;
int lightAverage = 20; 



DHT dht(DHTPIN, DHTTYPE);

void setup () 
{
    tft.initR(INITR_BLACKTAB);
    tft.fillScreen(ST7735_BLACK);
    delay(100);
    Wire.begin();
    rtc.begin();
    dht.begin();
    //Uncomment when you use it for the first time
    //setRTCTime(); //after set comment again
    ui();
    //LEDS
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
       //Lightsensor
      for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}
uint32_t old_ts;

void loop () 
{
  leds();
  time();
  readTemperature();
  readHumidity();
  delay(1);
  lightSensors();
  waterSensor();

  delay(1000);
}

void setRTCTime()
{
  DateTime dt(2017, 6, 8, 18, 44, 00, 4); // Year, Month, Day, Hour, Minutes, Seconds, Day of Week
  rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above 
}

void printText(char *text, uint16_t color, int x, int y,int textSize)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(textSize);
  tft.setTextWrap(true);
  tft.print(text);
}

String getDayOfWeek(int i)
{
  switch(i)
  {
    case 1: return "Mandag";break;
    case 2: return "Tirsdag";break;
    case 3: return "Onsdag";break;
    case 4: return "Torsdag";break;
    case 5: return "Fredag";break;
    case 6: return "Lørdag";break;
    case 7: return "Søndag";break;
    default: return "Mandag";break;
  }
}

void time()
{
    DateTime now = rtc.now(); //get the current date-time
    uint32_t ts = now.getEpoch();

    if (old_ts == 0 || old_ts != ts) 
    {
  old_ts = ts;
  
  minuteNow = now.minute();
  if(minuteNow!=minutePrevious)
  {
    dayString = getDayOfWeek(now.dayOfWeek());
    dateString = String(now.date())+"/"+String(now.month());
    dateString= dateString+"/"+ String(now.year()); 
    minutePrevious = minuteNow;
    String hours = String(now.hour());
    if(now.minute()<10)
    {
      hours = hours+":0"+String(now.minute());
    }else
    {
      hours = hours+":"+String(now.minute());
    }
   
    hours.toCharArray(timeChar,100);
    //Fill date and day
    tft.fillRect(4,10,120,40,ST7735_BLACK);
    //Fill time 
    tft.fillRect(4,55,120,33,ST7735_BLACK);
    printText(timeChar, ST7735_WHITE,19,60,3);
    dateString.toCharArray(dateChar,50);
    printText(dateChar, ST7735_GREEN,40,10,1);
    dayString.toCharArray(dayChar, 50);
    printText(dayChar, ST7735_GREEN,21,30,2);
  }
 }
}

//Humidity
void readHumidity()
{
      previousHum = nowHum;
      nowHum = dht.readHumidity();
    if(nowHum != previousHum)
    {
      String hum = String(dht.readHumidity()) + "%";
      tft.fillRect(50,140,33,15,ST7735_BLACK);
      hum.toCharArray(humidity,100);
      String humi = "Humidity";
      humi.toCharArray(humString, 100);
      printText(humString, ST7735_GREEN, 40, 127, 1.9);
      printText(humidity,ST7735_YELLOW, 50, 140, 1.9);
    }  
}
//Temperature
void readTemperature()
{
    previousTemp = nowTemp;
    nowTemp = dht.readTemperature();
    if(nowTemp != previousTemp)
    {
    String tmp = String(dht.readTemperature()) + "C";
    tft.fillRect(7,140,33,15,ST7735_BLACK);
    tmp.toCharArray(humidity,100);
    String temp = "Temp";
    temp.toCharArray(tempString, 50);
    printText(tempString,ST7735_RED,7,127,1.9);
    printText(humidity,ST7735_WHITE, 7,140,1.9);
    }
}
void lightSensors()
{
    total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(lightSensor);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }
  // calculate the average:
    average = total / numReadings;
  // send it to the computer as ASCII digits
    previousAverage = lightAverage;
    lightAverage = average;
   
    if(lightAverage != previousAverage)
    {
    int input = average / 10;
    String light = String(input) + "%";
    tft.fillRect(91,140,20,15,ST7735_BLACK);
    light.toCharArray(lights,100);
    String lightS = "Light";
    lightS.toCharArray(lightString, 50);
    printText(lightString,ST7735_RED,90,127,1.9);
    printText(lights,ST7735_WHITE, 100,140,1.9);
    }  
}

//WaterSensor
void waterSensor()
{
  
  previousWater = nowWater;
  nowWater = analogRead(A1);
 
  if(nowWater != previousWater)
  {
    int water = nowWater /10;
    String waterString = String(water) + "%";
    tft.fillRect(50,110,25,10,ST7735_BLACK);
    String waterName = "Water";
    waterName.toCharArray(waterStr, 50); 
    waterString.toCharArray(waterSens, 50);
    printText(waterStr,ST7735_CYAN, 47,100,1);
    printText(waterSens, ST7735_BLUE,55,110,1.9); 
  }
  
}

//Leds logic
void leds()
{
  if(nowTemp < 30 && nowHum < 50 && nowWater < 50 && lightAverage > 100)
  {
    if(nowHum < 50)
    {
      digitalWrite(LED2, LOW);  
    }

    if(nowTemp < 30)
    {
        digitalWrite(LED1, LOW);  
    }
    if(nowWater < 50)
    {
       digitalWrite(LED4, LOW);  
    }
    if(lightAverage > 200)
    {
      digitalWrite(LED5,LOW);  
    }

     digitalWrite(LED3,HIGH);
  }
  else if(nowTemp > 30)
  {
    
    digitalWrite(LED3, LOW); 
    digitalWrite(LED2, HIGH);
    toneSound();
    delay(5000);
  }
  else if(nowHum > 50)
  {
    digitalWrite(LED3, LOW);
    digitalWrite(LED1, HIGH);
    toneSound();
    delay(5000);
  }
  else if(nowWater > 200)
  {
     digitalWrite(LED3, LOW);
     digitalWrite(LED4,HIGH);
     toneSound();
    delay(5000);  
  }
  else if(lightAverage < 100)
  {
    digitalWrite(LED3, LOW);
    digitalWrite(LED5, HIGH);
    toneSound();
    delay(5000);  
  }
  else
  {
      digitalWrite(LED3, LOW);  
  }
}
//Sound for the buzzer
void toneSound()
{
    tone(buzzerPin, tones);
    delay(500);
  
    noTone(buzzerPin);     
}
//Draws the rectangles around date/day and sensors. 
void ui()
{
 tft.drawRoundRect(1,3,125,50,1,ST7735_WHITE); 
 tft.drawRoundRect(1,90,125,70,1,ST7735_GREEN);  
}

