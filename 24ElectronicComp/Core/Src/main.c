/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "menu.h"
#include "stdio.h"
#include "oled.h"
#include "geometry.h"
#include "motor.h"
#include "string.h"

struct page p0, p1, p2, p3, p4, p5, p6;
void menu_init(void);
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MAX_DATALEN 128 //X = 53 --> 128
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int16_t sin_wave[MAX_DATALEN];//俯仰角数据缓存
int16_t sincnt=0;//俯仰角数据缓存计数
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void sin_wave_gen(void)
{
  uint8_t wave_index=0;
  uint8_t wave_i=0;

  oled_clear();
  oled_show_string(0, 0, "sin_wave_gen()");
  HAL_TIM_Base_Start_IT(&htim10);
  HAL_TIM_Base_Start_IT(&htim7);
  global_wave_type=1;
  __HAL_TIM_SET_AUTORELOAD(&htim10, global_freq);

  while (1)
  {
    wave_i++;
    if(wave_i>=5/global_freq)
    {
      wave_i=0;
      wave_index++;
      wave_index%=200;   
      for(uint8_t i=0;i<MAX_DATALEN;i++) 
      {
        OLED_ClearPoint(i,sin_wave[i]*64/4095);
      }         
      if(sincnt < MAX_DATALEN)//sine数据保存
			{
				sin_wave[sincnt++] = sine[wave_index];
			}
			else
			{
				memcpy((void *)sin_wave, (void *)(sin_wave + 1), sizeof(sin_wave[0]) * (MAX_DATALEN - 1));
				sin_wave[MAX_DATALEN - 1] = sine[wave_index];
			}
    }
    for (uint8_t i = 0; i < MAX_DATALEN ; i++)
				OLED_DrawPoint(i,sin_wave[i]*64/4095 );
    OLED_Refresh();
    HAL_Delay(1);
    if (ADCY > MAX_ADC_VAL) {
      HAL_Delay(KEY_DelayTime);
      if (ADCY > MAX_ADC_VAL) {
          global_wave_type=0;
          HAL_TIM_Base_Stop_IT(&htim10);
          MenuRender(1);
          return;
      }
    }
  }
}

void square_wave_gen(void)
{
  oled_clear();
  oled_show_string(0, 0, "square_wave_gen()");
  HAL_TIM_Base_Start_IT(&htim10);
  HAL_TIM_Base_Start_IT(&htim7);
  global_wave_type=2;
  __HAL_TIM_SET_AUTORELOAD(&htim10, global_freq);
  while (1)
  {
    oled_show_string(0,0,"sine_wave");
    HAL_Delay(5);

    if (ADCY > MAX_ADC_VAL) {
      HAL_Delay(KEY_DelayTime);
      if (ADCY > MAX_ADC_VAL) {
        global_wave_type=0;
          HAL_TIM_Base_Stop_IT(&htim10);
          MenuRender(1);
          return;
      }
    }
  }
}

void angle_confirm(void)
{
  oled_clear();
  oled_show_string(0, 0, "angle_confirm()");
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, HORIZON_PWM);
  
  while (1)
  {
    oled_show_uint(0,1,HORIZON_PWM,5);
    __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, HORIZON_PWM);
    HAL_Delay(10);

    if (ADCY > MAX_ADC_VAL) {
      HAL_Delay(KEY_DelayTime);
      if (ADCY > MAX_ADC_VAL) {
        MenuRender(1);
        return;
      }
    }
    if (ADCX > MAX_ADC_VAL) {
      HAL_Delay(100);
      if (ADCX > MAX_ADC_VAL) {
          HORIZON_PWM--;
      }
    }
    if (ADCX < MIN_ADC_BAL) {
        HAL_Delay(100);
        if (ADCX < MIN_ADC_BAL) {
          HORIZON_PWM++;
        }
    }
  }
}

void speed_control(void)
{
  oled_clear();
  oled_show_string(0, 0, "speed_control()");
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, HORIZON_PWM);
  HAL_TIM_Base_Start_IT(&htim7);
  HAL_UARTEx_ReceiveToIdle_DMA(&huart3,rx_buf,127);	
  control_state=1;
  while (1)
  {
    oled_show_int(0,1,speed.now_error,5);
    oled_show_int(0,2,speed.pre_error,5);
    oled_show_int(0,3,speed.pwm_out,5);
    HAL_Delay(10);

    if (ADCY > MAX_ADC_VAL) {
      HAL_Delay(KEY_DelayTime);
      if (ADCY > MAX_ADC_VAL) {
        HAL_TIM_Base_Stop_IT(&htim7);
        MenuRender(1);
        return;
      }
    }
    
  }
}

void location_control(void)
{
  uint8_t tmp_string[30]={0};

  oled_clear();
  oled_show_string(0, 0, "location_control()");
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, HORIZON_PWM);
  HAL_TIM_Base_Start_IT(&htim7);
  HAL_UARTEx_ReceiveToIdle_DMA(&huart3,rx_buf,127);	
  control_state=2;
  while (1)
  {
    sprintf((char *)tmp_string,"spd_err:%d",speed.now_error);
    oled_show_string(0,1,(const char *)tmp_string);
    sprintf((char *)tmp_string,"spd_out:%d",speed.pwm_out);
    oled_show_string(0,2,(const char *)tmp_string);
    sprintf((char *)tmp_string,"loc_tar:%d",location.set_targetS);
    oled_show_string(0,3,(const char *)tmp_string);
    sprintf((char *)tmp_string,"loc_err:%d",location.now_error);
    oled_show_string(0,4,(const char *)tmp_string);
    sprintf((char *)tmp_string,"sum_err:%d",location.sum_error);
    oled_show_string(0,5,(const char *)tmp_string);
    sprintf((char *)tmp_string,"loc_out:%d",location.pwm_out);
    oled_show_string(0,6,(const char *)tmp_string);
    sprintf((char *)tmp_string,"total_out:%d",location.pwm_out+speed.pwm_out);
    oled_show_string(0,7,(const char *)tmp_string);
    oled_clear();

    HAL_Delay(10);

    if (ADCY > MAX_ADC_VAL) {
      HAL_Delay(KEY_DelayTime);
      if (ADCY > MAX_ADC_VAL) {
        HAL_TIM_Base_Stop_IT(&htim7);
        MenuRender(1);
        return;
      }
    }
    
  }
}
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
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM2_Init();
  MX_TIM7_Init();
  MX_DAC_Init();
  MX_TIM10_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  printf("SYSTEM START\r\n");
  oled_init();
  menu_init();
  sin_basedata(); //basic sin function data generate
  square_basedata(); //basic square function data generate
  HAL_ADC_Start_IT(&hadc1);
  HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, HORIZON_PWM);
  spd_pid_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    MenuCmd(key_scan());
    if (navigate[cntpage]->dymantic_page)
    {
        MenuRender(0);
        HAL_Delay(100);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void menu_init(void)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
{
  add_func(&p0, "<loc_control>", location_control);
  add_func(&p0, "<speed_control>", speed_control);
  add_func(&p0, "<sin_wave_gen>", sin_wave_gen);
  add_func(&p0, "<square_wave_gen>", square_wave_gen);
  add_func(&p0, "<angle_confirm>", angle_confirm);
  add_subpage(&p0, "<pid>", &p1);
  add_subpage(&p0, "<param>", &p2);

  add_value(&p2, "[global_freq]", (int *)&global_freq, 1, NULL);
  add_value(&p2, "[HORIZON_PWM]", (int *)&HORIZON_PWM, 1, NULL);

  MenuInit(&p0);
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
