
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#define buzzer D5
#define red_led D6
#define blue_led D7
#define gas_sensor A0
#define DHTPIN D3
#define DHTTYPE 11
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

  byte degree_symbol[8] = 
              {
                0b00111,
                0b00101,
                0b00111,
                0b00000,
                0b00000,
                0b00000,
                0b00000,
                0b00000
              };
 byte noInternetChar[8] = {
    B10001,
    B01010,
    B00100,
    B01010,
    B10001,
    B00000,
    B00000,
    B00000
  };
  

//mail cuJwxm92tLX84JLt6N6iOa
//sheet cuJwxm92tLX84JLt6N6iOa
//Sensor_Alert
//Sensor_Data
//sda a4 D2
//scl a5 D1
//dht 2 arduino
//Vjk77krlj59esQRI_JzgmriSBZFizlBgjARCdDoD8SuWm2x_f3sukyDf2J0_Sq3B

int gasValue,maxvalue=360;
const char* ssid = "abcde";  //userid
const char* password = "abcdefghi";//password
const String ifttt_key = "Vjk77krlj59esQRI_JzgmriSBZFizlBgjARCdDoD8SuWm2x_f3sukyDf2J0_Sq3B"; //ifttt key
unsigned long sheetPreviousMillis = 0;
const long sheetInterval = 30000;  // time delay for update in google sheet
unsigned long mailPreviousMillis = 0;
const long errorInterval = 60000;  // time delay for senting mail for sensor error
int currentMessage = 1;
unsigned long lcdPreviousMillis = 0;
const long lcdInterval = 1000;//time delayfor lcd
unsigned long mailPreviousMillis2 = 0;
const long alertInterval = 30000;  //time delay for alert
unsigned long ledPreviousMillis = 0;
const long leddelay = 2000;  //time delay for alert

void google(String data1, String data2, String data3, String event_name) {
  String jsonData = "{\"value1\":\"" + data1 +
                    "\",\"value2\":\"" + data2 +
                    "\",\"value3\":\"" + data3 + "\"}";

  WiFiClient client;
  if (!client.connect("maker.ifttt.com", 80)) {
    Serial.println("Connection failed");
    return;
  } 

  // Send an HTTP POST request to IFTTT
  client.print("POST /trigger/" + event_name + "/with/key/" + ifttt_key + " HTTP/1.1\r\n");
  client.print("Host: maker.ifttt.com\r\n");
  client.print("Content-Type: application/json\r\n");
  client.print("Connection: close\r\n");
  client.print("Content-Length: " + String(jsonData.length()) + "\r\n\r\n");
  client.print(jsonData);

  // Wait for a response
  while (!client.available()) {
    delay(10);
  }

  // Read the response
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("Request sent");
  client.stop();
}
void network(){

  Serial.begin(115200);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
  
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
 
}

void setup() {
  pinMode(buzzer,OUTPUT);
  pinMode(red_led,OUTPUT);
  pinMode(blue_led,OUTPUT);
   digitalWrite(buzzer,LOW);
     digitalWrite(red_led,LOW);
      digitalWrite(blue_led,LOW);
  Serial.begin(115200);
  lcd.begin();
  dht.begin();
  lcd.setCursor(3,0);
  lcd.print("Air Quality");
  lcd.setCursor(3,1);
  lcd.print("Monitoring");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connecting ");
  lcd.setCursor(1, 1);
  lcd.print("to WiFi...");
  network();


}

void loop() {
  gasValue=analogRead(gas_sensor);
  //wifistatus
  if (WiFi.status() != WL_CONNECTED) {
           lcd.setCursor(14, 0);
           lcd.write(1);
           lcd.createChar(1, noInternetChar);}
  //read dht
  int humidity  = dht.readHumidity();
  
  float temperature = dht.readTemperature();
  //lcd display

  if(gasValue<maxvalue){
    digitalWrite(buzzer,LOW);
     digitalWrite(red_led,LOW);
      digitalWrite(blue_led,LOW);
  unsigned long currentLcdMillis = millis();
  if (currentLcdMillis - lcdPreviousMillis >= lcdInterval) {
    lcdPreviousMillis = currentLcdMillis;
    lcd.clear();
    
    switch (currentMessage) {
      case 1:
        if(isnan(humidity) || isnan(temperature)){
          lcd.setCursor(0,0);
        lcd.print("Error reading ");
        lcd.setCursor(2,1);
        lcd.print("Temperature! ");
        lcd.blink();
        }
        else{
        lcd.noBlink();
        lcd.setCursor(0,0);
        lcd.print("Temperature: "); 
        lcd.setCursor(0,1);
        lcd.print(temperature);
        lcd.setCursor(5,1);
        lcd.write(2);
        lcd.createChar(2, degree_symbol);
        lcd.setCursor(6,1);
        lcd.print("C");}
        break;
      case 2:
        lcd.clear();
        if(isnan(humidity) || isnan(temperature)){
          lcd.setCursor(0, 0);
          lcd.print("Error reading ");
          lcd.setCursor(2, 1);
          lcd.print("Humidity! ");
          lcd.blink();
        }
        else{
        lcd.noBlink();
        lcd.setCursor(0, 0);
        lcd.print("Humidity: ");
        lcd.setCursor(0, 1);
        lcd.print(humidity);
        lcd.print("%");}
        break;
      case 3:
        lcd.noBlink();
        lcd.setCursor(0,0);
        lcd.print("Gas Value: ");
        lcd.print(gasValue);
        lcd.setCursor(0, 1);
        lcd.print("Fresh Air");
        break;
    }
    currentMessage++;
    if (currentMessage > 3) {
      currentMessage = 1;
    }
  }
  }
  else{ 
       digitalWrite(red_led,HIGH);
      
      digitalWrite(buzzer,HIGH);
       unsigned long currentledMillis = millis();
          if (currentledMillis - ledPreviousMillis >= leddelay) {
          ledPreviousMillis = currentledMillis;
          digitalWrite(red_led,!digitalRead(red_led));
         
   }
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Gas Value: ");
        lcd.print(gasValue);
        lcd.setCursor(0, 1);
        lcd.print("AQ LEVEL HIGH!");
        lcd.blink();
          unsigned long currentmailMillis2 = millis();
          if (currentmailMillis2 - mailPreviousMillis2 >= alertInterval) {
          mailPreviousMillis2 = currentmailMillis2;
          google("AIR QUALITY LEVEL HIGH",String(gasValue),"~LIMIT EXCEEDED!","Sensor_Alert");
   }
  }
  
  //sent error to mail
   if (isnan(humidity) || isnan(temperature)) {
      unsigned long currentmailMillis = millis();
  if (currentmailMillis - mailPreviousMillis >= errorInterval) {
    mailPreviousMillis = currentmailMillis;
     google("ERROR IN DHT SENSOR",".","..","Sensor_Alert");
   }
    
 }
else{
   unsigned long currentsheetMillis = millis();//update google sheet
  if (currentsheetMillis - sheetPreviousMillis >= sheetInterval) {
    sheetPreviousMillis = currentsheetMillis;
     google(String(temperature)+"°C",String(humidity)+"%",String(gasValue),"Sensor_Data");
   }
}
//

   
}
    
  


