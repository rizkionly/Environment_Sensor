#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "icon.h"
#include <NTPClient.h>
#include <WiFiUdp.h>
//#include "DHT.h"
//#define DHTPIN D1
//#define DHTTYPE DHT11

//DHT dht(DHTPIN, DHTTYPE);
//inisialisasi OLED
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1 

// Timing-related variables
#define tahun timeClient.getYear()
#define bulan timeClient.getMonth()
#define tanggal timeClient.getDate()
//#define hari daysOfTheWeek[timeClient.getDay()]
#define jam timeClient.getHours()
#define menit timeClient.getMinutes()
#define detik timeClient.getSeconds()
// Timezone rules for GMT+7 (Jakarta, Bangkok, Hanoi)
const long utcOffsetInSeconds = 25200;
//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);
WiFiClient espClient;
WiFiClient httpClient;
PubSubClient client(espClient);
HTTPClient http;

const char* guid                  = "18b91fec-957b-464e-9f3e-f4f716cf2d67";
const char* mqttHost              = "iwkrmq.pptik.id"; // Deklarasi untuk link yang akan dituju
const char* mqttUserName          = "/trainerkit:trainerkit"; // Deklarasi untuk nama UserName DI RMQ /survey:survey
const char* mqttPassword          = "12345678"; // Deklarasi untuk Passwordnya di RMQ $surv3yy!
const char* mqttQueue             = "Environment"; // Deklarasi untuk nama Queue di RMQ
const char* CL = guid;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
const char* ssid = "TEKIDO";      
const char* password = "iotworkshop2021"; 
String apiKey = "cbd4f560b4b5c040e76e653cda335ce9";       //openweather apikey
String city = "Bandung";              //City name
String units = "metric";              //or standart, imperial
String payload; 
int wifi_stat = 0;
int wifiIcon = LOW; 


const char* weather_0_main;
const char* weather_0_icon;
float main_temp = 0;
int main_humidity = 0;
String weatherr;
String Icon;
unsigned long previousMillis_1 = 0;  
const long interval_1 = 10000; //interval time HTTP req 

void setup () {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();
  timeClient.begin();
//  dht.begin();
  // just intro
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(24,20);  display.println(F("Welcome"));
  display.setTextSize(1);
  display.setCursor(10,40);  display.println(F("Environment Sensor"));
  display.display();
  delay(1000); 
  display.clearDisplay();
  WiFi.mode(WIFI_STA); 
  ConnectWIFI();
  client.setServer(mqttHost, 1883);
  delay(1000);
}
void loop() {
   if (!client.connected()) {
    reconnect();
    }
   client.loop();
   float dataSuhu;
//   dataSuhu = dht.readTemperature();
   Serial.println(dataSuhu);
   String convertDataSuhu = String(dataSuhu);
   String dataRMQ = String(String(guid) + "#" + dataSuhu );
   char dataToMQTT[50];
   dataRMQ.toCharArray(dataToMQTT, sizeof(dataToMQTT)); 
   Serial.println("Ini Data untuk ke MQTT: ");
   Serial.println(dataToMQTT);
   client.publish(mqttQueue,dataToMQTT);
   delay(1000);
    
  unsigned long currentMillis_1 = millis();
  DisplayUpdate();
    
  if (currentMillis_1 - previousMillis_1 >= interval_1) {
    previousMillis_1 = currentMillis_1; 
    req_data();
  } 
}


void req_data(){
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
      String address;
      //http://api.openweathermap.org/data/2.5/weather?q=****,ID&appid=******&units=metric
      address ="http://api.openweathermap.org/data/2.5/weather?q=";
      address += city;
      address += "&appid="; 
      address += apiKey ;
      address += "&units=";
      address += units;  
      http.begin(httpClient,address);  //Specify request destination
      int httpCode = http.GET();//Send the request
       
      if (httpCode > 0) { //Check the returning code               
          payload = http.getString();   //Get the request response payload
          Serial.println(payload);      //Print the response payload
      }
     
      http.end();   //Close connection
      const size_t capacity = JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(8) + JSON_OBJECT_SIZE(14) + 290;
       
      DynamicJsonDocument doc(capacity);
      
      // Deserialize the JSON document
      DeserializationError error = deserializeJson(doc, payload);
      
      // Test if parsing succeeds.
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
      }
      
     JsonObject weather_0 = doc["weather"][0];
     weather_0_main = weather_0["main"];
     weather_0_icon = weather_0["icon"];
     Icon = String(weather_0_icon);
     weatherr =String(weather_0_main);
     
     JsonObject main = doc["main"];
     main_temp = main["temp"];
     main_humidity = main["humidity"];  
     
     //Print Data 
     Serial.println("Current Weather");
     Serial.print(weather_0_main);Serial.print(", Temp: ");
     Serial.print(main_temp);Serial.print(", Hum:");
     Serial.print(main_humidity); Serial.println();
//     Serial.println(hari);
//      Serial.print(", ");
      Serial.print(tanggal);
      Serial.print("/");
      Serial.print(bulan);
      Serial.print("/");
      Serial.print(tahun);
      Serial.print("\t");
      Serial.print(jam);
      Serial.print(":");
      Serial.print(menit);
      Serial.print(":");
      Serial.println(detik);
      delay(1000);
    }else{
         ConnectWIFI();
    }
} 


void DisplayUpdate(){
  int x_cursor;
  int TextSize;
  char buff[20];
  display.clearDisplay(); 
  checkWIFI();
   if (wifi_stat == 0){
      // if the LED is off turn it on and vice-versa:
      //Serial.println(F(" Wifi Not Connected"));
      display.setTextSize(1);
      display.setCursor(2,24); display.print("Connecting ...");
      if (wifiIcon == LOW) {
      display.drawBitmap(108, 2, wifi_icon, 16, 16, WHITE);
      wifiIcon = HIGH;
      } else {
      display.drawBitmap(108, 2, wifi_icon, 16, 16, BLACK);
      wifiIcon = LOW;
      }
    }else{
      float dataAnalog;
//      dataAnalog = dht.readTemperature();
      timeClient.update();
      display.drawBitmap(106, 2, wifi_icon, 16, 16, WHITE);
      //display variable
      display.setTextColor(WHITE);
      display.setTextSize(1);
      //display.setCursor(80,40);display.print(humNow); display.println(" %");
      display.setCursor(0,56);display.println(city);
      display.setCursor(64,30);display.println(weatherr);
      String cd = String(tanggal)+"/"+ String(bulan)+"/"+String(tahun)+" "+String(jam)+":"+String(menit);
      display.setCursor(0,2);display.println(cd);
      display.setTextSize(1);
      display.setCursor(64,19); display.print(main_temp);display.println(" C");
      display.setTextSize(2);
      display.setCursor(64,45); display.print(dataAnalog);display.println(" C");
      DisplayBitmap();
    }
    display.display();
    delay(600);
}
void DisplayBitmap(){
  if (Icon=="01d"){
    display.drawBitmap(0, 0, d01, 64, 64, WHITE);
  }
  else if (Icon=="01n"){
    display.drawBitmap(0, 0, n01, 64, 64, WHITE);
  }
  else if (Icon=="02n"){
    display.drawBitmap(0, 0, n02, 64, 64, WHITE);
  }
  else if (Icon=="02d"){
    display.drawBitmap(0, 0, d02, 64, 64, WHITE);
  }
  else if ((Icon=="03d")||(Icon=="03n")){
    display.drawBitmap(0, 0, Clouds, 64, 64, WHITE);
  }
  else if ((Icon=="04d")||(Icon=="04n")){
    display.drawBitmap(0, 0, Broken_clouds, 64, 64, WHITE);
  }
  else if ((Icon=="09d")||(Icon=="09n")){
    display.drawBitmap(0, 0,  Shower_rain, 64, 64, WHITE);
  }
  else if ((Icon=="10d")||(Icon=="10n")){
    display.drawBitmap(0, 0, Rain, 64, 64, WHITE);
  }
  else if ((Icon=="11d")||(Icon=="11n")){
    display.drawBitmap(0, 0, Thunderstorm, 64, 64, WHITE);
  }
  else if ((Icon=="13d")||(Icon=="13n")){
    display.drawBitmap(0, 0, Snow, 64, 64, WHITE);
  }
  else if ((Icon=="50d")||(Icon=="50n")){
    display.drawBitmap(0, 0, Mist, 64, 64, WHITE);
  }
  else{
    display.drawBitmap(0, 0, Unknows, 32, 32, WHITE);
  } 
}

void checkWIFI(){
  if(WiFi.status() == WL_CONNECTED){
    wifi_stat=1;
  }else{
    wifi_stat=0;
  }
}

void ConnectWIFI(){
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    int i=0;
    while(WiFi.status() != WL_CONNECTED){ 
      Serial.print(".");
      delay(1000); 
      ++i;
      if (i==30){
        i=0;
        Serial.println("\n Failed to Connect.");
        break;
        }    
    } 
    if (WiFi.status() == WL_CONNECTED){
      Serial.println("\nConnected.");
      wifi_stat=1;
    }
  }
}
void reconnect() {
  // Loop until we're reconnected
  Serial.println("In reconnect...");
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if(client.connect(CL, mqttUserName, mqttPassword)) {
      Serial.println("connected");
      Serial.println('\n');

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
