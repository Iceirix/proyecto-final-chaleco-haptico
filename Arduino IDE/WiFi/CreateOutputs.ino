void CreateOutputs()
{
  analogWrite(motor1Pin, motor1Value);
  analogWrite(motor2Pin, motor2Value);
  analogWrite(motor3Pin, motor3Value);
  analogWrite(motor4Pin, motor4Value);
  digitalWrite(relayPin, relayValue);
}