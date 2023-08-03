#define SERVO_IS_ON true

#define CMD_PORT Serial

#if SERVO_IS_ON
#include <DynamixelWorkbench.h>
DynamixelWorkbench ax;

#define BAUDRATE    1000000 // Set 1Mbps baudrate
#define DEVICE_NAME ""      // OpenCR

#endif

volatile bool loop_play = false;
volatile bool thread_1_2_stop = false;

const uint8_t ax_id[2] = {1, 2};
int32_t ax_syncwrite_variable[2];
const uint8_t handler_index = 0;

#include <math.h>
#include <IMU.h>
cIMU IMU;

#include <RTOS.h>
osThreadId thread_id_loop;
osThreadId thread_id_IMU;
osThreadId thread_id_SW1;

double kp = .28;
double ki = .00000000000001;
double kd = 0;

int milliOld;
int milliNew;
int dt;

double rollTarget = 0;
double rollActual;
double rollError = 0;
double rollErrorOld;
double rollErrorChange;
double rollErrorSlope = 0;
double rollErrorArea = 0;
float rollServoVal = 0;
float rollServoVal_ax;

double pitchTarget = 0;
double pitchActual;
double pitchError = 0;
double pitchErrorOld;
double pitchErrorChange;
double pitchErrorSlope = 0;
double pitchErrorArea = 0;
float pitchServoVal = 0;
float pitchServoVal_ax;

#define delay_loop 50
unsigned long loop_last_timer = 0, imu_last_timer = 0;

void setup() {
  // put your setup code here, to run once:
  CMD_PORT.begin(115200);

  pinMode(34, INPUT); // SW1

  pinMode(BDPIN_BUZZER, OUTPUT); // Set BDPIN_BUZZER - pin 31 as an output

  // beep sound
  beep();

#if SERVO_IS_ON
  setup_dxl_servo();

  ax_syncwrite_variable[0] = (rollServoVal * 3.422222) + 512;
  ax_syncwrite_variable[1] = (pitchServoVal * 3.422222) + 512;
  const char *log;
  ax.syncWrite(handler_index, &ax_syncwrite_variable[0], &log);

#endif

  IMU.begin();

  /*
    CMD_PORT.println("ACC Cali Start");
    IMU.SEN.acc_cali_start();
    while ( IMU.SEN.acc_cali_get_done() == false )
    {
      IMU.update();
    }
    CMD_PORT.print("ACC Cali End ");

    delay(1000);
  */

  milliNew = millis();

  // define thread
  osThreadDef(THREAD_NAME_LOOP, Thread_Loop, osPriorityNormal, 0, 1024);
  osThreadDef(THREAD_NAME_IMU, Thread_IMU, osPriorityNormal, 0, 1024);
  osThreadDef(THREAD_NAME_SW1, Thread_SW1, osPriorityNormal, 0, 1024);

  // create thread
  thread_id_loop = osThreadCreate(osThread(THREAD_NAME_LOOP), NULL);
  thread_id_IMU = osThreadCreate(osThread(THREAD_NAME_IMU), NULL);
  thread_id_SW1 = osThreadCreate(osThread(THREAD_NAME_SW1), NULL);

  // start kernel
  osKernelStart();
}

void loop() {
  // put your main code here, to run repeatedly:

  unsigned long now = millis();
  if (now - loop_last_timer >= delay_loop)
  {
    loop_last_timer = now;

    rollActual = IMU.rpy[0];
    pitchActual = IMU.rpy[1];

    milliOld = milliNew;
    milliNew = millis();
    dt = milliNew - milliOld;

    rollErrorOld = rollError;
    rollError = rollTarget - rollActual;
    rollErrorChange = rollError - rollErrorOld;
    rollErrorSlope = rollErrorChange / dt;
    rollErrorArea = rollErrorArea + rollError * dt;

    pitchErrorOld = pitchError;
    pitchError = pitchTarget - pitchActual;
    pitchErrorChange = pitchError - pitchErrorOld;
    pitchErrorSlope = pitchErrorChange / dt;
    pitchErrorArea = pitchErrorArea + pitchError * dt;

    rollServoVal = rollServoVal + kp * rollError + kd * rollErrorSlope + ki * rollErrorArea;
    rollServoVal_ax = (rollServoVal * 3.422222) + 512;

    //  rollServo.write(rollServoVal);
    ax_syncwrite_variable[0] =  rollServoVal_ax;

    pitchServoVal = pitchServoVal + kp * pitchError + kd * pitchErrorSlope + ki * pitchErrorArea;
    pitchServoVal_ax = (pitchServoVal * 3.422222) + 512;

    //  pitchServo.write(pitchServoVal);
    ax_syncwrite_variable[1] =  pitchServoVal_ax;

#if SERVO_IS_ON
    const char *log;
    ax.syncWrite(handler_index, &ax_syncwrite_variable[0], &log);

#endif

    CMD_PORT.print(rollTarget);
    CMD_PORT.print(",");
    CMD_PORT.print(rollActual);
    CMD_PORT.print(",");
    CMD_PORT.print(pitchTarget);
    CMD_PORT.print(",");
    CMD_PORT.print(pitchActual);
    CMD_PORT.print(",");
    CMD_PORT.print(rollServoVal);
    CMD_PORT.print(",");
    CMD_PORT.print(pitchServoVal);
    CMD_PORT.print(",");
    CMD_PORT.print(rollServoVal_ax);
    CMD_PORT.print(",");
    CMD_PORT.println(pitchServoVal_ax);

  }
}
