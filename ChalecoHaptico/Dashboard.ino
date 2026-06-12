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
//
//  Las peticiones cortas (pagina y /cmd) van por slots keep-alive
//  (dashboardReqClients[]): el navegador reusa UNA conexion para todos los
//  comandos de un arrastre de slider. No volver a "Connection: close" aqui —
//  abrir una conexion TCP por comando agota los PCB de lwIP (los cerrados
//  quedan en TIME_WAIT) y lwIP termina matando el stream SSE o el socket de
//  Unity. Un slot inactivo DASHBOARD_REQ_IDLE_MS se cierra para liberarlo.
// ============================================================================

#if ENABLE_DASHBOARD

#include "DashboardPage.h"

static void HandleDashboardRequest(int slot);
static void ServeHtmlPage(int slot);
static void StartSseStream(WiFiClient& c);
static void HandleCommandRequest(int slot);
static void Send404(int slot);
static void SendDashboardFrame();
static int DashboardStreamCount();
static void ResetReqSlotState(int slot);
static void ReleaseReqSlot(int slot);
static String UrlDecode(const String& s);

void InitDashboard()
{
  dashboardServer.begin();
  dashboardServer.setNoDelay(true);
  Serial.print("Dashboard v3 listo en http://");
  Serial.print(WiFi.softAPIP());
  Serial.print(":");
  Serial.print(DASHBOARD_PORT);
  Serial.println("/");
}

void UpdateDashboard()
{
  unsigned long now = millis();

  // Mantenimiento: libera los slots de streaming cuyo dispositivo cerro la
  // pestana, para que otro dispositivo pueda reusar el socket.
  for (int i = 0; i < DASHBOARD_MAX_STREAMS; i++)
  {
    if (dashboardClients[i] && !dashboardClients[i].connected())
    {
      dashboardClients[i].stop();
      dashboardStreamStrikes[i] = 0;
    }
  }

  // Mantenimiento de los slots keep-alive: cierra los desconectados y los que
  // llevan demasiado tiempo sin mandar nada (para que otro dispositivo entre).
  for (int i = 0; i < DASHBOARD_MAX_REQ; i++)
  {
    if (!dashboardReqClients[i]) continue;
    if (!dashboardReqClients[i].connected())
    {
      ReleaseReqSlot(i);
    }
    else if (now - dashboardReqLastMs[i] > DASHBOARD_REQ_IDLE_MS)
    {
      dashboardReqClients[i].stop();
      ReleaseReqSlot(i);
    }
  }

  // Acepta conexiones nuevas mientras haya slots libres.
  for (int i = 0; i < DASHBOARD_MAX_REQ; i++)
  {
    if (dashboardReqClients[i]) continue;
    WiFiClient incoming = dashboardServer.available();
    if (!incoming) break;
    dashboardReqClients[i] = incoming;
    dashboardReqClients[i].setNoDelay(true);
    ResetReqSlotState(i);
    dashboardReqLastMs[i] = now;
  }

  // Lee cada slot hasta la linea en blanco que cierra las cabeceras. Como la
  // conexion es persistente, despues de atender una peticion el mismo socket
  // puede traer la siguiente.
  for (int s = 0; s < DASHBOARD_MAX_REQ; s++)
  {
    if (!dashboardReqClients[s] || !dashboardReqClients[s].connected()) continue;

    while (dashboardReqClients[s].available())
    {
      char c = dashboardReqClients[s].read();
      dashboardReqLastMs[s] = now;
      if (c == '\r') continue;
      if (c == '\n')
      {
        if (dashboardRxBuffer[s].length() == 0)
        {
          // Linea en blanco -> fin de cabeceras, ya tenemos la ruta.
          HandleDashboardRequest(s);
          break;
        }
        if (dashboardMethod[s].length() == 0)
        {
          int sp1 = dashboardRxBuffer[s].indexOf(' ');
          int sp2 = dashboardRxBuffer[s].indexOf(' ', sp1 + 1);
          if (sp1 > 0 && sp2 > sp1)
          {
            dashboardMethod[s] = dashboardRxBuffer[s].substring(0, sp1);
            dashboardPath[s] = dashboardRxBuffer[s].substring(sp1 + 1, sp2);
          }
        }
        dashboardRxBuffer[s] = "";
      }
      else
      {
        dashboardRxBuffer[s] += c;
        if (dashboardRxBuffer[s].length() > 256) dashboardRxBuffer[s] = "";
      }
    }
  }

  // Modo streaming: empuja una trama JSON a TODOS los dispositivos conectados
  // cada DASHBOARD_PERIOD_MS. Se construye una sola vez y se reparte.
  if (DashboardStreamCount() > 0)
  {
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

// Limpia el estado de parseo de un slot (la conexion sigue viva).
static void ResetReqSlotState(int slot)
{
  dashboardRxBuffer[slot] = "";
  dashboardMethod[slot] = "";
  dashboardPath[slot] = "";
}

// Suelta el slot por completo (socket ya cerrado o transferido a streaming).
static void ReleaseReqSlot(int slot)
{
  dashboardReqClients[slot] = WiFiClient();
  ResetReqSlotState(slot);
}

static void HandleDashboardRequest(int slot)
{
  if (dashboardPath[slot] == "/" || dashboardPath[slot] == "/index.html")
  {
    ServeHtmlPage(slot);
    ResetReqSlotState(slot); // keep-alive: el socket queda esperando la siguiente
  }
  else if (dashboardPath[slot] == "/stream")
  {
    // Busca un slot de streaming libre para este dispositivo. Asi varios
    // celulares/PCs pueden ver el dashboard a la vez, cada uno con su socket.
    int free = -1;
    for (int i = 0; i < DASHBOARD_MAX_STREAMS; i++)
    {
      if (!dashboardClients[i] || !dashboardClients[i].connected()) { free = i; break; }
    }
    if (free < 0)
    {
      // Todos los slots ocupados: desaloja el mas antiguo (slot 0) para el nuevo.
      dashboardClients[0].stop();
      free = 0;
    }
    dashboardClients[free] = dashboardReqClients[slot];
    dashboardStreamStrikes[free] = 0;
    ReleaseReqSlot(slot); // libera el slot sin cerrar el socket (ya vive en streaming)
    StartSseStream(dashboardClients[free]);
  }
  else if (dashboardPath[slot].startsWith("/cmd"))
  {
    HandleCommandRequest(slot);
    ResetReqSlotState(slot); // keep-alive: el siguiente comando llega por aqui mismo
  }
  else
  {
    Send404(slot);
    dashboardReqClients[slot].stop();
    ReleaseReqSlot(slot);
  }
}

static void ServeHtmlPage(int slot)
{
  const size_t pageLen = sizeof(DASHBOARD_HTML) - 1; // sin el nulo final

  char header[160];
  int hn = snprintf(header, sizeof(header),
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"
    "Content-Length: %u\r\n"
    "Cache-Control: no-store\r\n"
    "Connection: keep-alive\r\n"
    "\r\n",
    (unsigned)pageLen);
  dashboardReqClients[slot].write((const uint8_t*)header, hn);

  // Envia el cuerpo en bloques para no acaparar la pila TCP.
  const size_t CHUNK = 512;
  size_t sent = 0;
  while (sent < pageLen && dashboardReqClients[slot].connected())
  {
    size_t n = pageLen - sent;
    if (n > CHUNK) n = CHUNK;
    dashboardReqClients[slot].write((const uint8_t*)(DASHBOARD_HTML + sent), n);
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

static void Send404(int slot)
{
  const char* msg =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Length: 0\r\n"
    "Connection: close\r\n\r\n";
  dashboardReqClients[slot].write((const uint8_t*)msg, strlen(msg));
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
// formato "M1=128;HP=80;SOL=1;DMG=2;PELT=0".
static void HandleCommandRequest(int slot)
{
  String cmd = "";
  int q = dashboardPath[slot].indexOf('?');
  if (q >= 0)
  {
    String query = dashboardPath[slot].substring(q + 1);
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
    "Connection: keep-alive\r\n"
    "\r\n"
    "OK";
  dashboardReqClients[slot].write((const uint8_t*)ok, strlen(ok));
}

static void SendDashboardFrame()
{
  // Numero de estaciones conectadas al softAP (telefono, PC, etc.).
  int wifiClients = (int)WiFi.softAPgetStationNum();

  // Zona de dano con un toque en curso: se mantiene mientras dura el patron
  // (~400 ms), asi el navegador alcanza a verla y dispara el flash una vez.
  // No se puede usar damageZoneMask: UpdateVibrationMotors ya lo limpio.
  int dmgZone = activeDamageZone;

  char buf[520];
  int n = snprintf(buf, sizeof(buf),
    "data: {"
      "\"j\":%d,\"s\":%d,\"w\":%d,\"b3\":%d,\"b4\":%d,"
      "\"jx1\":%.3f,\"jy1\":%.3f,\"jx2\":%.3f,\"jy2\":%.3f,"
      "\"fx\":%d,\"fxr\":%d,\"fxt\":%d,"
      "\"r\":%.2f,\"p\":%.2f,\"wp\":%d,\"wa\":%d,"
      "\"ax\":%.3f,\"ay\":%.3f,\"az\":%.3f,"
      "\"gx\":%.2f,\"gy\":%.2f,\"gz\":%.2f,"
      "\"m\":[%d,%d,%d,%d],\"vm\":%.2f,\"vb\":%.2f,"
      "\"sol\":%d,\"pel\":%d,\"hp\":%d,\"dz\":%d,"
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
    motorLevel[0], motorLevel[1], motorLevel[2], motorLevel[3],
    (float)MOTOR_RATED_VOLTAGE, (float)BATTERY_PACK_VOLTAGE,
    solenoidActive ? 1 : 0,
    peltierActive ? 1 : 0,
    healthPercent,
    dmgZone,
    (client && client.connected()) ? 1 : 0,
    wifiClients,
    (unsigned long)(millis() / 1000UL));

  if (n > 0 && n < (int)sizeof(buf))
  {
    // Reparte la misma trama a cada dispositivo con el stream abierto. Antes
    // de escribir se sondea el socket con select() de timeout cero: si el
    // buffer TCP de un cliente no drena (p.ej. quedo zombi tras recargar la
    // pagina en el celular), WiFiClient::write() reintentaria con esperas de
    // ~1 s y congelaria el loop completo (uptime detenido, 0 Hz, Unity sin
    // tramas). Al cliente atascado se le salta la trama y tras
    // DASHBOARD_STREAM_MAX_STRIKES seguidas se desaloja su slot.
    for (int i = 0; i < DASHBOARD_MAX_STREAMS; i++)
    {
      if (!dashboardClients[i] || !dashboardClients[i].connected()) continue;

      int sockfd = dashboardClients[i].fd();
      bool writable = false;
      if (sockfd >= 0)
      {
        fd_set wset;
        FD_ZERO(&wset);
        FD_SET(sockfd, &wset);
        struct timeval tv = {0, 0};
        writable = (select(sockfd + 1, NULL, &wset, NULL, &tv) > 0) && FD_ISSET(sockfd, &wset);
      }

      if (writable)
      {
        dashboardClients[i].write((const uint8_t*)buf, n);
        dashboardStreamStrikes[i] = 0;
      }
      else if (++dashboardStreamStrikes[i] >= DASHBOARD_STREAM_MAX_STRIKES)
      {
        dashboardClients[i].stop();
        dashboardStreamStrikes[i] = 0;
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
