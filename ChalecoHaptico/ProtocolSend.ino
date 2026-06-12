// ============================================================================
//  ProtocolSend.ino - Empaqueta y envia el estado a Unity.
//
//  CSV terminado en "\n":
//    jump,shoot,weapon,vrx1,vry1,vrx2,vry2,flex,roll,pitch,btn2
//
//  shoot ya viene calculado a partir del sensor de flexion.
//  weapon es el indice de arma 0..3, fuente de verdad en la ESP, calculado a
//  partir del gesto de pitch de la IMU (ver IMU.ino::UpdateWeaponFromPitch).
//  Unity cambia de arma cuando este indice difiere del suyo.
//  btn2 es el boton PB2 (recarga manual en Unity); va al FINAL para no correr
//  los indices 0..9 que Unity ya parsea por posicion (firmware viejo sigue ok).
// ============================================================================

void SendStateToUnity()
{
  char buf[160];
  snprintf(buf, sizeof(buf),
           "%d,%d,%d,%.3f,%.3f,%.3f,%.3f,%d,%.2f,%.2f,%d\n",
           jump ? 1 : 0,
           shoot ? 1 : 0,
           weaponIndex,
           vrx1, vry1, vrx2, vry2,
           flexValue,
           rollAngle, pitchAngle,
           pb2Reserved ? 1 : 0);

  if (client && client.connected())
  {
    client.print(buf);
  }
}
