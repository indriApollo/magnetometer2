
#include "main.h"

#include "arrow.h"
#include "calib_icon.h"
#include "calib_screen.h"
#include "calib_loading.h"
#include "stm32f7xx_hal.h"

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_ts.h"

#include "lsm303.h"
#include "graphics.h"
#include "uart.h"
#include <math.h>

#define HEADING_OFFSET 166

#define LCD_WIDTH 480
#define LCD_HEIGHT 272
#define IMGSIZE (LCD_WIDTH*LCD_HEIGHT*2)

#define FB1 0xc0000000
#define FB2 (FB1+IMGSIZE)

static sample accSample;
static sample magSample;

static LTDC_LayerCfgTypeDef pLayerCfg;
static LTDC_LayerCfgTypeDef pLayerCfg1;

static TS_StateTypeDef ts_state;
static uint8_t touchToggle = 0;
static uint8_t screenToggle = 0;
static float loadingRad = 0;

UART_HandleTypeDef huart1;
I2C_HandleTypeDef hi2c1;
LTDC_HandleTypeDef hLtdcHandler;

void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_LTDC_Init(void);

static void initScreen1(void);
static void initScreen2(void);
static void switchScreens(void);
static void updateScreen1(float rad, int16_t deg);
static void updateScreen2(void);

// we have plenty of ram anyways
uint16_t buf1[272*272];
uint16_t buf2[90*90];
uint16_t fontBuffer[50*74*3];
uint16_t fontBufferRtd[50*74*3];


int main(void)
{
  /* Enable I-Cache-------------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache-------------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_LTDC_Init();
  BSP_TS_Init(LCD_WIDTH, LCD_HEIGHT);
  BSP_SDRAM_Init();

  initScreen1();
  initScreen2();

  writeAccConfig(&hi2c1);
  writeMagConfig(&hi2c1);
  HAL_Delay(100);

  float rad, deg, roll, pitch, rollCos, rollSin, pitchCos, pitchSin;
  float p = 0.125f;
  float degprev = 0;
  char msg[64];
 int16_t heading;

  while (1)
  {
	  readAccSample(&hi2c1, &accSample);
	  readMagSample(&hi2c1, &magSample);

	  roll = atan2(accSample.y, accSample.z);
	  rollCos = cos(roll);
	  rollSin = sin(roll);
	  magSample.y = (magSample.y*rollCos)-(magSample.z*rollSin);
	  magSample.z = (magSample.y*rollSin)+(magSample.z*rollCos);
	  accSample.z = (accSample.y*rollSin)+(accSample.z*rollCos);
	  pitch = atan2(-accSample.x, accSample.z);
	  pitchCos = cos(pitch);
	  pitchSin = sin(pitch);
	  magSample.x = (magSample.x*pitchCos)+(magSample.z*pitchSin);
	  //magSample.z = (magSample.z*pitchSin)+(magSample.z*pitchCos);

	  heading = ((int16_t)(HEADING_OFFSET-atan2(magSample.y, magSample.x)*180.0f/M_PI))%360;
	  if(heading<0)
		  heading += 360;

	  deg = (1-p)*degprev + p*(float)heading;
	  degprev = deg;
	  rad = deg*M_PI/180.0f;

	  if(!screenToggle) {
		  updateScreen1(rad, (int16_t)deg);
		  snprintf(msg, sizeof(msg), "angle: %d\n", (int16_t)deg);
		  uart_send(&huart1, msg);
	  }
	  else {
		  updateScreen2();
		  snprintf(msg, sizeof(msg), "mag: %d %d %d\n", magSample.x, magSample.y, magSample.z);
		  uart_send(&huart1, msg);
	  }

	  switchScreens();
	  HAL_Delay(20);
  }
}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 200;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Activate the Over-Drive mode 
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 50;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USART1 init function */
static void MX_USART1_UART_Init(void) // Uses USB ST-LINK (cn14)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x20404768; // 100 khz
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

    /**Configure Analogue filter 
    */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure Digital filter 
    */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }

}

/* LTDC init function */
static void MX_LTDC_Init(void)
{
	BSP_LCD_Init();

  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = LCD_WIDTH;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = LCD_HEIGHT;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = (uint32_t)FB1;
  pLayerCfg.ImageWidth = LCD_WIDTH;
  pLayerCfg.ImageHeight = LCD_HEIGHT;
  pLayerCfg.Backcolor.Blue = 255;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hLtdcHandler, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }

  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = LCD_WIDTH;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = LCD_HEIGHT;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  pLayerCfg1.Alpha = 0;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg1.FBStartAdress = (uint32_t)FB2;
  pLayerCfg1.ImageWidth = LCD_WIDTH;
  pLayerCfg1.ImageHeight = LCD_HEIGHT;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hLtdcHandler, &pLayerCfg1, 1) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOK_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

}

static void initScreen1(void)
{
	clearBuffer((uint16_t*)FB1, IMGSIZE, 0x0000);
	angleText(0.0f, fontBuffer);
	cpyRotated(fontBuffer, fontBufferRtd, 150, 74);
	cpyToFb(fontBufferRtd, 74, 150, (uint16_t*)FB1, LCD_WIDTH, 400, 61);
	cpyToFb(arrow, 272, 272, (uint16_t*)FB1, LCD_WIDTH, 100, 0);
	cpyToFb(calib_icon, 64, 64, (uint16_t*)FB1, LCD_WIDTH, 0, 208);
}

static void initScreen2(void)
{
	cpyRotated(calib_screen, (uint16_t*)FB2, 272, 480);
	cpyToFb(calib_loading, 90, 90, (uint16_t*)FB2, LCD_WIDTH, 20, 91);
}

static void switchScreens(void)
{
	BSP_TS_GetState(&ts_state);
	if(ts_state.touchDetected && ts_state.touchX[0] <= 64 && ts_state.touchY[0] >= 208) {
		if(!touchToggle) { // first time we touch
			touchToggle = 1;

			HAL_LTDC_SetAlpha_NoReload(&hLtdcHandler, 0, screenToggle);
			screenToggle = !screenToggle;
			HAL_LTDC_SetAlpha_NoReload(&hLtdcHandler, 255, screenToggle);
			HAL_LTDC_Reload(&hLtdcHandler, LTDC_RELOAD_VERTICAL_BLANKING);
		}
	} else {
		touchToggle = 0; // reset touch
	}
	BSP_TS_ResetTouchData(&ts_state);
}

static void updateScreen1(float rad, int16_t deg)
{
	angleText(deg, fontBuffer);
	cpyRotated(fontBuffer, fontBufferRtd, 150, 74);
	cpyToFb(fontBufferRtd, 74, 150, (uint16_t*)FB1, LCD_WIDTH, 400, 61);
	rotateImage(rad, arrow, buf1, 272, 272);
	cpyToFb(buf1, 272, 272, (uint16_t*)FB1, LCD_WIDTH, 100, 0);
}

static void updateScreen2(void)
{
	rotateImage(loadingRad, calib_loading, buf2, 90, 90);
	cpyToFb(buf2, 90, 90, (uint16_t*)FB2, LCD_WIDTH, 20, 91);
	loadingRad += 0.31415926536;
}

void Error_Handler(void)
{
  while(1) {}
}
