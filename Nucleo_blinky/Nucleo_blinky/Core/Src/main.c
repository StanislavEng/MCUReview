/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
//#define MPUSlaveAddr 0x68								 // Defines the MPU Slave address
//static const uint8_t MPUSlaveAddr = 0x68;				 // makes it more readable for functions
//static const uint8_t MPU6050_ADDR = MPUSlaveAddr << 1;   // Setting I2C slave address (and saving bit for R/W bit)
#define MPU6050_ADDR 0xD0
//static const uint8_t MPU_WHOAMI = 0x75;                  // for checking if the device is (properly) detected
#define WHO_AM_I_REG 0x75
//#define PWR_MGMT_ADDR 0x6B                             // used for waking up device from sleep
//static const uint8_t
#define PWR_MGMT_1_REG 0x6B				 // makes it more readable for functions
//static const uint8_t
#define SMPLRT_DIV_REG 0x19 				 // sample rate divider register
//static const uint8_t
#define GYRO_CONFIG_REG 0x1B					 // Gyroscope register address
//static const uint8_t
#define ACCEL_CONFIG_REG 0x1C					 // Accelerometer register address
//static const uint8_t
#define ACCEL_XOUT_H_REG 0x3B
//static const uint8_t
#define gyroRead  0x43

static float accelData[2];
static float gyroData[2];
static int16_t X_ACCEL_RAW;
static int16_t Y_ACCEL_RAW;
static int16_t Z_ACCEL_RAW;
static int16_t X_ACCEL;
static int16_t Y_ACCEL;
static int16_t Z_ACCEL;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM16_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM17_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void MPU6050_Init(void){
	uint8_t check, Data;
	Data = 128;
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &Data, 1, 1000);
	Data = 104;
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, WHO_AM_I_REG, 1,  &Data, 1, 1000);
	HAL_I2C_Mem_Read (&hi2c1, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 1000);
	//HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout)
	Data = 0;
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &Data, 1, 1000);
	Data = 0x07;
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &Data, 1, 1000);
	Data = 0x00;
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, 1000);
	Data = 0x00;
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &Data, 1, 1000);



	/*HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, MPU_WHOAMI, 1, addr_check,1, 100); // reads the WHO_AM_I register for its address
	char buff[100];
	//HAL_I2C_Mem_Read(hi2c, DevAddress, MemAddress, MemAddSize, pData, Size, Timeout)
	//sprintf(buff, "Read, but expected value is = %d",addr_check);
	//HAL_UART_Transmit(&huart2, &addr_check, 1, 100);
	if (addr_check == 104){    // checks if the MPU is (properly) detected
	/////// Wakes the device up by setting power management register to 0x0 /////////
		strcpy((char*)buff,"We worked!\r\n");
		HAL_UART_Transmit(&huart2, buff, strlen((char*)buff), HAL_MAX_DELAY);
		data = 0;
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_ADDR, 1, &data, 1, 1000);
	/////// Sets the sampling rate of the device ////////  Gyro rate default = 8kHz
		data = 0x07; // 1kHz sampling rate (sample rate = gyro rate / (1 + sample rate) = 8 / (1 + 7)
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMP_RATE_DIV, 1, &data, 1, 1000);
	/////// Configure Gyroscope settings ////////
		data = 0; // scale range @ +/-250 deg /s     ||    scale rage +/- 2g
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_ADDR, 1, &data, 1, 1000);
		HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_ADDR, 1, &data, 1, 1000);

	}
	   for(uint8_t i=0 ; i <255;i++)
	   {
	       if(HAL_I2C_IsDeviceReady(&hi2c1,i,1 ,10) == HAL_OK )
	       {
	    	  sprintf(buff, "Right at value %f",i);
	    	  HAL_UART_Transmit(&huart2, buff, strlen((char*)buff), HAL_MAX_DELAY);
	          break;
	       }
	    }*/
}
void readAccel(void){
	uint8_t accData[6]; // Takes in : X high, X low, Y high, Y low, Z high, Z low

	//HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, accelRead, I2C_MEMADD_SIZE_8BIT, &accData, 6, 10);
	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H_REG, 1, accData, 6, 1000);
	// Raw Accelerometer data
	X_ACCEL_RAW = (int16_t)( accData[0] << 8 | accData[1] );
	Y_ACCEL_RAW = (int16_t)( accData[2] << 8 | accData[3] );
	Z_ACCEL_RAW = (int16_t)( accData[4] << 8 | accData[5] );

	// Converted Accelerometer data
	X_ACCEL = X_ACCEL_RAW  / 16384.0;
	Y_ACCEL = Y_ACCEL_RAW  / 16384.0;
	Z_ACCEL = Z_ACCEL_RAW  / 16384.0;

	// sending converted data into a array
	accelData[0] = (float)X_ACCEL;
	accelData[1] = (float)Y_ACCEL;
	accelData[2] = (float)Z_ACCEL;


}
/*void readGyro(void){
	uint8_t gyroData[6]; // Takes in : X high, X low, Y high, Y low, Z high, Z low

	//HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, gyroRead, 1, &gyroData, 6, 10);

	// Raw Accelerometer data
	uint16_t X_GYRO_RAW = (int16_t)( gyroData[0] << 8 | gyroData[1] );
	uint16_t Y_GYRO_RAW = (int16_t)( gyroData[2] << 8 | gyroData[3] );
	uint16_t Z_GYRO_RAW = (int16_t)( gyroData[4] << 8 | gyroData[5] );

	// Converted Accelerometer data
	uint16_t X_GYRO = X_GYRO_RAW / 131.0;
	uint16_t Y_GYRO = Y_GYRO_RAW / 131.0;
	uint16_t Z_GYRO = Z_GYRO_RAW / 131.0;

	// sending converted data into a array
	gyroData[0] = X_GYRO ;
	gyroData[1] = Y_GYRO ;
	gyroData[2] = Z_GYRO ;

}*/
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	//uint8_t buff[12];
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  //int readPin = 0;
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM16_Init();
  MX_I2C1_Init();
  MX_TIM17_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim16);
  HAL_TIM_Base_Start_IT(&htim17);
  MPU6050_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //strcpy((char*)buff,"Hello!\r\n");  // temporarily dumping this into the one interrupt I have going
	  //HAL_UART_Transmit(&huart2, buff, strlen((char*)buff), HAL_MAX_DELAY);
	  //HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	  //HAL_Delay(200);
	  /*if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == 1){
		  readPin ^= 1;
		  HAL_Delay(100);
	  }*/
	  if((HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin)==0)){
		  HAL_Delay(100);
		  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
		  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_9);
		  HAL_Delay(100);
	  }
	  char buff[100];
	  readAccel();
	  //strcpy((char*)buff,"Hello There!\r\n");  // temporarily dumping this into the one interrupt I have going
	  //strcpy((char*)buff, "Ax = %f Ay = %f, Az = %f", accelData[0],accelData[1],accelData[2]);
	  sprintf(buff, "Ax = %f Ay = %f, Az = %f\r\n", accelData[0],accelData[1],accelData[2]);
	  HAL_UART_Transmit(&huart2, buff, strlen((char*)buff), HAL_MAX_DELAY);
	  HAL_Delay(200);
	  //readAccel();
	  //sprintf(buff, "Gx = %f Gy = %f, Gz = %f\r\n", gyroData[0],gyroData[1],gyroData[2]);
	  //HAL_UART_Transmit(&huart2, buff, strlen((char*)buff), HAL_MAX_DELAY);
	  //HAL_Delay(200);
	  //HAL_Delay(100);


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10909CEC;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 8000 - 1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 10000 - 1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 8000 - 1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 10000-1;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

}

/**
  * @brief TIM17 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 8000 - 1;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 2000 - 1;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD2_Pin PA8 PA9 */
  GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim == &htim16){
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	}
	if (htim == &htim17){
	 /* char buff[100];
	  readAccel();
	  //strcpy((char*)buff,"Hello There!\r\n");  // temporarily dumping this into the one interrupt I have going
	  //strcpy((char*)buff, "Ax = %f Ay = %f, Az = %f", accelData[0],accelData[1],accelData[2]);
	  sprintf(buff, "Ax = %f Ay = %f, Az = %f\r\n", accelData[0],accelData[1],accelData[2]);
	  HAL_UART_Transmit(&huart2, buff, strlen((char*)buff), HAL_MAX_DELAY);
	  readAccel();
	  sprintf(buff, "Gx = %f Gy = %f, Gz = %f\r\n", gyroData[0],gyroData[1],gyroData[2]);
	  HAL_UART_Transmit(&huart2, buff, strlen((char*)buff), HAL_MAX_DELAY);*/
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


