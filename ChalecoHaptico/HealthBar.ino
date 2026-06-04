// ============================================================================
//  HealthBar.ino — Barra de vida con la tira NeoPixel del chaleco
//
//  8 LEDs WS2812 representan la vida del jugador (0..100%).
//  - Cada LED encendido vale 12.5%.
//  - El color de la barra cambia segun el porcentaje:
//        > 60%  -> verde
//        25-60% -> amarillo
//        < 25%  -> rojo
//  - Cuando llega un toque de daño (DMG), se hace un parpadeo rojo de toda la
//    tira durante el patron del motor correspondiente.
// ============================================================================

Adafruit_NeoPixel strip(NEOPIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Parpadeo de daño (no-bloqueante)
const unsigned long FLASH_TOTAL_MS = 250;
bool flashing = false;
unsigned long flashEndMs = 0;

// Cache para evitar redibujar cada ciclo
int lastDrawnHealth = -1;

// Refresco periodico aunque el estado no cambie. Sirve para:
//  - ver actividad continua en osciloscopio/analizador logico al depurar
//  - corregir LEDs corrompidos por glitches electricos (cable largo, ruido)
const unsigned long HEALTHBAR_REFRESH_MS = 100;
unsigned long lastRefreshMs = 0;

void InitHealthBar()
{
  strip.begin();
  strip.setBrightness(80); // 0..255, evita consumo excesivo
  strip.clear();
  strip.show();
  lastDrawnHealth = -1; // forzar primer dibujo
}

uint32_t HealthColor(int hp)
{
  if (hp > 60) return strip.Color(0, 200, 0);      // verde
  if (hp > 25) return strip.Color(220, 180, 0);    // amarillo
  return strip.Color(220, 0, 0);                   // rojo
}

void DrawHealthBar()
{
  int hp = healthPercent;
  if (hp < 0)   hp = 0;
  if (hp > 100) hp = 100;

  // Cuantos LEDs deben encenderse (redondeo al alza para que >0% encienda 1)
  int litLeds = (hp * NEOPIXEL_COUNT + 99) / 100;
  uint32_t color = HealthColor(hp);

  for (int i = 0; i < NEOPIXEL_COUNT; i++)
  {
    strip.setPixelColor(i, i < litLeds ? color : 0);
  }
  strip.show();
  lastDrawnHealth = hp;
}

void DrawDamageFlash()
{
  uint32_t red = strip.Color(255, 0, 0);
  for (int i = 0; i < NEOPIXEL_COUNT; i++)
  {
    strip.setPixelColor(i, red);
  }
  strip.show();
}

void UpdateHealthBar()
{
  unsigned long now = millis();

  // Si llego un trigger de daño, arrancamos el parpadeo (sin consumir el flag,
  // que ya lo consume VibrationMotors.ino; aqui solo observamos la solicitud
  // espejo via healthPercent recien actualizado u otro mecanismo).
  // Para no depender del flag de daño que el modulo de motores ya consumio,
  // exponemos un trigger propio si Unity envio DMG en este ciclo:
  // (ProtocolReceive marca damageFlash=true, ver Globals/ProtocolReceive)
  if (damageFlashTrigger)
  {
    flashing   = true;
    flashEndMs = now + FLASH_TOTAL_MS;
    damageFlashTrigger = false;
    DrawDamageFlash();
    return;
  }

  if (flashing)
  {
    if ((long)(now - flashEndMs) >= 0)
    {
      flashing = false;
      lastDrawnHealth = -1; // forzar redibujo
    }
    else
    {
      return; // mantenemos el parpadeo en pantalla hasta que termine
    }
  }

  if (healthPercent != lastDrawnHealth ||
      (now - lastRefreshMs) >= HEALTHBAR_REFRESH_MS)
  {
    DrawHealthBar();
    lastRefreshMs = now;
  }
}
