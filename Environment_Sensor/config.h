//Set Wifi
const char* ssid      = "TEKIDO"; //Deklarasi nama Wifi
const char* password  = "iotworkshop2021";  //Deklarasi password Wifi 
byte mac[6];
String MACAddress;

//Set RMQ
const char* guid                  = "18b91fec-957b-464e-9f3e-f4f716cf2d67"; //Deklarasi GUID di GUID Generator
const char* mqttHost              = "iwkrmq.pptik.id"; // Deklarasi untuk link yang akan dituju
const char* mqttUserName          = "/trainerkit:trainerkit"; // Deklarasi untuk nama UserName DI RMQ
const char* mqttPassword          = "12345678"; // Deklarasi untuk Passwordnya di RMQ
const char* mqttQueue             = "Environment"; // Deklarasi untuk nama Queue di RMQ
const char* CL = guid;

//DHT (Sensor Suhu)
#define DHTPIN D5
#define DHTTYPE DHT11

//OLED (Output Monitor)
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1 

//Set HTTP
String openWeatherMapApiKey = "cbd4f560b4b5c040e76e653cda335ce9";
String city = "Bandung";
String countryCode = "ID";
String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey + "&units=metric";
unsigned long previousMillis_1 = 0;  
const long interval_1 = 60000; //data tiap 1 menit

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

//Set OLED
String main_temp;
String weather;
String Icon;
int wifiIcon = LOW;
