# Cambio de arma por IMU (reemplaza recarga)

Fecha: 2026-06-08
Estado: aprobado para implementacion

## Objetivo

El videojuego FPS no tiene recarga tradicional con boton, asi que la senal
`reload` (giro de brazo sobre umbral) deja de tener sentido. En su lugar, la
IMU controla el **cambio de arma** sobre la rueda de seleccion de 4 armas que
ya existe en Unity (`SelectWeaponScript`). Girar la muneca en una direccion
avanza un arma; girarla en la otra retrocede un arma.

El disparo sigue siendo por sensor de flexion. De los 4 botones, solo PB1
(salto) queda funcional; PB2/PB3/PB4 quedan reservados para acciones futuras.

## Arquitectura: la ESP32 es la fuente de verdad del arma

La ESP32 mantiene `weaponIndex` (0..3), aplica el gesto de roll y envia el
**indice absoluto** en cada trama (50 Hz). Unity compara con su arma actual y
cambia cuando difiere.

Se descarto enviar pulsos de direccion (+/-1 por un frame) porque a 50 Hz un
pulso de un solo frame puede perderse o duplicarse si el `Update` de Unity va
mas lento que el stream. El indice absoluto es auto-sincronizante y robusto
ante frames perdidos.

## Gesto en el firmware (roll de la IMU)

Logica "inclinar y volver al centro" con rearmado, analoga al gatillo por flex:

- Reposo (`|roll| < WEAPON_ROLL_REARM`) -> gesto **armado**.
- `roll >= +WEAPON_ROLL_THRESHOLD` y armado -> `weaponIndex` avanza un paso,
  se **desarma**.
- `roll <= -WEAPON_ROLL_THRESHOLD` y armado -> `weaponIndex` retrocede un paso,
  se **desarma**.
- Se rearmar solo cuando el roll vuelve a la banda central.

Resultado: un cambio por gesto, con **wrap** (de la ultima arma a la primera y
viceversa) sobre `WEAPON_COUNT = 4`. La direccion se invierte con la bandera
`WEAPON_ROLL_INVERT`. Umbral y banda de rearmado configurables en
`GlobalState.h`.

## Protocolo nuevo ESP32 -> Unity (10 campos, antes 11)

```
jump,shoot,weapon,vrx1,vry1,vrx2,vry2,flex,roll,pitch
  0    1     2      3    4    5    6    7    8    9
```

- Indice 2: pasa de `changeWeapon` (bool del boton PB2) a `weapon` (indice de
  arma 0..3 del gesto IMU).
- Se elimina el campo `reload` (antiguo indice 10).

Este cambio aprovecha para corregir el desfase de indices que tenia el parser
de Unity (leia los joysticks corridos un lugar y nunca consumia `shoot`).

## Cambios en el firmware

- `GlobalState.h`: quita `reload`, `RELOAD_ROLL_THRESHOLD`,
  `RELOAD_ROLL_HYSTERESIS`. Renombra `changeWeapon` -> `pb2Reserved`. Agrega
  `int weaponIndex`, `bool weaponGestureArmed`, consts `WEAPON_ROLL_THRESHOLD`,
  `WEAPON_ROLL_REARM`, `WEAPON_COUNT`.
- `HardwareConfig.h`: `RELOAD_WHEN_ABOVE` -> `WEAPON_ROLL_INVERT`. `PB2_PIN`
  deja de mapearse como `CHANGE_WEAPON_PIN`; PB2 queda como boton reservado.
- `IMU.ino`: `UpdateReloadFromRoll()` -> `UpdateWeaponFromRoll()`.
- `Inputs.ino`: lee PB2 en `pb2Reserved`.
- `ProtocolSend.ino`: nuevo formato CSV.
- `Debug.ino`: columna `RLD` -> `WP` (indice de arma); `W` -> `B2`.

## Cambios en el dashboard

- `DashboardPage.h`: tarjeta "PB2 - ARMA" -> "PB2 - reservado". Tarjeta
  "RECARGA" -> "ARMA" mostrando el indice (1..4) y el estado del gesto
  (armado / cambiando).
- `Dashboard.ino`: el JSON SSE manda `wp` (weaponIndex) y `wa`
  (weaponGestureArmed) en vez de `rl`; `w` pasa a ser el boton PB2 reservado.

## Cambios en Unity

- `WIFIConnectionScript.cs`: corrige indices (`J1X..J2Y` <- `[3..6]`), agrega
  `public static int Weapon` <- `[2]`, usa `CultureInfo.InvariantCulture` en
  los `TryParse` (evita fallo con configuracion regional de coma decimal), gate
  `Length >= 7`.
- `SelectWeaponScript.cs`: nuevo camino dirigido por IMU. En `Update`, si
  `WIFIConnectionScript.Weapon != weaponIndex` y no se esta usando la rueda con
  mouse, reusa el HUD/animacion existentes: muestra la rueda brevemente,
  resalta el arma nueva (`weaponsIndicators`), fija `weaponIndex` y ejecuta el
  cambio. El camino con mouse (`Fire3`) se conserva para pruebas en PC.
- Disparo por flex: se conecta `WIFIConnectionScript.Shoot` al disparo
  (`WeaponScript`), porque hoy el juego dispara con `Fire1` y el flex no estaba
  realmente conectado.

## Verificacion

- Firmware: compila con `arduino-cli compile --fqbn esp32:esp32:esp32`.
- Unity: los scripts compilan en el editor (sin test framework en el proyecto).
- Bench: girar el guante cambia el arma de a uno por gesto; doblar el flex
  dispara; el dashboard muestra el indice de arma correcto.

## Docs a sincronizar

`IDEAS.md` (seccion 5.1 protocolo, 4 decisiones de firmware, 8 estado) y
`CLAUDE.md` (descripcion del protocolo) en el mismo cambio.
