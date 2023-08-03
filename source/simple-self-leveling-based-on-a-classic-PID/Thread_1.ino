static void Thread_Loop(void const *argument)
{
  (void) argument;

  while (!loop_play);

  for (;;)
  {
    while (thread_1_2_stop);
    loop();
  }
}
