void SendData()
{
  sendViaWifi();
  sendViaSerial();
}

void sendViaWifi()
{
  client.print(jump);
  client.print(",");
  client.print(shoot);
  client.print(",");
  client.print(changeWeapon);
  client.print(",");
  client.print(vrx1);
  client.print(",");
  client.print(vry1);
  client.print(",");
  client.print(vrx2);
  client.print(",");
  client.print(vry2);
  client.print(",");
  client.println(flexValue);
}

void sendViaSerial()
{
  Serial.print(jump);
  Serial.print(",");
  Serial.print(shoot);
  Serial.print(",");
  Serial.print(changeWeapon);
  Serial.print(",");
  Serial.print(vrx1);
  Serial.print(",");
  Serial.print(vry1);
  Serial.print(",");
  Serial.print(vrx2);
  Serial.print(",");
  Serial.print(vry2);
  Serial.print(",");
  Serial.println(flexValue);
}