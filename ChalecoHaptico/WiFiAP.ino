// ============================================================================
//  WiFiAP.ino - Access Point + servidor TCP para Unity y captive portal.
//
//  El servidor TCP del puerto 80 acepta dos tipos de clientes:
//    - Unity: lineas tipo "M1=128;HP=80\n" (el primer byte util es letra y
//      el primer separador es '='). Se conservan los bytes ya leidos.
//    - Navegadores: HTTP, primer byte 'GET ' o similar (espacio antes de '=').
//      Se les manda un 302 a http://chaleco:81/ para abrir el dashboard.
//
//  Adicionalmente se levanta:
//    - DNSServer en :53 que responde "192.175.5.1" a cualquier nombre. Esto
//      hace que tipear "chaleco" en el navegador funcione aunque el celular
//      no soporte mDNS, y dispara el captive portal del SO al conectarse.
//    - mDNS responder, asi "chaleco.local" tambien resuelve en iOS/macOS/Win.
// ============================================================================

static void RedirectBrowserToDashboard(WiFiClient& c);
static bool LooksLikeHttp(const char* sniff, int n);
static void FeedSniffedByteToUnity(char c);

void ConfigureAP()
{
  IPAddress IP_AP(192, 175, 5, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IP_AP, IP_AP, subnet);
  WiFi.softAPsetHostname(DEVICE_HOSTNAME);
  WiFi.softAP(AP_SSID, AP_PASSWORD);

  server.begin();
  server.setNoDelay(true);

  // DNS captivo: "*" matchea cualquier nombre y lo resuelve a la IP del AP.
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", IP_AP);

  // mDNS para "chaleco.local". Publica los servicios visibles.
#if ENABLE_DASHBOARD
  if (MDNS.begin(DEVICE_HOSTNAME))
  {
    MDNS.addService("http", "tcp", DASHBOARD_PORT);
  }
#else
  MDNS.begin(DEVICE_HOSTNAME);
#endif

  Serial.println("Punto de acceso configurado");
  Serial.print("SSID: ");
  Serial.print(AP_SSID);
  Serial.print("  IP: ");
  Serial.println(IP_AP);
  Serial.println("Abrir el dashboard desde el celular:");
  Serial.println("  http://chaleco/         (captive DNS, cualquier ruta)");
  Serial.print  ("  http://chaleco.local:");
  Serial.print(DASHBOARD_PORT);
  Serial.println("/  (mDNS, iOS/macOS/Win)");
  Serial.print  ("  http://");
  Serial.print(IP_AP);
  Serial.print(":");
  Serial.print(DASHBOARD_PORT);
  Serial.println("/   (IP directa, siempre funciona)");
}

void TryAcceptClient()
{
  if (client && client.connected()) return;

  WiFiClient incoming = server.available();
  if (!incoming) return;

  // Lee unos pocos bytes para distinguir HTTP de Unity. El timeout es corto
  // pero suficiente: tanto navegadores como Unity envian datos de inmediato.
  char sniff[8];
  int n = 0;
  unsigned long start = millis();
  while (n < (int)sizeof(sniff) && incoming.connected() && (millis() - start) < 80)
  {
    if (incoming.available())
    {
      sniff[n++] = (char)incoming.read();
      if (sniff[n - 1] == '\n') break;
    }
    else
    {
      delay(1);
    }
  }

  if (LooksLikeHttp(sniff, n))
  {
    RedirectBrowserToDashboard(incoming);
    incoming.stop();
    return;
  }

  // Cliente Unity. Conserva los bytes ya consumidos: alimentalos al parser
  // como si acabaran de llegar (asi no se pierde el primer comando).
  client = incoming;
  for (int i = 0; i < n; i++) FeedSniffedByteToUnity(sniff[i]);
  Serial.println("Cliente Unity conectado");
}

// Heuristica: protocolo Unity siempre lleva '=' temprano (ej. "M1=128"),
// mientras HTTP siempre tiene un espacio temprano antes del '='. Cualquier
// '=' antes que ' ' es Unity; cualquier ' ' antes que '=' es HTTP.
static bool LooksLikeHttp(const char* sniff, int n)
{
  for (int i = 0; i < n; i++)
  {
    if (sniff[i] == '=') return false;
    if (sniff[i] == ' ') return true;
  }
  return false; // sin datos suficientes asumimos Unity (default seguro)
}

static void RedirectBrowserToDashboard(WiFiClient& c)
{
  // Drena lo que el navegador haya enviado para no dejar bytes a medias.
  unsigned long start = millis();
  while (c.connected() && (millis() - start) < 30)
  {
    if (c.available()) c.read(); else break;
  }

  char buf[256];
  int n = snprintf(buf, sizeof(buf),
    "HTTP/1.1 302 Found\r\n"
    "Location: http://%s:%u/\r\n"
    "Cache-Control: no-store\r\n"
    "Content-Length: 0\r\n"
    "Connection: close\r\n\r\n",
    DEVICE_HOSTNAME, (unsigned)DASHBOARD_PORT);
  c.write((const uint8_t*)buf, n);
  c.flush();
}

// Replica la logica de ReceiveCommandsFromUnity para un solo byte. Lo usamos
// para reinyectar los bytes que ya leimos al hacer sniff del protocolo.
static void FeedSniffedByteToUnity(char c)
{
  if (c == '\n' || c == '\r')
  {
    if (wifiRxBuffer.length() > 0)
    {
      ProcessRxLine(wifiRxBuffer);
      wifiRxBuffer = "";
    }
  }
  else
  {
    wifiRxBuffer += c;
    if (wifiRxBuffer.length() > 200) wifiRxBuffer = "";
  }
}
