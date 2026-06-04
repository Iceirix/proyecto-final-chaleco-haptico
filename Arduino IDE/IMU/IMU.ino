#include <Wire.h>

//-------------variables----------------//
int16_t ax, ay, az;
int16_t gx, gy, gz;
float radToDeg = 57.2957;
float xRotation, yRotation, zRotation;
float xGyr, yGyr, zGyr;
float xAccelerometer, yAccelerometer, zAccelerometer;
float xAccAngle, yAccAngle;
float xAccAngleFil, yAccAngleFil;
float pitchAngleFil, rollAngleFil; // cabeceo, inclinacion, no se puede medir guiñada (yaw)

//-------------tiempo----------------//
float dt;
float time1;
float time2;
float counter;

//-------------filtro----------------//
float alfa = 0.6; // factor para acelerometro
float beta = 0.8; // factor para filtro complementario

void setup()
{
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL
  InitializeAccelerometer();
  ConfiguraAccelerometer();
  InitializeGyroscope();
  ConfigureGyroscope();
  delay(100);
}

void loop()
{
  time1 = millis();
  ReadAccelerometer();
  ReadGyroscope();
  ComplementaryFilter();
  delay(1);
  time2 = millis();
  dt = (time2 - time1) / 1000;
  counter += dt;
  
  if(counter >= 0.006)
  {
    counter = 0;
    SendInfo();
  }
}
