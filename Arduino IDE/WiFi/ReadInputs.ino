void ReadInputs()
{
  jump = digitalRead(jumpPin);
  shoot = digitalRead(shootPin);
  changeWeapon = digitalRead(changeWeaponPin);
  
  vrx1 = analogRead(vrx1Pin);
  vry1 = analogRead(vry1Pin);
  vrx2 = analogRead(vrx2Pin);
  vry2 = analogRead(vry2Pin);
  flexValue = analogRead(flexValuePin);

  vrx1 = mapFloat(vrx1, 0 , 4095, 1, -1);
  vry1 = mapFloat(vry1, 0 , 4095, 1, -1);

  vrx2 = mapFloat(vrx2, 0 , 4095, 1, -1);
  vry2 = mapFloat(vry2, 0 , 4095, 1, -1);

  Calibrate();
}

void Calibrate()
{
  if( abs(vrx1) < threshold1)
  {
    vrx1 = 0;
  }

  if( abs(vry1) < threshold1)
  {
    vry1 = 0;
  }

  if( abs(vrx2) < threshold2)
  {
    vrx2 = 0;
  }

  if( abs(vry2) < threshold2)
  {
    vry2 = 0;
  }
}