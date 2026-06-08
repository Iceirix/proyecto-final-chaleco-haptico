// ============================================================================
//  Debug.ino - Impresion legible por monitor Serial.
// ============================================================================

static uint8_t debugRowsUntilHeader = 0;

void DebugPrintAll()
{
#if DEBUG_VERBOSE >= 1
  if (debugRowsUntilHeader == 0)
  {
    Serial.println();
    Serial.printf(" %1s %1s %2s %2s %2s | %6s %6s | %6s %6s | %4s %4s %4s | %7s %7s %3s | %3s %3s %3s %3s | %1s %1s %3s | %s\r\n",
                  "J",
                  "S",
                  "B2",
                  "B3",
                  "B4",
                  "J1X",
                  "J1Y",
                  "J2X",
                  "J2Y",
                  "FLEX",
                  "RST",
                  "TRG",
                  "ROLL",
                  "PITCH",
                  "WP",
                  "M1",
                  "M2",
                  "M3",
                  "M4",
                  "S",
                  "E",
                  "HP",
                  "WIFI");
    debugRowsUntilHeader = 20;
  }
  debugRowsUntilHeader--;

  Serial.printf(" %d %d %2d %2d %2d | %+6.2f %+6.2f | %+6.2f %+6.2f | %4d %4d %4d | %+7.2f %+7.2f %3d | %3d %3d %3d %3d | %d %d %3d | %s\r\n",
                jump ? 1 : 0,
                shoot ? 1 : 0,
                pb2Reserved ? 1 : 0,
                pb3Reserved ? 1 : 0,
                pb4Reserved ? 1 : 0,
                vrx1,
                vry1,
                vrx2,
                vry2,
                flexValue,
                flexRestValue,
                flexTriggerThreshold,
                rollAngle,
                pitchAngle,
                weaponIndex,
                motorTarget[0],
                motorTarget[1],
                motorTarget[2],
                motorTarget[3],
                solenoidActive ? 1 : 0,
                electrodeActive ? 1 : 0,
                healthPercent,
                (client && client.connected()) ? "OK" : "WAIT");

#if DEBUG_VERBOSE >= 2
  Serial.printf("           ACC %+7.3f %+7.3f %+7.3f | GYR %+7.2f %+7.2f %+7.2f\r\n",
                accX,
                accY,
                accZ,
                gyrX,
                gyrY,
                gyrZ);
#endif
#endif
}
