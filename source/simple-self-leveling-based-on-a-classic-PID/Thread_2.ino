static void Thread_IMU(void const *argument)
{
  (void) argument;

  IMU.update();
  while (!(IMU.rpy[0] < 1 && IMU.rpy[0] > - 1))
  {
    IMU.update();
    Serial.println("Loop IMU update");
  }
  while (!(IMU.rpy[1] < 1 && IMU.rpy[1] > - 1))
  {
    IMU.update();
    Serial.println("Loop IMU update");
  }
  Serial.println("Loop IMU update done");

  loop_play = true;

  // beep sound
  beep();

  for (;;)
  {
    while (thread_1_2_stop);

    unsigned long now = millis();
    if (now - imu_last_timer >= 5)
    {
      imu_last_timer = now;
      IMU.update();
      /*
            Serial.print("Roll: ");
            Serial.print(IMU.rpy[0]);
            Serial.print(", Pitch: ");
            Serial.print(IMU.rpy[1]);
            Serial.print(", Yaw: ");
            Serial.print(IMU.rpy[2]);
      */
    }
  }
}
