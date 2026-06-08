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

// 4 zonas del chaleco: PCB MOT2..MOT5.
#define MOTOR1_PIN 17
#define MOTOR2_PIN 25
#define MOTOR3_PIN 26
#define MOTOR4_PIN 27

// Salidas disponibles pero no usadas por defecto.
// MOT8 (GPIO19) ahora se usa como solenoide; GPIO0 queda libre (problemas de boot).
#define MOTOR_OPTION_MOT1_PIN 12
#define MOTOR_OPTION_MOT7_PIN 18

// Actuadores.
// Solenoide reasignado a MOT8 (GPIO19) — GPIO0 daba problemas (pin de boot).
#define SOLENOID_PIN 19
#define ELECTRODE_RELAY_PIN 4
#define ENABLE_ELECTRODE_RELAY 1

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

// Hostname amigable. Se anuncia via mDNS (chaleco.local) y un servidor DNS
// captivo embebido lo resuelve aunque el celular no soporte mDNS.
#define DEVICE_HOSTNAME "chaleco"

#endif
