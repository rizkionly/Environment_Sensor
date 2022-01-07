#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <DHT.h>

#include "icon.h"
#include "config.h"

WiFiUDP ntpUDP;
WiFiClient espClient;
WiFiClient httpClient;
HTTPClient http;
PubSubClient client(espClient);
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup () {
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); 
  }
  delay(1000);
  timeClient.begin();
  dht.begin();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(24,20);  display.println(F("Welcome"));
  display.setTextSize(1);
  display.setCursor(10,40);  display.println(F("Environment Sensor"));
  display.display();
  delay(1000); 
  display.clearDisplay();
  WiFi.mode(WIFI_STA); 
  setup_wifi();
  printMACAddress();
  client.setServer(mqttHost, 1883);
  delay(1000);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  kirimRMQ();
  delay(1000);
  unsigned long currentMillis_1 = millis();
  DisplayUpdate();
  if (currentMillis_1 - previousMillis_1 >= interval_1) {
  previousMillis_1 = currentMillis_1; 
  req_data();
  } 
}

void req_data(){
  if (WiFi.status() == WL_CONNECTED) {
    String jsonBuffer;
    jsonBuffer = httpGETRequest(serverPath.c_str());
    JSONVar myObject = JSON.parse(jsonBuffer);
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }
    main_temp = JSON.stringify(myObject["main"]["temp"]);
    String weather1 = JSON.stringify(myObject["weather"][0]["main"]);
    weather = weather1.substring(1, weather1.length()-1);
    String Icon1 = JSON.stringify(myObject["weather"][0]["icon"]);
    Icon = Icon1.substring(1, Icon1.length()-1);
  }else{
    setup_wifi();
  }
} 

void DisplayUpdate(){
  display.clearDisplay(); 
   if (WiFi.status() != WL_CONNECTED){
      display.drawBitmap(112, 1, wifi_icon, 16, 16, BLACK);
    }else{
      float dataAnalog;
      dataAnalog = dht.readTemperature();
      timeClient.update();
      display.drawBitmap(112, 1, wifi_icon, 16, 16, WHITE);
      display.setTextColor(WHITE);
      display.setTextSize(1);
      //display.setCursor(80,40);display.print(humNow); display.println(" %");
      display.setCursor(0,56);display.println(city);
      display.setCursor(64,30);display.println(weather);
      String cd = String(tanggal)+"/"+ String(bulan)+"/"+String(tahun)+" "+String(jam)+":"+String(menit);
      display.setCursor(0,2);display.println(cd);
      display.setTextSize(1);
      display.setCursor(64,19); display.print(main_temp);display.println("'C");
      display.setTextSize(2);
      display.setCursor(62,45); display.print(dataAnalog,1);display.setCursor(116,45);display.println("C");
      display.setTextSize(1);display.setCursor(110,42);display.println("o");
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

void reconnect() {
  // Loop until we're reconnected
  Serial.println("In reconnect...");
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if(client.connect(CL, mqttUserName, mqttPassword)) {
      Serial.println("connected");
      display.clearDisplay();
      req_data();
      Serial.println('\n');

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(2,24); display.print("Connecting to RMQ...");
      display.display();
      delay(5000);
    }
  }
}

String httpGETRequest(const char* serverName) {
  http.begin(httpClient, serverName);
  int httpResponseCode = http.GET();
  String payload = "{}";  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  http.end();
  return payload;
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(2,24); display.print("Connecting ...");
    if (wifiIcon == LOW) {
    display.drawBitmap(108, 2, wifi_icon, 16, 16, WHITE);
    wifiIcon = HIGH;
    } else {
    display.drawBitmap(108, 2, wifi_icon, 16, 16, BLACK);
    wifiIcon = LOW;
    }
    display.display();
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  display.clearDisplay();
}

String mac2String(byte ar[]) {
  String s;
  for (byte i = 0; i < 6; ++i)
  {
    char buf[3];
    sprintf(buf, "%2X", ar[i]);
    s += buf;
    if (i < 5) s += ':';
  }
  return s;
}

void printMACAddress() {
  WiFi.macAddress(mac);
  MACAddress = mac2String(mac);
  Serial.println(MACAddress);
}

void kirimRMQ() {
   float dataSuhu;
   dataSuhu = dht.readTemperature();
   Serial.println(dataSuhu);
   String convertdatasuhu = String(dataSuhu);
   String convertguid = String(guid);
   String dataRMQ = convertguid + "#" + convertdatasuhu;
   char dataToMQTT[50];
   dataRMQ.toCharArray(dataToMQTT, sizeof(dataToMQTT)); 
   Serial.println("Ini Data untuk ke MQTT: ");
   Serial.println(dataToMQTT);
   client.publish(mqttQueue,dataToMQTT);
}
