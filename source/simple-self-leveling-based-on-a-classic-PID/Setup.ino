void beep()
{
  tone(BDPIN_BUZZER, 500); // Send 0.5KHz sound signal...
  delay(125);        // ...for 0.125 sec
  tone(BDPIN_BUZZER, 1000); // Send 1KHz sound signal...
  delay(125);        // ...for 0.125 sec
  tone(BDPIN_BUZZER, 1500); // Send 1.5KHz sound signal...
  delay(125);        // ...for 0.125 sec
  tone(BDPIN_BUZZER, 500); // Send 0.5KHz sound signal...
  delay(125);        // ...for 0.125 sec
  noTone(BDPIN_BUZZER);     // Stop sound...
}

#if SERVO_IS_ON
void setup_dxl_servo()
{
  const char *log;
  bool result = false;

  result = ax.init(DEVICE_NAME, BAUDRATE, &log);
  if (result == false)
  {
    Serial.println(log);
    Serial.println("\nFailed to init (DEVICE)");
  }
  else
    Serial.println("\nSucceed to init (DEVICE)");

  ping_servo(ax_id[0]);
  ping_servo(ax_id[1]);

  result = ax.addSyncWriteHandler(ax_id[0], "Goal_Position", &log);
  if (result == false)
  {
    Serial.println(log);
    Serial.println("\nFailed to add sync write handler - mx-ax");
  }
  else
    Serial.println("\nSucceed to add sync write handler - mx-ax");

  enable_led(ax_id[0], false);
  enable_led(ax_id[1], false);
}

void ping_servo(uint8_t DXL_ID)
{
  const char *log;

  uint16_t model_number = 0;
  bool result = false;

  result = ax.ping(DXL_ID, &model_number, &log);
  if (result == false)
  {
    Serial.println(log);
    Serial.print("\nFailed to ping, id: ");
    Serial.print(DXL_ID);
    Serial.println("\n");
  }
  else
  {
    Serial.println("\nSucceeded to ping");
    Serial.print("\nid : ");
    Serial.print(DXL_ID);
    Serial.print(" model_number : ");
    Serial.println(model_number);

    ax.itemWrite(DXL_ID, "LED", 1, &log);
    Serial.println("Dynamixel MX-AX has been successfully connected \n");

    ax.torqueOn(DXL_ID, &log);
  }
}

void enable_led(uint8_t myID, uint8_t myLED)
{
  const char *log;

  if (myLED >= 1) myLED = 1;
  ax.itemWrite(myID, "LED", myLED, &log);
  Serial.print("\nID: ");
  Serial.print(myID);
  Serial.print("LED AX successfully change (ON/OFF)\n");
}

#endif
