#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define WIFI_AP "YOUR_WIFI_AP"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define TOKEN "ESP8266_DEMO_TOKEN"
char thingsboardServer[] = "YOUR_THINGSBOARD_HOST_OR_IP";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;
unsigned long lastSend;

Adafruit_BMP085 bmp;
  
void setup() {
  Serial.begin(115200);
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  lastSend = 0;

  if (!bmp.begin()) {
	Serial.println("Could not find a valid BMP085 sensor, check wiring!");
	while (1) {}
  }
}
  
void loop() {
    

  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    lastSend = millis();
  }

  client.loop();
}








void get_data_send_data(){

    temperatura  = bmp.readTemperature();
    pressao      = bmp.readPressure();
    altitude     = bmp.readAltitude();
    pressao_calc = bmp.readSealevelPressure();
    altitude_2   = bmp.readAltitude(101500);
    
    String Temperatura  = String(temperatura);
    String Pressao      = String(pressao);
    String Altitude     = String(altitude);
    String Pressao_calc = String(pressao_calc);
    String Altitude_2   = String(altitude_2);
    // json data


 String payload = "{";
  payload += "\"Temperatura\":"; payload += Temperatura; payload += ",";
  payload += "\"Pressao\":"; payload += Pressao; payload += ",";
  payload += "\"Altitude\":"; payload += Altitude; payload += ",";
  payload += "\"Pressao_calc\":"; payload += Pressao_calc; payload += ",";
  payload += "\"Altitude_2\":"; payload += Altitude;
  payload += "}";


  char attributes[200];
  payload.toCharArray( attributes, 200 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );
}




void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}




