// ============================================================================
//  Dashboard.ino - Servidor HTTP + Server-Sent Events para dashboard web.
//
//  Sirve una sola pagina HTML autocontenida (DashboardPage.h) y, cuando el
//  navegador abre EventSource('/stream'), empieza a empujar telemetria en
//  formato JSON una vez cada DASHBOARD_PERIOD_MS. Los datos se leen de las
//  mismas variables globales que ya alimentan a Unity, asi que el dashboard
//  no duplica trabajo ni interfiere con el envio TCP del videojuego.
//
//  Acceso desde el navegador del telefono o PC conectado al AP del ESP:
//     http://192.175.5.1:81/
//
//  El puerto 80 atiende a Unity (y redirige navegadores al :81 aunque el juego
//  ya este conectado). El puerto 81 sirve el dashboard. Hasta
//  DASHBOARD_MAX_STREAMS dispositivos pueden ver telemetria a la vez: cada uno
//  ocupa un slot en dashboardClients[] y recibe la misma trama SSE.
// ============================================================================

#if ENABLE_DASHBOARD

#include "DashboardPage.h"

static void HandleDashboardRequest();
static void ServeHtmlPage();
static void StartSseStream(WiFiClient& c);
static void HandleCommandRequest();
static void Send404();
static void SendDashboardFrame();
static int DashboardStreamCount();
static String UrlDecode(const String& s);

void InitDashboard()
{
  dashboardServer.begin();
  dashboardServer.setNoDelay(true);
  Serial.print("Dashboard listo en http://");
  Serial.print(WiFi.softAPIP());
  Serial.print(":");
  Serial.print(DASHBOARD_PORT);
  Serial.println("/");
}

void UpdateDashboard()
{
  // Mantenimiento: libera los slots de streaming cuyo dispositivo cerro la
  // pestana, para que otro dispositivo pueda reusar el socket.
  for (int i = 0; i < DASHBOARD_MAX_STREAMS; i++)
  {
    if (dashboardClients[i] && !dashboardClients[i].connected())
    {
      dashboardClients[i].stop();
    }
  }

  // Acepta una peticion HTTP corta en un cliente transitorio aparte, para no
  // bloquear los streams SSE persistentes.
  if (!dashboardReqClient || !dashboardReqClient.connected())
  {
    WiFiClient incoming = dashboardServer.available();
    if (incoming)
    {
      dashboardReqClient = incoming;
      dashboardReqClient.setNoDelay(true);
      dashboardRxBuffer = "";
      dashboardMethod = "";
      dashboardPath = "";
    }
  }

  // Lee la peticion del cliente transitorio hasta la linea en blanco.
  if (dashboardReqClient && dashboardReqClient.connected())
  {
    while (dashboardReqClient.available())
    {
      char c = dashboardReqClient.read();
      if (c == '\r') continue;
      if (c == '\n')
      {
        if (dashboardRxBuffer.length() == 0)
        {
          // Linea en blanco -> fin de cabeceras, ya tenemos la ruta.
          HandleDashboardRequest();
          break;
        }
        if (dashboardMethod.length() == 0)
        {
          int sp1 = dashboardRxBuffer.indexOf(' ');
          int sp2 = dashboardRxBuffer.indexOf(' ', sp1 + 1);
          if (sp1 > 0 && sp2 > sp1)
          {
            dashboardMethod = dashboardRxBuffer.substring(0, sp1);
            dashboardPath = dashboardRxBuffer.substring(sp1 + 1, sp2);
          }
        }
        dashboardRxBuffer = "";
      }
      else
      {
        dashboardRxBuffer += c;
        if (dashboardRxBuffer.length() > 256) dashboardRxBuffer = "";
      }
    }
  }

  // Modo streaming: empuja una trama JSON a TODOS los dispositivos conectados
  // cada DASHBOARD_PERIOD_MS. Se construye una sola vez y se reparte.
  if (DashboardStreamCount() > 0)
  {
    unsigned long now = millis();
    if (now - dashboardLastMs >= DASHBOARD_PERIOD_MS)
    {
      dashboardLastMs = now;
      SendDashboardFrame();
    }
  }
}

// Cantidad de dispositivos con el stream SSE abierto en este momento.
static int DashboardStreamCount()
{
  int count = 0;
  for (int i = 0; i < DASHBOARD_MAX_STREAMS; i++)
  {
    if (dashboardClients[i] && dashboardClients[i].connected()) count++;
  }
  return count;
}

static void HandleDashboardRequest()
{
  if (dashboardPath == "/" || dashboardPath == "/index.html")
  {
    ServeHtmlPage();
    dashboardReqClient.stop();
  }
  else if (dashboardPath == "/stream")
  {
    // Busca un slot de streaming libre para este dispositivo. Asi varios
    // celulares/PCs pueden ver el dashboard a la vez, cada uno con su socket.
    int slot = -1;
    for (int i = 0; i < DASHBOARD_MAX_STREAMS; i++)
    {
      if (!dashboardClients[i] || !dashboardClients[i].connected()) { slot = i; break; }
    }
    if (slot < 0)
    {
      // Todos los slots ocupados: desaloja el mas antiguo (slot 0) para el nuevo.
      dashboardClients[0].stop();
      slot = 0;
    }
    dashboardClients[slot] = dashboardReqClient;
    dashboardReqClient = WiFiClient(); // libera el slot transitorio sin cerrar el socket
    StartSseStream(dashboardClients[slot]);
  }
  else if (dashboardPath.startsWith("/cmd"))
  {
    HandleCommandRequest();
    dashboardReqClient.stop();
  }
  else
  {
    Send404();
    dashboardReqClient.stop();
  }
}

static void ServeHtmlPage()
{
  const size_t pageLen = sizeof(DASHBOARD_HTML) - 1; // sin el nulo final

  char header[160];
  int hn = snprintf(header, sizeof(header),
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Content-Length: %u\r\n"
    "Cache-Control: no-store\r\n"
    "Connection: close\r\n"
    "\r\n",
    (unsigned)pageLen);
  dashboardReqClient.write((const uint8_t*)header, hn);

  // Envia el cuerpo en bloques para no acaparar la pila TCP.
  const size_t CHUNK = 512;
  size_t sent = 0;
  while (sent < pageLen && dashboardReqClient.connected())
  {
    size_t n = pageLen - sent;
    if (n > CHUNK) n = CHUNK;
    dashboardReqClient.write((const uint8_t*)(DASHBOARD_HTML + sent), n);
    sent += n;
    yield(); // permite que la red drene
  }
}

static void StartSseStream(WiFiClient& c)
{
  const char* hdr =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/event-stream\r\n"
    "Cache-Control: no-store\r\n"
    "Connection: keep-alive\r\n"
    "X-Accel-Buffering: no\r\n"
    "\r\n"
    ": ok\n\n";
  c.write((const uint8_t*)hdr, strlen(hdr));
  dashboardLastMs = 0; // fuerza envio inmediato a todos en el siguiente tick
}

static void Send404()
{
  const char* msg =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Length: 0\r\n"
    "Connection: close\r\n\r\n";
  dashboardReqClient.write((const uint8_t*)msg, strlen(msg));
}

// Decodifica un valor de query string (%XX y '+'). Suficiente para los comandos
// que el dashboard manda con encodeURIComponent (=,;,espacio).
static String UrlDecode(const String& s)
{
  String out;
  out.reserve(s.length());
  for (int i = 0; i < (int)s.length(); i++)
  {
    char c = s[i];
    if (c == '+')
    {
      out += ' ';
    }
    else if (c == '%' && i + 2 < (int)s.length())
    {
      auto hex = [](char h) -> int {
        if (h >= '0' && h <= '9') return h - '0';
        if (h >= 'a' && h <= 'f') return h - 'a' + 10;
        if (h >= 'A' && h <= 'F') return h - 'A' + 10;
        return -1;
      };
      int hi = hex(s[i + 1]);
      int lo = hex(s[i + 2]);
      if (hi >= 0 && lo >= 0) { out += (char)((hi << 4) | lo); i += 2; }
      else out += c;
    }
    else
    {
      out += c;
    }
  }
  return out;
}

// Atiende GET /cmd?c=<comando>. El comando reusa exactamente el mismo parser
// que Unity/Serial (ProcessRxLine en ProtocolReceive.ino), asi que acepta el
// formato "M1=128;HP=80;SOL=1;DMG=2;ELEC=0".
static void HandleCommandRequest()
{
  String cmd = "";
  int q = dashboardPath.indexOf('?');
  if (q >= 0)
  {
    String query = dashboardPath.substring(q + 1);
    int start = 0;
    while (start < (int)query.length())
    {
      int amp = query.indexOf('&', start);
      if (amp < 0) amp = query.length();
      String pair = query.substring(start, amp);
      int eq = pair.indexOf('=');
      if (eq > 0 && pair.substring(0, eq) == "c")
      {
        cmd = UrlDecode(pair.substring(eq + 1));
        break;
      }
      start = amp + 1;
    }
  }

  if (cmd.length() > 0)
  {
    ProcessRxLine(cmd);
    Serial.print("CMD WEB: ");
    Serial.println(cmd);
  }

  const char* ok =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 2\r\n"
    "Cache-Control: no-store\r\n"
    "Connection: close\r\n"
    "\r\n"
    "OK";
  dashboardReqClient.write((const uint8_t*)ok, strlen(ok));
}

static void SendDashboardFrame()
{
  // Numero de estaciones conectadas al softAP (telefono, PC, etc.).
  int wifiClients = (int)WiFi.softAPgetStationNum();

  // Captura la zona de dano pendiente y limpia la marca para que el navegador
  // solo vea un pulso por evento (sin perder sincronia con la barra de vida).
  int dmgZone = damageZoneTrigger;

  char buf[480];
  int n = snprintf(buf, sizeof(buf),
    "data: {"
      "\"j\":%d,\"s\":%d,\"w\":%d,\"b3\":%d,\"b4\":%d,"
      "\"jx1\":%.3f,\"jy1\":%.3f,\"jx2\":%.3f,\"jy2\":%.3f,"
      "\"fx\":%d,\"fxr\":%d,\"fxt\":%d,"
      "\"r\":%.2f,\"p\":%.2f,\"wp\":%d,\"wa\":%d,"
      "\"ax\":%.3f,\"ay\":%.3f,\"az\":%.3f,"
      "\"gx\":%.2f,\"gy\":%.2f,\"gz\":%.2f,"
      "\"m\":[%d,%d,%d,%d],"
      "\"sol\":%d,\"ele\":%d,\"hp\":%d,\"dz\":%d,"
      "\"u\":%d,\"wc\":%d,\"up\":%lu"
    "}\n\n",
    jump ? 1 : 0,
    shoot ? 1 : 0,
    pb2Reserved ? 1 : 0,
    pb3Reserved ? 1 : 0,
    pb4Reserved ? 1 : 0,
    vrx1, vry1, vrx2, vry2,
    flexValue, flexRestValue, flexTriggerThreshold,
    rollAngle, pitchAngle, weaponIndex, weaponGestureArmed ? 1 : 0,
    accX, accY, accZ,
    gyrX, gyrY, gyrZ,
    motorTarget[0], motorTarget[1], motorTarget[2], motorTarget[3],
    solenoidActive ? 1 : 0,
    electrodeActive ? 1 : 0,
    healthPercent,
    dmgZone,
    (client && client.connected()) ? 1 : 0,
    wifiClients,
    (unsigned long)(millis() / 1000UL));

  if (n > 0 && n < (int)sizeof(buf))
  {
    // Reparte la misma trama a cada dispositivo con el stream abierto.
    for (int i = 0; i < DASHBOARD_MAX_STREAMS; i++)
    {
      if (dashboardClients[i] && dashboardClients[i].connected())
      {
        dashboardClients[i].write((const uint8_t*)buf, n);
      }
    }
  }
}

#else  // ENABLE_DASHBOARD == 0

// Stubs vacios para que ChalecoHaptico.ino enlace siempre, aunque el dashboard
// este deshabilitado en HardwareConfig.h.
void InitDashboard() {}
void UpdateDashboard() {}

#endif
