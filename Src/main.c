/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
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
#include "fatfs.h"
#include "usb_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Motor_Control.h"
#include "hx_UI.h"
#include "hx_Message.h"
#include "hx_Resource.h"
#include "hx_API.h"
#include "hx_APP.h"
#include "hx_LED.h"
#include "hx_USBDisk.h"
#include "hx_TouchLCD.h"
#include "hx_Key.h"
#include "hx_MotorDriver.h"
#include "hx_mcuFlash.h"
#include "hx_Error.h" //pjg++190906
#include "stm32f4xx_hal_flash_ex.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PRINTF_BUF  100

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM7_Init(void);
static void MX_TIM3_Init(void);
static void MX_SPI3_Init(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
/*
uint8_t g_cUart1_Buff[10];
uint8_t g_cUart3_Buff[10];
uint8_t g_cUart1_RxFlag;
uint8_t g_cUart3_RxFlag;
*/
uint8_t   Uart_Rx_Buff[4]; 
//
//->use from smart 
//#ifndef USE_MOTOR_CTRL_PJG
uint32_t  ADC_value[10], Pwm_Value = 450, Task_TIMER = 0;
uint32_t  Home_flag = 100, Home_TIMER = 0;
uint8_t   Home_sensor = 0, Run_mode = 0, Home_mode = 0, Timer_sec = 0, dir = 0;
uint16_t  MOTOR_STATE, Total_Counter = 0;
//#endif
//<-to smart
//
char CommonBuf[FLASH_PAGE_SIZE16*1];
uint32_t tickOld;

#ifdef USE_MOTOR_CTRL_PJG
int32_t     encoder, Target_Angle, Target_counter;
uint32_t  Motor_Current, Over_Current;
uint8_t FF1, FF2; 
uint32_t ADC_rms;
#else
extern  int32_t     encoder, Target_Angle, Target_counter;
extern  uint32_t  Motor_Current, Over_Current;
extern  uint8_t ff1, ff2; 
#endif
extern  ApplicationTypeDef Appli_state;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//--------------------------------------------------------------------------------------------------
void uart_putstring(char *pbuf)
{
	int i;
	char bufchk[PRINTF_BUF];

	//while (sTx.head != sTx.tail);
	
	for (i = 0; i < PRINTF_BUF; i++) {
		bufchk[i] = pbuf[i];
		if (pbuf[i] == 0) break;
//		uart_putchar(pbuf[i]);
	}
	printf("%s\r\n", bufchk); 
}

void USBH_UserProcess (USBH_HandleTypeDef *phost, uint8_t id)
{
	switch (id)
	{
	case HOST_USER_DISCONNECTION:
		Appli_state = APPLICATION_DISCONNECT;
	break;
		/* when HOST_USER_CLASS_ACTIVE event is received, application can start
		communication with device*/
	case HOST_USER_CLASS_ACTIVE:
		Appli_state = APPLICATION_READY;
		break;
	case HOST_USER_CONNECTION:
		Appli_state = APPLICATION_START;
		break;
	default:
		break;
	}
}

void USB_Process(void)
{
	MX_USB_HOST_Process();
}

void USB_Init(void)
{
	API_SetTimer(OS_TIMER_ID_2, 200);
}

/*
void MotorDrv_Init(void)
{
	Motor_Init();
	HAL_ADC_Start_DMA(&hadc1,ADC_value,1);

	MOTOR_STATE = MOTOR_CCW;    
	Motor_direction();   
      Target_counter = 0;
	Pwm_Value =START_PWM;
	Motor_Current = 0;

	Run_mode = 0;
	Home_mode = 0;
	Task_TIMER = 0;
	MotorDrv_Init2();
}
*/

void DeviceDrv_Init(void)
{	
	//UARTDrv_Init();
	//USART3_Init();
	TLCD_Driver_Init();
	KeyDrv_Init();
	LEDDrv_Init();
	USBDisk_Init();
	USB_Init();
#ifdef USE_FUN_REMOTE
	UI_Remote_Init();
#endif
	//uart_putchar('\r');
//	uart_putchar('\r');
//          printf("\r");
    uart_putstring("\r");
}

void USB_ClockDisable(void)
{
	RCC->PLLCFGR &= 0xf0ffffff; //PLLQ = 0
}

void USB_ClockEnable(void)
{
	RCC->PLLCFGR |= 0x07000000; //PLLQ = 0
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart)
{
	if (huart->Instance == USART1) {	//lcd touch
		sTLcd_q.buf[sTLcd_q.head] = Uart_Rx_Buff[0];
		sTLcd_q.head++;
		if (sTLcd_q.head >= TLCD_BUF_SIZE) sTLcd_q.head = 0;
		//HAL_UART_Receive_IT(huart, (uint8_t *)&Uart_Rx_Buff[0], 1);
		HAL_UART_Receive_DMA(huart, (uint8_t *)&Uart_Rx_Buff[0], 1);
	}
	else if (huart->Instance == USART3) {	//pc comm
		HAL_UART_Receive_IT(huart, (uint8_t *)&Uart_Rx_Buff[2], 1);
		#ifdef USE_FUN_REMOTE
		UI_Remote_PutData(Uart_Rx_Buff[2]);
		#endif
	}
}

void uart_putchar3(char data)
{
	HAL_UART_Transmit(&huart3, (uint8_t *)&data, 1, 5);
}

void uart_putstring3(char *pbuf)
{
	int i;
	
	for (i = 0; i < PRINTF_BUF; i++) {
		if (pbuf[i] == 0) break;
		uart_putchar3(pbuf[i]);
	}
}

void SoundOutError(void) //pjg++190903
{
	int i;
	uint32_t temp;
    
	for (i = 0; i < 3; i++) {
		uart_putstring(UI_GetSndInfo(14));
        temp = HAL_GetTick();
		while (1) {
			if ((HAL_GetTick()-temp) >= (uint32_t)150) break;
		}
		//Task_TIMER = 1;
	}
}


//--------------------------------------------------------------------------------------------------  
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
    FLASH_OBProgramInitTypeDef flash;

    //NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);        //<- 요 부분 추가
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
  HAL_GPIO_WritePin(LCD_PWR_GPIO_Port, LCD_PWR_Pin, GPIO_PIN_RESET);                                // LCD PWR LOW 
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_USART3_UART_Init();
  MX_USART1_UART_Init();
  MX_TIM7_Init();
  MX_TIM3_Init();
  MX_SPI3_Init();
  MX_USB_HOST_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
#ifdef USE_MOTOR_CTRL_PJG
	HAL_TIM_Base_Start(&htim7);
	HAL_TIM_Base_Start_IT(&htim3);
#else
	HAL_TIM_Base_Start_IT(&htim7);
	HAL_TIM_Base_Start(&htim3);
#endif
	//#if 0//ndef USE_MOTOR_CTRL_PJG
	//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	//HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); 
	//#endif
  
#ifndef USE_MOTOR_CTRL_PJG
	Pwm_Value = START_PWM;
#endif
	Motor_Current = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
/*
  FF1
  FF2
  Run_mode
  SpdTmWnd.speed
  Target_Angle
  ADC_rms
*/
//------------------------------------------------------------------ Motor TEST
 /* 
 #if 0 //orid
  HAL_ADC_Start_DMA(&hadc1,ADC_value,1);
 #else
	Motor_Init();
	HAL_ADC_Start_DMA(&hadc1,ADC_value,1);

	MOTOR_STATE = MOTOR_CCW;	
	Motor_direction();
#endif


  
//  Target_counter = 0;
  Pwm_Value = START_PWM;
  Motor_Current = 0;  

 
  Task_TIMER = 0;
  
  
//  1 60
//  2 30
//  3 20
//  4 15
//  5 12
  
  while (1){
      
      if(Home_mode == 0){
          
          if(Task_TIMER < 12000){
              Target_Angle = 50;
          }
          else if(Task_TIMER >= 24000){
              Task_TIMER = 0;            
          }            
          else if(Task_TIMER >= 12000){
              Target_Angle = 0;            
          }     

      }    
 //     else      Target_counter = 0;
      HAL_Delay(1);
  }
*/
  
//------------------------------------------------------------------ Motor TEST 
//motor release
//HAL_GPIO_WritePin(COAST_GPIO_Port, COAST_Pin, GPIO_PIN_RESET);      // COAST LOW
/*  
  Motor_Init();
  MOTOR_STATE = MOTOR_CCW;    
  Motor_direction();
  Run_mode = 1;  
  Home_mode = 1;   

  while (1){
      if(!(HAL_GPIO_ReadPin(HOME_GPIO_Port,HOME_Pin))){                               // home check
        if(Home_TIMER++ >10){
        
                      Pwm_Value = START_PWM;
                      encoder = 0;
                      __HAL_TIM_SetCounter(&htim3,0);
                      Home_sensor = 1;
                      Home_mode = 0;
      //                MOTOR_STATE = MOTOR_CW;
      //                HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_RESET);                          // DIR LOW                   
                      Target_Angle = 0;
        }         
      }
  }
  while (1){

  }
  */
//------------------------------------------------------------------- Motor TEST    
  //Motor_Init();
  //HAL_ADC_Start_DMA(&hadc1,ADC_value,1);
  
  //MOTOR_STATE = MOTOR_CCW;    
  //Motor_direction();
  //HAL_GPIO_WritePin(LCD_PWR_GPIO_Port, LCD_PWR_Pin, GPIO_PIN_RESET);                                // LCD PWR LOW 
  OS_Init();
  Over_Current = 500;				 
  HAL_UART_Receive_DMA(&huart1, Uart_Rx_Buff, 1);
  /* USART1 interrupt Init */
  HAL_NVIC_SetPriority(USART3_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(USART3_IRQn);
  #ifdef USE_FUN_REMOTE
  HAL_UART_Receive_IT(&huart3, &Uart_Rx_Buff[2],1);
  #endif
  UI_LED_Control(LM_BEFORE_BOOT);
  tickOld = HAL_GetTick();
  HAL_FLASHEx_OBGetConfig(&flash);
  if(flash.RDPLevel != OB_RDP_LEVEL_1 ){
    flash.RDPLevel = OB_RDP_LEVEL_1;
    HAL_FLASHEx_OBProgram(&flash);
    //flash.RDPLevel = OB_RDP_LEVEL_0;
  }
  
 while (1)
  {
    /* USER CODE END WHILE */
    //MX_USB_HOST_Process();

    /* USER CODE BEGIN 3 */
     //HAL_UART_Receive_IT(&huart1, &Uart_Rx_Buff[0], 1);
     /*
      if(!(HAL_GPIO_ReadPin(STOP_SW_GPIO_Port,STOP_SW_Pin))){  
          
          if(Stop_TIMER >= 500 ){
              Run_mode = 0;                
          }
      }
      else          Stop_TIMER = 0;*/
	OS_Process();

	//if(Task_TIMER >= 1){//10){                                                                         // 10ms
	if ((HAL_GetTick() - tickOld) > 10) {
		//Stop_TIMER++;
		//Task_TIMER = 0;
		tickOld = HAL_GetTick();
		UI_Timer_Process();
		App_Timer_Process();
		OS_Timer_Process();
		MotorDrv_Process();
		UI_CheckMotorStatus(); //pjg++190830 : if press machine when stanby the motor gear is broken
		if (API_CheckErrorCode()) {
			if (UI_DisplayErrorCode(API_GetErrorCode()) == EDT_DISP_HALT) { 
				MotorDrv_Disable();
				MotorDrv_Release();
				SoundOutError();
				__disable_irq();
				//halt();
				while (1);
			}
			for (int i = 0; i < 2000; i++) {
				HAL_Delay(1);
			}
		}
	}
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
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 20000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  #ifdef USE_MOTOR_CTRL_PJG
  htim3.Init.Period = 1;
  #else
  htim3.Init.Period = 10000;
  #endif
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_BOTHEDGE;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim3, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 83;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
#ifdef USE_MOTOR_CTRL_PJG
  htim7.Init.Period = 99;
#else
  htim7.Init.Period = 99;
#endif
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, MODE_Pin|LCD_PWR_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, DIR_Pin|BRAKE_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, RESET_Pin|COAST_Pin|ESF_Pin|LED_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, USB_PWR_Pin|LED_B_Pin|LED_G_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SPI3_CS_Pin|WP_Pin|HOLD_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : MODE_Pin LCD_PWR_Pin */
  GPIO_InitStruct.Pin = MODE_Pin|LCD_PWR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : HOME_Pin */
  GPIO_InitStruct.Pin = HOME_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(HOME_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : DIR_Pin BRAKE_Pin */
  GPIO_InitStruct.Pin = DIR_Pin|BRAKE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : FF1_Pin FF2_Pin STOP_SW_Pin */
  GPIO_InitStruct.Pin = FF1_Pin|FF2_Pin|STOP_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RESET_Pin COAST_Pin ESF_Pin */
  GPIO_InitStruct.Pin = RESET_Pin|COAST_Pin|ESF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : DIRO_Pin */
  GPIO_InitStruct.Pin = DIRO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DIRO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_PWR_Pin */
  GPIO_InitStruct.Pin = USB_PWR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_PWR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_B_Pin LED_G_Pin */
  GPIO_InitStruct.Pin = LED_B_Pin|LED_G_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_R_Pin */
  GPIO_InitStruct.Pin = LED_R_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_R_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SPI3_CS_Pin WP_Pin HOLD_Pin */
  GPIO_InitStruct.Pin = SPI3_CS_Pin|WP_Pin|HOLD_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void MX_DriverVbusFS(uint8_t state)
{
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
  while(1) 
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
