static void Thread_SW1(void const *argument)
{
  (void) argument;

  while (!loop_play);

  for (;;)
  {
    if (digitalRead(34) == HIGH)
    {
      // beep sound
      beep();
      delay(500);

      thread_1_2_stop = true;

      ax_syncwrite_variable[0] = (0 * 3.422222) + 512; // convert degree to ax input
      ax_syncwrite_variable[1] = (0 * 3.422222) + 512; // convert degree to ax input

#if SERVO_IS_ON
      const char *log;
      ax.syncWrite(handler_index, &ax_syncwrite_variable[0], &log);

#endif

      Serial.println("ACC Cali Start");
      IMU.SEN.acc_cali_start();
      while ( IMU.SEN.acc_cali_get_done() == false )
      {
        IMU.update();
      }
      Serial.print("ACC Cali End ");

      // delay(1000);

      thread_1_2_stop = false;

      // beep sound
      beep();
      delay(500);
    }
  }
}
