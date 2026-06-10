#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

// PCB actual: ESP32-WROOM-32 DevKit de 38 pines.

// Botones activos en alto: boton a 3.3 V, resistor a GND.
#define PB1_PIN 13
#define PB2_PIN 14
#define PB3_PIN 15
#define PB4_PIN 16

#define JUMP_PIN PB1_PIN
// PB2/PB3/PB4 quedan reservados para acciones futuras. El cambio de arma ya no
// usa boton: lo controla el roll de la IMU (ver IMU.ino::UpdateWeaponFromRoll).

// Joysticks y flex.
#define JOY1_X_PIN 32
#define JOY1_Y_PIN 33
#define JOY2_X_PIN 34
#define JOY2_Y_PIN 35
#define FLEX_PIN   36

// I2C.
#define IMU_SDA_PIN 21
#define IMU_SCL_PIN 22

// 4 zonas del chaleco. Firmware M1..M4 -> pads de la PCB.
//   M1 = MOT2 (GPIO17)
//   M2 = MOT7 (GPIO18)  <- reubicado: el TIP120 del pad MOT3 (GPIO25) se quemo,
//                          asi que M2 se cablea al pad libre MOT7. Si reparas el
//                          TIP120, vuelve a poner MOTOR2_PIN en 25.
//   M3 = MOT4 (GPIO26)
//   M4 = MOT5 (GPIO27)
#define MOTOR1_PIN 17
#define MOTOR2_PIN 18
#define MOTOR3_PIN 26
#define MOTOR4_PIN 27

// Salidas disponibles pero no usadas por defecto.
// MOT8 (GPIO19) ahora se usa como solenoide; GPIO0 queda libre (problemas de boot).
// MOT7 (GPIO18) paso a usarse para M2 (ver arriba). MOT3 (GPIO25) queda libre
// pero con el TIP120 quemado: no usar hasta repararlo.
#define MOTOR_OPTION_MOT1_PIN 12
#define MOTOR_OPTION_MOT3_PIN 25

// Limite de voltaje de los motores vibradores.
// La bateria entrega mas voltaje del que aguantan los motores (3 V), asi que el
// PWM se recorta para que el promedio entregado nunca pase de MOTOR_RATED_VOLTAGE.
// El slider del dashboard y los comandos M1..M4 de Unity conservan rango completo
// 0..255 (0% = 0 V, 100% = MOTOR_RATED_VOLTAGE); el recorte ocurre en WriteMotor().
// Si tus celdas miden distinto (p.ej. salen a 4.0 V cargadas en vez de los 3.7 V
// nominales), cambia solo BATTERY_CELL_VOLTAGE y el limite se recalcula solo.
#define BATTERY_CELL_VOLTAGE 4.0f   // voltaje real medido por celda (nominal 3.7)
#define BATTERY_CELL_COUNT   2      // celdas de litio en serie
#define MOTOR_RATED_VOLTAGE  3.0f   // voltaje nominal maximo de los motores

// Voltaje total del pack (celdas en serie). Macro para que VibrationMotors.ino y
// Dashboard.ino lo vean sin depender del orden de concatenacion de los .ino.
#define BATTERY_PACK_VOLTAGE (BATTERY_CELL_VOLTAGE * BATTERY_CELL_COUNT)

// Actuadores.
// Solenoide reasignado a MOT8 (GPIO19) — GPIO0 daba problemas (pin de boot).
#define SOLENOID_PIN 19

// Celda Peltier (antes electrodos): se controla por un rele de 5 V via TIP120.
// Es un golpe termico temporizado por evento del juego. Una Peltier necesita
// segundos encendida para notarse, asi que el pulso es largo (no 80 ms como el
// viejo electrodo, que por eso "no se activaba"). ENABLE_PELTIER bloquea la
// salida fisica si se pone en 0. PELTIER_PULSE_MS = duracion del golpe.
#define PELTIER_RELAY_PIN 4
#define ENABLE_PELTIER 1
#define PELTIER_PULSE_MS 4000

// Barra de vida.
#define NEOPIXEL_PIN 2
#define NEOPIXEL_COUNT 8

// Flex: sensor tipo velostat (0 Ohm en reposo, 1-1000 Ohm al flexionar).
// Con el cableado actual la lectura ADC BAJA al doblar (extendido ~alto,
// flexionado ~bajo), asi que el gatillo dispara cuando cae bajo el umbral.
#define FLEX_TRIGGER_WHEN_BELOW 1

// Cambio de arma por giro del brazo: roll positivo avanza un arma y roll
// negativo retrocede. Pon esto en 1 si en el guante el gesto queda invertido
// (girar a la derecha deberia avanzar pero retrocede). Umbral y banda de
// rearmado se ajustan en GlobalState.h tras pruebas.
#define WEAPON_ROLL_INVERT 0

// Dashboard web embebido. Usa un puerto distinto al TCP de Unity para no
// interferir con la comunicacion del videojuego.
#define ENABLE_DASHBOARD 1
#define DASHBOARD_PORT 81
#define DASHBOARD_PERIOD_MS 40   // 25 Hz - mitad del rate de Unity
#define DASHBOARD_MAX_STREAMS 4  // dispositivos que pueden ver el dashboard a la vez (cada uno usa un socket)
#define DASHBOARD_MAX_REQ 2      // conexiones keep-alive simultaneas para pagina y /cmd (una por dispositivo activo)
#define DASHBOARD_REQ_IDLE_MS 2000      // cierra una conexion keep-alive sin actividad para liberar el slot
#define DASHBOARD_STREAM_MAX_STRIKES 25 // tramas SSE seguidas sin drenar (~1 s) antes de desalojar un cliente zombi

// Hostname amigable. Se anuncia via mDNS (chaleco.local) y un servidor DNS
// captivo embebido lo resuelve aunque el celular no soporte mDNS.
#define DEVICE_HOSTNAME "chaleco"

#endif
