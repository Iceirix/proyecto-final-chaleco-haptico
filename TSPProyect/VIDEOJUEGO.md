# VIDEOJUEGO.md — Mapa del proyecto Unity (TSPProyect)

Guia de navegacion del videojuego para no tener que reescanear la carpeta en
cada sesion. El videojuego es el FPS de RV con el que dialoga el chaleco
haptico (ESP32). Si tocas el codigo, lee tambien `IDEAS.md` (raiz del repo)
para el protocolo y el pinout del firmware.

> Convencion: sin acentos para mantener consistencia con el resto de docs del repo.

## Datos clave

| Dato | Valor |
|------|-------|
| Motor | Unity **6000.3.11f1** (Unity 6) — ver `ProjectSettings/ProjectVersion.txt` |
| Render | Universal Render Pipeline (URP) 17.3.0 |
| Plataformas | PC (StandaloneWindows) y Android (hay BurstAotSettings para ambos) |
| IP/puerto del ESP32 | `192.175.5.1:80` (definido en `WIFIConnectionScript.cs`) |
| Soporte VR/XR | Modulos `com.unity.modules.vr` / `xr` presentes |

**Para abrir:** clonar, `git pull`, y abrir la carpeta `TSPProyect/` desde Unity Hub
con la version 6000.3.11f1. Unity reconstruye `Library/`, `obj/`, etc. en el primer
arranque (estan en `.gitignore`, no se versionan).

## Que se versiona y que no

Solo se sube lo que Unity necesita para reconstruir el proyecto:

- **Versionado:** `Assets/`, `Packages/`, `ProjectSettings/`, `.gitignore`, este doc.
- **Ignorado** (`.gitignore` de esta carpeta): `Library/`, `Temp/`, `obj/`, `Logs/`,
  `Builds/`, `.vs/`, `UserSettings/`, y los `*.sln`/`*.csproj` autogenerados. Ahi viven
  ademas los archivos >100 MB que GitHub no admite (p.ej. `Builds/.../resources.assets.resS` ~161 MB).

## El puente con el chaleco (LO MAS IMPORTANTE)

`Assets/WIFIPlugin/Scripts/WIFIConnectionScript.cs` es **el lado Unity del protocolo
del ESP32**. Resumen de lo que hace:

- Abre un `TcpClient` contra `192.175.5.1:80` en un hilo secundario (`secondaryThread`).
- Expone como `static` las entradas que manda el chaleco: `J1X J1Y J2X J2Y` (joysticks),
  `Jump`, `Shoot`, `CAMX CAMY`. Otros scripts las leen directo.
- Envia comandos al ESP con `SendData(...)` usando el formato `M1=..;M2=..;M3=..;M4=..`
  (motores del chaleco). Coincide con el protocolo Unity->ESP de `IDEAS.md` (claves
  `M1..M4 SOL HP DMG ELEC`).
- Evento `WhenReceiveDataCall` para distribuir la trama CSV que llega del ESP (50 Hz).

`Assets/1.- REBOOTVRFPSController/Scripts/REBOOTBTInputs.cs` es la version "Bluetooth/
gamepad Android" de las entradas (triggers `primaryTrigger`, ejes `x`/`y`). Convive con
el camino WiFi; util si se prueba con control Android en vez del chaleco.

> Si cambias el protocolo en el firmware (`ChalecoHaptico/`), este es el archivo Unity
> que hay que mantener sincronizado. Actualiza tambien `IDEAS.md`.

## Estructura de `Assets/`

Las carpetas estan numeradas. Marcadas como **[propio]** = codigo/contenido del equipo,
**[3ros]** = paquetes de Asset Store / demos que no se editan.

| Carpeta | Que es |
|---------|--------|
| `1.- REBOOTVRFPSController/` | **[propio]** Controlador FPS de RV. Scripts del jugador en `Scripts/` (ver abajo). Prefabs de UI (botones, transiciones). |
| `2.- Models/` | **[3ros]** Modelos 3D, texturas de terreno, props. |
| `3.- GameScenes/` | **[propio]** Escenas reales del juego: `1.- MenuScene.unity`, `2.- Level2Scene.unity`, `My scene.unity`. |
| `4.- Intro/` | Pantalla/secuencia de intro. |
| `5.- Skyboxes/` | **[3ros]** Skyboxes espaciales (Starfield, MilkyWay, GalaxyBox). Las texturas grandes (~52-55 MB) viven aqui. |
| `6.- EnemyBehavioursScripts/` | **[propio]** IA de enemigos (ver scripts abajo). |
| `7.- FXSounds/` | Efectos de sonido. |
| `8.- Music/` | Musica. |
| `9.- TextMesh Pro/` | **[3ros]** TMP (texto). |
| `10.- Fonts/` | Tipografias. |
| `11.- SpritesAndImages/` | Sprites y UI 2D. |
| `12.- Shaders/` | Shaders (incluye ToonyColorsPro). |
| `13.- ParticleSytems/` | **[3ros]** CartoonFX (CFXR) + Kino Bloom (post-proceso). No editar. |
| `14.- Test/` | **[propio]** Escenas y scripts de aprendizaje/pruebas (raycast, navmesh, colisiones). No es parte del juego final. |
| `15.- Animations/` | Animaciones. |
| `16.- MyFolder/` | Carpeta de trabajo varia. |
| `18.-Materials/` | Materiales. |
| `WIFIPlugin/` | **[propio]** El puente WiFi con el ESP32 (ver seccion anterior). |
| `Settings/` | Perfiles de URP / volumen. |
| `TutorialInfo/`, `Readme.asset` | Restos de la plantilla base de Unity. |
| `_Recovery/` | Escenas recuperadas por Unity (autoguardado). |
| `sphere.cs` (raiz de Assets) | Script suelto, probablemente prueba; revisar antes de borrar. |

## Scripts propios (los que se editan)

**Jugador / FPS** — `1.- REBOOTVRFPSController/Scripts/`:
- `FSPControllerScript.cs` — controlador principal del jugador.
- `VRMovementScript.cs`, `VRHeadScript.cs`, `VRDistortion.cs` — movimiento y camara VR.
- `WeaponScript.cs`, `SelectWeaponScript.cs`, `AMMOScript.cs` — armas y municion.
- `PlayersHealthScript.cs` — vida del jugador (se enlaza con barra NeoPixel via `HP`/`DMG`).
- `RecoverHealthScript.cs`, `RecoverAmmoScript.cs` — pickups.
- `REBOOTBTInputs.cs` — entradas Bluetooth/gamepad Android.
- Prefabs UI: `Other Prefabs/ButtonPrefab/Scripts/MenuScript.cs`,
  `Other Prefabs/CanvasTransitionPrefab/Scripts/LoadingImageScript.cs`.

**Enemigos** — `6.- EnemyBehavioursScripts/`:
- `CommonScripts/EnemyHealthScript.cs`, `InstantiateEnemiesScript.cs`.
- `IdleRunAndShootScript/` — enemigo que persigue y dispara.
- `RunAndAttackScripts/` — enemigo cuerpo a cuerpo (`HitPlayerScript.cs`, `RunAndAttackScript.cs`).

**Red** — `WIFIPlugin/Scripts/WIFIConnectionScript.cs` (el puente con el chaleco).

> Los scripts bajo `13.- ParticleSytems/` (CFXR_*, Bloom*) y los `examplescene.cs`,
> `MouseLook.cs`, `LookCamera.cs` de los skyboxes son de terceros: no se tocan.

## Escenas

- **Juego real:** `Assets/3.- GameScenes/` → `1.- MenuScene.unity`, `2.- Level2Scene.unity`.
- **Pruebas/aprendizaje:** `Assets/14.- Test/Scenes/` (no forman parte del build final).
- **Demos de terceros:** las `.unity` dentro de `5.- Skyboxes/`, `13.- ParticleSytems/`
  y `2.- Models/` son demos de los paquetes; ignorables.
