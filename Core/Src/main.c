/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ns_con_device.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
typedef struct _SteanReportStruct{
	uint8_t buttA;
	uint8_t buttB;
	uint8_t buttX;
	uint8_t buttY;
	uint8_t buttMenuL;
	uint8_t buttMenuM;
	uint8_t buttMenuR;
	uint8_t buttL1;
	uint8_t buttL2;
	uint8_t buttL3;
	uint8_t buttR1;
	uint8_t buttR2;
	uint8_t buttR3;
	uint8_t buttLStickM;
	uint8_t buttLPadM1;
	uint8_t buttLPadM2;
	uint8_t buttLPadL;
	uint8_t buttLPadR;
	uint8_t buttLPadU;
	uint8_t buttLPadD;
	uint8_t buttRPadM1;
	uint8_t buttRPadM2;
	int16_t analLStickX;
	int16_t analLStickY;
	uint16_t analLT;
	uint16_t analRT;
	int16_t analLPadX;
	int16_t analLPadY;
	int16_t analRPadX;
	int16_t analRPadY;
} SteanReportStruct;

SteanReportStruct steamreport;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  //MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
	NSCON_init();
	HAL_SPI_Receive_DMA(&hspi1, (uint8_t*)&steamreport, sizeof(steamreport));
  /* USER CODE END 2 */
 
 

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 12;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System 
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &htim10){
		NSCON_Timer1_Callback();
	} else if (htim == &htim11){
		NSCON_Timer2_Callback();
	}
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi){
	NSCON_ButtonList_TypeDef food = {0};
	if(steamreport.buttA)food.buttA = 1;
	if(steamreport.buttB)food.buttB = 1;
	if(steamreport.buttX)food.buttX = 1;
	if(steamreport.buttY)food.buttY = 1;
	if(steamreport.buttMenuL)food.buttMinus = 1;
	if(steamreport.buttMenuM)food.buttHome = 1;
	if(steamreport.buttMenuR)food.buttPlus = 1;
	if(steamreport.buttL1)food.buttL1 = 1;
	if(steamreport.buttL2)food.buttL2 = 1;
	if(steamreport.buttR1)food.buttR1 = 1;
	//if(steamreport.buttR2)food.buttR2 = 1;
	if(steamreport.buttLStickM)food.buttLStickM = 1;
	if(steamreport.buttLPadL)food.buttLPadL = 1;
	if(steamreport.buttLPadR)food.buttLPadR = 1;
	if(steamreport.buttLPadU)food.buttLPadU = 1;
	if(steamreport.buttLPadD)food.buttLPadD = 1;
	if(steamreport.buttRPadM2)food.buttRStickM = 1;
	
	food.analLStickX = steamreport.analLStickX >> 4;
	food.analLStickY = steamreport.analLStickY >> 4;
	food.analRStickX = steamreport.analRPadX >> 4;
	food.analRStickY = steamreport.analRPadY >> 4;
	
	NSCON_Parse_Report(&food);
	
	HAL_SPI_Receive_DMA(&hspi1, (uint8_t*)&steamreport, sizeof(steamreport));
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
