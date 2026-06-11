# Control de mano (rediseño ergonómico)

Rediseño del prototipo `controlabela.stl` (prisma 50×130×40). Empuñadura ergonómica
estilo *nunchuk* generada paramétricamente con `generar_control.py`
(Python: trimesh + manifold3d + scipy). Se imprimen **dos controles espejo**,
uno por mano, cada uno con su joystick, dos botones y una IMU.

## Archivos

| Archivo | Descripción |
|---|---|
| `control_derecho.stl` | Cuerpo del control (56 × 50 × 130 mm) |
| `control_izquierdo.stl` | Pieza idéntica a la anterior: el cuerpo es simétrico y así los jumpers quedan del lado derecho en ambos controles |
| `tapa_trasera.stl` | Tapa trasera desmontable (misma pieza para ambos controles, imprimir 2) |
| `generar_control.py` | Generador paramétrico; editar parámetros y volver a correr |
| `controlabela.stl` | Prototipo original (referencia, no usar) |

## Forma

Loft de superelipses (exponente 2.5, perfil redondeado-plano que asienta mejor en
la palma): base Ø~42, abultamiento de palma en z≈45, cuello para los dedos en
z≈72, y cabeza ensanchada arriba que aloja el joystick. La cara superior está
inclinada 8° hacia la palma para que el pulgar caiga natural sobre el stick.
Pared de 3 mm, piso de 5 mm.

## Alojamiento de componentes (medidas investigadas)

**Joystick KY-023** — PCB 34 × 26 mm, 4 barrenos Ø3.2 en retícula 26.7 × 20.3 mm,
header de 5 pines en el borde corto.
- Bolsillo abierto por arriba de 35 × 27 × 9 mm en la cara superior.
- 4 postes Ø7 × 4.5 mm con piloto Ø2.5: atornillar con **M3 × 6 mm** (×4).
- El PCB queda 4.5 mm bajo la cara; la palanca sobresale ~27 mm.
- Ranura de 6.5 × 20 mm en el borde del header para los **jumpers dupont**
  (llega solo hasta el piso del bolsillo para no perforar la pared exterior)
  + canal interno retraído hacia el centro que baja los cables a la cavidad.
- Ambos controles son la misma pieza, con la ranura del lado derecho.

**Botones 12 × 12 mm** (cuerpo 12 × 12 × 3, capuchón Ø13 × 6, altura total 12)
— dos en la cara frontal, z = 94 y 76, al alcance de índice y dedo medio.
- El capuchón pasa por un barreno **Ø13.6** en panel de 2 mm; el cuerpo 12 × 12
  no cabe por el barreno (diagonal 17 mm), el panel lo retiene por las esquinas.
- Detrás del panel: asiento de registro de 12.8 mm (3.5 mm de fondo) y luego
  una **cámara de 19 × 19 mm** abierta a la cavidad para meter el botón ya
  soldado con sus cables. Fijar con gota de silicón caliente.
- Soldar los cables a las patas laterales **antes** de insertar el botón.

**IMU GY-BMI160 (13 × 18 mm)** — charola con paredes de 1.8 mm en el piso de la
cavidad (hueco 19 × 14 × 4 mm, abierto hacia la puerta). El módulo se acuesta con
el eje largo en X; fijar con silicón o espuma contra la puerta. Mantener la misma
orientación en ambos controles para que roll/pitch del firmware coincidan.

**Tapa trasera desmontable** — toda la espalda del control (z = 8 a 100, del
lado de la palma) se separa como concha: la cavidad queda completamente
expuesta y los botones, la IMU y el cableado se colocan directo con las manos.
- Cierre de **gancho y tornillo** (estilo tapa de baterías): **nada entra a
  presión** — ganchos y guías llevan 0.5 mm de holgura y la tapa solo se
  sujeta al atornillar.
- Dos ganchos en el borde superior de la tapa que quedan detrás de la pared
  del cuerpo; el borde superior exterior lleva chaflán de 20° para poder
  rotar la tapa al cerrarla sin que pegue contra el marco.
- Dos guías laterales en el cuerpo (z 62–80) alinean la tapa cerca de los
  ganchos.
- Se fija abajo con **2 × M3 × 6 mm** en postes Ø7.2 con piloto Ø2.5 anclados
  al piso; la punta del poste asienta en un bolsillo plano de la cara
  interior de la tapa y la caja escariada Ø6.2 hunde la cabeza (~4.4 mm de
  rosca).
- **Montaje:** tapa inclinada ~15° (abajo hacia afuera), meter el borde
  superior y deslizarla hacia arriba hasta enganchar, rotar el fondo hasta
  cerrar y poner los 2 tornillos. Desmontaje a la inversa.
  (Trayectoria verificada por simulación: 0 mm³ de choque en todo el
  recorrido; sin tornillos los ganchos impiden jalarla recta.)

**Cable** — salida Ø8 en el piso, lado de la puerta.

## Lista de compras (fijación, para los DOS controles)

| Cant. | Componente | Uso |
|---|---|---|
| 12 | Tornillo milimétrico **M3 × 6 mm** (único tipo de tornillo en todo el diseño) | 4 por control para el joystick + 2 por control para la tapa trasera; piloto Ø2.5, la rosca se forma sola en el PLA |
| 2 | Joystick KY-023 | Uno por control |
| 4 | Push botón 12×12 mm con capuchón Ø13 | Dos por control |
| 2 | IMU GY-BMI160 | Una por control |
| 10 | Jumper dupont **hembra-hembra** (o hembra al largo que necesite el arnés) | 5 por joystick; entran por la ranura del header |
| 1 | Barra de silicón caliente | Fijar botones en su cámara y la IMU en su charola |
| 1 | Cinta de espuma doble cara (opcional) | Asentar la IMU en la charola sin vibración |

Notas:
- Todo el diseño usa **un solo tipo de tornillo: M3 × 6 mm milimétrico**. Los
  pilotos son Ø2.5: al atornillar en PLA el tornillo forma su propia rosca
  (apretar firme la primera vez, sin exceso para no barrer la rosca).
- Engrane real: joystick ~4.4 mm (6 menos el PCB de 1.6); tapa ~4 mm gracias a
  la caja escariada Ø6.2 × 1.6 donde se hunde la cabeza.
- El cable a la ESP32 sale por el barreno Ø8 del piso; usar el calibre que ya
  maneja el arnés del chaleco.

## Impresión sugerida

- Cuerpo: de pie (Z como se modeló), 0.2 mm, 3 perímetros, 15–20 % relleno.
  Soportes para el bolsillo del joystick y el techo de la cavidad.
- Tapa: de pie sobre su borde inferior o acostada con la cara exterior hacia
  arriba (con soportes por la curvatura).

## Regenerar

```ps1
pip install trimesh manifold3d numpy scipy
python Control\generar_control.py
```

Todos los parámetros (alturas de botones, holguras, espesores, perfil del
mango) están al inicio del script.
