#include "motor.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "cmsis_os2.h"
#include "uart.h"

// For Motor PWM
TIM_HandleTypeDef htim1;  // Global declaration for TIM1

// For Motor Encoders
int32_t enc_count1 = 0;
int32_t enc_count2 = 0;
int32_t enc_count3 = 0;

void motor_pwm_init(void) {
    /* Enable TIM1 clock */
    __HAL_RCC_TIM1_CLK_ENABLE();
    /* Enable GPIOA clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    // PWM pins: PA8 (CH1), PA9 (CH2), PA10 (CH3)
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_8; // | GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Initialize PA0, PA1 for Motor 1 direction control */
    GPIO_InitStruct.Pin = M1_DIR_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    // GPIO_InitStruct.Pin = M2_DIR_PIN;
    // HAL_GPIO_Init(M2_DIR_GPIO_PORT, &GPIO_InitStruct);

    // GPIO_InitStruct.Pin = M3_DIR_PIN;
    // HAL_GPIO_Init(M3_DIR_GPIO_PORT, &GPIO_InitStruct);

    // // Brake pin
    // GPIO_InitStruct.Pin = BRAKE_PIN;
    // HAL_GPIO_Init(BRAKE_GPIO_PORT, &GPIO_InitStruct);
    
    /* Initialize timer 1 for PA8 (CH1), PA9 (CH2), PA10 (CH3) to control motor enable pins */
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 1;       // 
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 19998;   // 
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    // htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) {
        printf("Error: TIM1 PWM init failed\r\n");
        return;
    }

    /* Configure timer 1, channels for PA8 (CH1), PA9 (CH2), PA10 (CH3) */
    TIM_OC_InitTypeDef sConfigOC;
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;        // Start with 0% duty cycle
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    // Configure PWM Channel 1 for PA8 (Motor 1 Enable)
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, M1_PWM_CHANNEL) != HAL_OK)
    {
        // Handle error
        printf("Error: PWM channel configuration for TIM1 CH1 failed.\n");
        return;
    }
    // // Configure PWM Channel 2 for PA9 (Motor 2 Enable)
    // if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, M2_PWM_CHANNEL) != HAL_OK)
    // {
    //     // Handle error
    //     printf("Error: PWM channel configuration for TIM1 CH2 failed.\n");
    //     return;
    // }
    // // Configure PWM Channel 3 for PA10 (Motor 3 Enable)
    // if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, M3_PWM_CHANNEL) != HAL_OK)
    // {
    //     // Handle error
    //     printf("Error: PWM channel configuration for TIM1 CH3 failed.\n");
    //     return;
    // }

     /* Start PWM on TIM1 Channel 1 (PA8, ENA) and Channel 2 (PA9, ENB) */
    if (HAL_TIM_PWM_Start(&htim1, M1_PWM_CHANNEL) != HAL_OK)
    {
        printf("Error: Failed to start PWM on TIM1 CH1.\n");
        return;
    }

    // if (HAL_TIM_PWM_Start(&htim1, M2_PWM_CHANNEL) != HAL_OK)
    // {
    //     printf("Error: Failed to start PWM on TIM1 CH2.\n");
    //     return;
    // }
    // if (HAL_TIM_PWM_Start(&htim1, M3_PWM_CHANNEL) != HAL_OK)
    // {
    //     printf("Error: Failed to start PWM on TIM1 CH3.\n");
    //     return;
    // }
    else {
        // (printf("Timer CH1, CH2 & CH3 Initialized"));
        (printf("Timer CH1 Initialized"));
    }
}

void motor_encoder_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    
    // Encoder pins: PA6/PA7, PB4/PB5, PB6/PB7
    GPIO_InitStruct.Pin = M1_ENCA_PIN | M1_ENCB_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(M1_ENCA_GPIO_PORT, &GPIO_InitStruct);
    // GPIO_InitStruct.Pin = M2_ENCA_PIN | M2_ENCB_PIN;
    // HAL_GPIO_Init(M2_ENCA_GPIO_PORT, &GPIO_InitStruct);
    // GPIO_InitStruct.Pin = M3_ENCA_PIN | M3_ENCB_PIN;
    // HAL_GPIO_Init(M3_ENCA_GPIO_PORT, &GPIO_InitStruct);

    /* Set priority and enable EXTI interrupt for lines 4 to 9 (PA6, PA7, PB4, PB5) */
    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0x0f, 0x0f);
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

    printf("\nMotor Encoders Initialized\n");
}

void EXTI9_5_IRQHandler(void) {
    // Handle Encoder for Motor 1 
    if (__HAL_GPIO_EXTI_GET_IT(M1_ENCA_PIN) != RESET) {
        // Check the direction using PA6 (A) and PA7 (B) for Motor 1
        if (HAL_GPIO_ReadPin(M1_ENCA_GPIO_PORT, M1_ENCA_PIN) != HAL_GPIO_ReadPin(M1_ENCB_GPIO_PORT, M1_ENCB_PIN))
        {
            enc_count1++;   // Encoder is moving in one direction
        }  
        else enc_count1--;  // Encoder is moving in the opposite direction

        // Clear the interrupt flag for PA6
        HAL_GPIO_EXTI_IRQHandler(M1_ENCA_PIN);
    }
    // Handle Encoder for Motor 1 (PA7)
    if (__HAL_GPIO_EXTI_GET_IT(M1_ENCB_PIN) != RESET)
    {
        HAL_GPIO_EXTI_IRQHandler(M1_ENCB_PIN);
    }
}

static float counts_to_radians(int32_t counts) {
    return (counts * MOTOR_2PI) / MOTOR_CPR;
}

float motor1_get_wheel_rotation(void) {
    return counts_to_radians(enc_count1);
}

float motor2_get_wheel_rotation(void) {
    return counts_to_radians(enc_count2);
}

float motor3_get_wheel_rotation(void) {
    return counts_to_radians(enc_count3);
}

void motor_set_voltage(float voltage_motor1, float voltage_motor2, float voltage_motor3)
{
    float dutyCycle_motor1;
    float dutyCycle_motor2;
    // Motor 1 direction control (PA0, PA1)
    if (voltage_motor1 > 0)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);   // CW = HIGH
        // PWM Frequency between 16-26kHz
        dutyCycle_motor1 = voltage_motor1*6060;//6060*3.3 20000Hz MAX Duty cycle
    }
    else if (voltage_motor1 < 0)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // CCW = LOW
        dutyCycle_motor1 = voltage_motor1*-6060;//1667*6 10000Hz MAX Duty cycle
        
        
        
    }
    else
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
    }

    // Set PWM duty cycle for Motor 1 enable (ENA -> PA8, TIM1 CH1)
    // dutyCycle_motor1 = -dutyCycle_motor1;  // Make positive for PWM
    if (dutyCycle_motor1 > 19998) dutyCycle_motor1 = 19998;  // Limit duty cycle to 100%
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, dutyCycle_motor1); 
    printf("\nTimer Channel 1 set to duty cycle: %f\n" , dutyCycle_motor1);
}