void  ReceiveDataWiFi()
{
  while(client.available())
  {
    input = client.read();

    if(input != ',' && input != ';' && input != '/' && input != ')' && input != '(')
    {
      chain += input;
    }

    if(input == ',')
    {
      motor1Value = chain.toInt();
      chain = "";
    }

    if(input == ';')
    {
      motor2Value = chain.toInt();
      chain = "";
    }

    if(input == '/')
    {
      motor3Value = chain.toInt();
      chain = "";
    }

    if(input == ')')
    {
      motor4Value = chain.toInt();
      chain = "";
    }

    if(input == '(')
    {
      relayValue = chain.toInt();
      chain = "";
    }
  }
}