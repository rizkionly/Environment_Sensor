//DHT (Sensor Suhu)
#define DHTPIN D5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//OLED (Output Monitor)
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//NTP (Real-time Jam)
#define tahun timeClient.getYear()
#define bulan timeClient.getMonth()
#define tanggal timeClient.getDate()
#define jam timeClient.getHours()
#define menit timeClient.getMinutes()
#define detik timeClient.getSeconds()
const long utcOffsetInSeconds = 25200; // Timezone rules for GMT+7 (Jakarta, Bangkok, Hanoi)
//#define hari daysOfTheWeek[timeClient.getDay()]
//char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

//Set Wifi
WiFiClient espClient;
WiFiClient httpClient;
const char* ssid = "TEKIDO";      
const char* password = "iotworkshop2021"; 
int wifi_stat = 0;
int wifiIcon = LOW; 
byte mac[6];
String MACAddress;

//Set RMQ
PubSubClient client(espClient);
const char* guid                  = "18b91fec-957b-464e-9f3e-f4f716cf2d67";
const char* mqttHost              = "iwkrmq.pptik.id"; // Deklarasi untuk link yang akan dituju
const char* mqttUserName          = "/trainerkit:trainerkit"; // Deklarasi untuk nama UserName DI RMQ /survey:survey
const char* mqttPassword          = "12345678"; // Deklarasi untuk Passwordnya di RMQ $surv3yy!
const char* mqttQueue             = "Environment"; // Deklarasi untuk nama Queue di RMQ
const char* CL = guid;

//Set HTTP
HTTPClient http;
String openWeatherMapApiKey = "cbd4f560b4b5c040e76e653cda335ce9";
String city = "Bandung";
String countryCode = "ID";
String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";
unsigned long previousMillis_1 = 0;  
const long interval_1 = 30000; //data tiap 30detik

//Icon OLED
String main_temp;
String weather;
String Icon;

//json
String jsonBuffer;
