
#include "main.h"
#include "port.h"


extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim17;



void NMI_Handler(void)
{

  while (1)
  {
  }

}


void HardFault_Handler(void)
{

  while (1)
  {

  }
}


void SVC_Handler(void)
{

}


void PendSV_Handler(void)
{
}


void SysTick_Handler(void)
{
  HAL_IncTick();
}



void TIM17_IRQHandler(void)
{
  
  if(__HAL_TIM_GET_FLAG(&htim17, TIM_FLAG_UPDATE) != RESET && __HAL_TIM_GET_IT_SOURCE(&htim17, TIM_IT_UPDATE) !=RESET) {
    __HAL_TIM_CLEAR_IT(&htim17, TIM_IT_UPDATE);
    
    if(!--counter)
      prvvTIMERExpiredISR();	
  }
  
  HAL_TIM_IRQHandler(&htim17);
}



void USART2_IRQHandler(void)
{
  
  //Данные пришли, RDR при чтении сбрасывает флаг
  if((__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET) &&(__HAL_UART_GET_IT_SOURCE(&huart2, UART_FLAG_RXNE) != RESET)) {
    prvvUARTRxISR(  ); 
    return;
  }
  //Отправка данных TXE сбрасывается приз записи в TDR
  if((__HAL_UART_GET_FLAG(&huart2, UART_FLAG_TXE) != RESET) &&(__HAL_UART_GET_IT_SOURCE(&huart2, UART_IT_TXE) != RESET)) {
    prvvUARTTxReadyISR(  );
    return ;
  }
  
  /*!< UART overrun error | noise error  | frame error | parity error       */
  if((__HAL_UART_GET_FLAG(&huart2, USART_ISR_ORE) != RESET) || (__HAL_UART_GET_FLAG(&huart2, USART_ISR_NE) != RESET) \
    || ( __HAL_UART_GET_FLAG(&huart2, USART_ISR_ORE) != RESET)  || (__HAL_UART_GET_FLAG(&huart2, USART_ISR_PE) != RESET)) 
  {
    __HAL_UART_CLEAR_FLAG(&huart2, USART_ISR_ORE);
    __HAL_UART_CLEAR_FLAG(&huart2, USART_ISR_NE);
    __HAL_UART_CLEAR_FLAG(&huart2, USART_ISR_ORE);
    __HAL_UART_CLEAR_FLAG(&huart2, USART_ISR_PE);
     uart_error_global = 1;
     return;
  }
  
}





