#include <WiFi.h>

const char* ssid = "ESP32_Luis";
const char* password = "123456789";

WiFiServer server(80);
WiFiClient client;

bool jump;
bool shoot;
bool changeWeapon;

int flexValue;
int motor1Value;
int motor2Value;
int motor3Value;
int motor4Value;
int relayValue;


float vrx1;
float vry1;
float vrx2;
float vry2;
float threshold1 = 0.2;
float threshold2 = 0.2;

int jumpPin = 4; //Brinco
int shootPin = 5; //Dispara
int changeWeaponPin = 13; //Cambio de arma
int vrx1Pin = 33;
int vry1Pin = 34;
int vrx2Pin = 35;
int vry2Pin = 36;
int flexValuePin = 39; // Sensor de flexión
int motor1Pin = 16;
int motor2Pin = 17;
int motor3Pin = 18;
int motor4Pin = 19;
int relayPin = 23;

char input;
String chain;

void setup()
{
  Serial.begin(115200);
  ConfigureAP();
  pinMode(relayPin,OUTPUT);
}

void loop()
{
  WaitConnection();
  // No se ejecuta el código de abajo hasta que se conecta un cliente
  ReadInputs();
  SendData();
  ReceiveDataWiFi();
  CreateOutputs();
  delay(80);
}

float mapFloat(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
