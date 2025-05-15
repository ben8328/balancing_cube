#include "data_logging.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"
#include "uart.h"
#include "IMU.h"
#include "motor.h"



uint16_t logCount = 0; // Variable to store the number of samples logged
static osTimerId_t logTimerID; // Timer attributes for data logging
static osTimerAttr_t logTimerAttr = {
    .name = "LogTimer"
}; 

/* Variable declarations*/
static void (*log_function)(void); // Function pointer for logging function

/* Function declarations */
void        logging_init(void);
static void log_pointer(void *argument);
static void log_imu(void);




 static void log_pointer(void *argument)
 {
   UNUSED(argument);

   /* Call function pointed to by log_function*/
   (*log_function)();
 }



 void logging_init(void)
 {
    /* TODO: Initialise timer for use with pendulum data logging */
    logTimerID = osTimerNew(log_pointer, osTimerPeriodic, NULL, &logTimerAttr);

    //Error handling
    if (logTimerID == NULL) {
        printf("Error: Timer not created\n");
    }
   

 }

 void logging_stop(void)
 {
    /* TODO: Stop data logging timer */
        osTimerStop(logTimerID);
 }

 void imu_logging_start(void)
 {
    /* TODO: Change function pointer to the imu logging function (log_imu) */
        log_function = &log_imu;
    

    /* TODO: Reset the log counter */
        logCount = 0;

    /* TODO: Start data logging at 200Hz */
        osTimerStart(logTimerID, 5);
 }

static void log_imu(void)
{
    /* Read IMU data */
    IMU_read();

    /* Get the IMU angle from accelerometer readings (assuming Y and Z axis data) */
    double Y_angle = get_acc_angle();

    /* Get the IMU X gyro reading (In radians per second) */
    float gyroX_rps = get_gyroX(); 

    /* Print time, accelerometer angle, gyro angular velocity*/
    printf("%f,%f,%f\n", logCount/200.0, Y_angle, gyroX_rps);

    /* Increment log count */
    logCount++;

    /* Stop logging once 5 seconds (1000 samples at 200Hz) is reached */
    if (logCount >= 1000)
    {
        logging_stop();
    }
}


 