

#include "main.h"
#include "mb.h"
#include "mbport.h"
#include "stm32f0xx_hal_uart_ex.h"
#include "Flash_EEPROM.h"
#include "led.h"
#include "Buttons_menu.h"
#include "kty_81_110.h"



TIM_HandleTypeDef htim17;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim14;
ADC_HandleTypeDef hadc;
DMA_HandleTypeDef hdma_adc;


void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM17_Init(void);




extern USHORT  usRegInputStart;
extern USHORT  usRegInputBuf[REG_INPUT_NREGS];
extern USHORT  usRegHoldingStart;

static uint32_t lock_nesting_count = 0;



void __critical_enter(void)
{
  __disable_irq();
  ++lock_nesting_count;
}


void __critical_exit(void)
{
  --lock_nesting_count;
  if (lock_nesting_count == 0) {
    __enable_irq();
  }
}

eMBErrorCode    eStatus;

int main(void)
{

  SystemClock_Config();
  
  
  MX_GPIO_Init();
  MX_TIM17_Init();

  
  
  
  uint32_t UART_bod = 115200;
  eMBMode UserModbusMode = MB_RTU;

  eStatus = eMBInit( UserModbusMode, 0x01, 0, (ULONG)UART_bod, MB_PAR_NONE );
  eStatus = eMBEnable(  );
  uart_error_global = 0;
  
  
  
  while (1)
  {
    

    

    if(uart_error_global)
    {
      eStatus = eMBDisable(  );
      eStatus = eMBInit( UserModbusMode, (UCHAR)SLAVE_ID, 0, (ULONG)UART_bod, MB_PAR_NONE );
      eStatus = eMBEnable(  );
      uart_error_global = 0;
    }
    else
      eStatus = eMBPoll();
    
    
    
    
  }
  
}







void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;  
  
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}



static void MX_TIM17_Init(void)
{
  
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 23;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 49;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  
}



/*----------------------modbus functions-----------------------------*/
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int             iRegIndex;
  
  if( ( usAddress >= REG_INPUT_START )
     && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
  {
    iRegIndex = ( int )( usAddress - usRegInputStart );
    while( usNRegs > 0 )
    {
      *pucRegBuffer++ =
        ( unsigned char )( usRegInputBuf[iRegIndex] >> 8 );
      *pucRegBuffer++ =
        ( unsigned char )( usRegInputBuf[iRegIndex] & 0xFF );
      iRegIndex++;
      usNRegs--;
    }
  }
  else
  {
    eStatus = MB_ENOREG;
  }
  
  return eStatus;
}


eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int             iRegIndex;
  
  return eStatus;
}


eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
  /*

  */
  
  return MB_ENOREG;
}


eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  return MB_ENOREG;
}




static void MX_GPIO_Init(void)
{
  
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
}


void Error_Handler(void)
{
  
  __disable_irq();
  while (1)
  {
  }
  
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
  
}

#endif 
