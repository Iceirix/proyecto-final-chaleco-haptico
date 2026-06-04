// ============================================================================
//  ProtocolSend.ino - Empaqueta y envia el estado a Unity.
//
//  CSV terminado en "\n":
//    jump,shoot,changeWeapon,vrx1,vry1,vrx2,vry2,flex,roll,pitch,reload
//
//  shoot ya viene calculado a partir del sensor de flexion.
//  reload se calcula a partir del roll de la IMU (giro del brazo). Se anexa al
//  final para no mover los indices 0..9 que Unity ya parsea por posicion.
// ============================================================================

void SendStateToUnity()
{
  char buf[160];
  snprintf(buf, sizeof(buf),
           "%d,%d,%d,%.3f,%.3f,%.3f,%.3f,%d,%.2f,%.2f,%d\n",
           jump ? 1 : 0,
           shoot ? 1 : 0,
           changeWeapon ? 1 : 0,
           vrx1, vry1, vrx2, vry2,
           flexValue,
           rollAngle, pitchAngle,
           reload ? 1 : 0);

  if (client && client.connected())
  {
    client.print(buf);
  }
}
