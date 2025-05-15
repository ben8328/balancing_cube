#include "tm_stm32_mpu6050.h"
#include <math.h>
#include "IMU.h"
#include <stdint.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Variable declarations */
TM_MPU6050_t IMU_datastruct;

/* Function defintions */
void IMU_init(void)
{
    /* TODO: Initialise IMU with AD0 LOW, accelleration sensitivity +-4g, gyroscope +-250 deg/s */
    TM_MPU6050_Result_t result = TM_MPU6050_Init(&IMU_datastruct, TM_MPU6050_Device_0, TM_MPU6050_Accelerometer_4G, TM_MPU6050_Gyroscope_250s);


    if (result == TM_MPU6050_Result_Ok) {
        printf("IMU initialized successfully.\n");
    } 
    else if (result == TM_MPU6050_Result_DeviceNotConnected) {
        printf("Error: MPU6050 not connected.\n");
    } 
    else {
        printf("Error: MPU6050 initialization failed with unknown error.\n");
    }
}

void IMU_read(void)
{
    /* TODO: Read all IMU values */
    TM_MPU6050_ReadAll(&IMU_datastruct);
}

float get_accY(void)
{
    /* TODO: Convert accelleration reading to ms^-2 */
    /* Convert raw accelerometer reading to g */
    float accY_ms2 = IMU_datastruct.Accelerometer_Y * 9.81 / 8192;  // For 4g -> 1g = 8192 (32768/4) (Scaled)

    /* TODO: return the Y acceleration */
    return accY_ms2;
}

float get_accZ(void)
{
    /* Convert raw accelerometer Z reading to g (4g sensitivity -> 1g = 8192) */
    float accZ_ms2 = IMU_datastruct.Accelerometer_Z * 9.81 / 8192;

    /* Return the Z acceleration in m/s^2 */
    return accZ_ms2;
}

float get_accX(void)
{
    /* Convert raw accelerometer Z reading to g (4g sensitivity -> 1g = 8192) */
    float accX_ms2 = IMU_datastruct.Accelerometer_X * 9.81 / 8192;

    /* Return the Z acceleration in m/s^2 */
    return accX_ms2;
}

float get_gyroX(void)
{
    /* Convert degrees per second to radians per second */
    float gyroX_rads = IMU_datastruct.Gyroscope_X * (M_PI / 180) /131.072; // For 250 sensetivity -> 1rad = 131.072 (32768/250) (Scaled)
    // printf("\n%d\n",IMU_datastruct.Gyroscope_X);

    /* Return the X angular velocity in radians per second */
    return gyroX_rads;
}

double get_acc_angle(void)
{
    /* Get the Y and Z acceleration in m/s^2 */
    float accY = get_accY();
    float accZ = get_accZ();
    /* TODO: compute IMU angle using accY and accZ using atan2 */
    double angle = -atan2(accZ, accY) + 1.5708;  // Offset by 90 degrees
    
    /* Convert angle from radians to degrees*/
    // angle = angle * (180.0 / M_PI);
    
    /* TODO: return the IMU angle */
    return angle;
}

