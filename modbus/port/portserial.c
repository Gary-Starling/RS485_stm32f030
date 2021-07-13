/*
* FreeModbus Libary: BARE Port
* Copyright (C) 2006 Christian Walter <wolti@sil.at>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*
* File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
*/

#include "port.h"
#include "stm32f0xx_hal.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "stm32f0xx_hal_uart.h"

/* ----------------------- static functions ---------------------------------*/
UART_HandleTypeDef huart2;

#define USART_CR1_FIELDS  ((uint32_t)(USART_CR1_M | USART_CR1_PCE | USART_CR1_PS |  USART_CR1_TE | USART_CR1_RE | USART_CR1_OVER8 )) /*!< UART or USART CR1 fields of parameters set by UART_SetConfig API */

#define USART_CR3_FIELDS  ((uint32_t)(USART_CR3_RTSE | USART_CR3_CTSE | USART_CR3_ONEBIT))  /*!< UART or USART CR3 fields of parameters set by UART_SetConfig API */


#define UART_BRR_MIN    0x10U        /* UART BRR minimum authorized value */
#define UART_BRR_MAX    0x0000FFFFU  /* UART BRR maximum authorized value */

extern uint8_t tx_busy;
/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
  /* If xRXEnable enable serial receive interrupts. If xTxENable enable
  * transmitter empty interrupts.
  */
  if(xRxEnable)
  {
    while( __HAL_UART_GET_FLAG(&huart2, UART_FLAG_TC) != SET)
    {  
    }
     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
   
  }
  else
  {
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
  }
  
  if(xTxEnable)
  {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE); 
    
  }
  else
  {  
    __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
  }
}


BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
  huart2.Init.Mode = UART_MODE_TX_RX; // работаем на прием и передачу
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE; // без контрол€ потока (у нас же rs485)
  // сэмплинг, не могу нормально объ€снить, но это нужно дл€ защиты от шумов
  huart2.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  // один стоп-бит
  huart2.Init.StopBits = UART_STOPBITS_1;
  // без доп-фич
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  
  
  switch (ucPORT) {
  case 0:
    huart2.Instance = USART2;
    break;
  default:
    // вызовем ошибку, если выбрали не тот номер порта, хот€ это будет ошибкой портировани€, но хоть что-то
    return FALSE;
  }
  
  // скорость
  huart2.Init.BaudRate = ulBaudRate;
  
  // размер слова
  switch (ucDataBits) {
  case 8:
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    break;
  case 9:
    huart2.Init.WordLength = UART_WORDLENGTH_9B;
    break;
  default:
    // вызовем ошибку, если выбрали не тот номер порта, хот€ это будет ошибкой портировани€, но хоть что-то
    return FALSE;
  }
  
  // настраиваем бит четности
  switch (eParity) {
  case MB_PAR_NONE:
    huart2.Init.Parity = UART_PARITY_NONE;
    break;
  case MB_PAR_EVEN:
    huart2.Init.Parity = UART_PARITY_EVEN;
    break;
  case MB_PAR_ODD:
    huart2.Init.Parity = UART_PARITY_ODD;
    break;
  default:
    // вызовем ошибку, если выбрали не тот номер порта, хот€ это будет ошибкой портировани€, но хоть что-то
    return FALSE;
  }
  
  return HAL_UART_Init(&huart2) == HAL_OK ? TRUE : FALSE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
  /* Put a byte in the UARTs transmit buffer. This function is called
  * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
  * called. */   
  huart2.Instance->TDR = ucByte;
  return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
  /* Return the byte in the UARTs receive buffer. This function is called
  * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
  */
  *pucByte = (CHAR)huart2.Instance->RDR;
  
  return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
* (or an equivalent) for your target processor. This function should then
* call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
* a new character can be sent. The protocol stack will then call 
* xMBPortSerialPutByte( ) to send the character.
*/


void prvvUARTTxReadyISR( void )
{
  pxMBFrameCBTransmitterEmpty(  ); 
}

/* Create an interrupt handler for the receive interrupt for your target
* processor. This function should then call pxMBFrameCBByteReceived( ). The
* protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
* character.
*/
void prvvUARTRxISR( void )
{
  pxMBFrameCBByteReceived(  ); // 16.03
}





