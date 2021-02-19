#include "usart.h"




uint8_t TxBuffer[] = "Buffer Send from USARTy to USARTz using Flags";
uint8_t RxBuffer[TxBufferSize + 1];
   

extern UART_HandleTypeDef huart2;

char  data[20], txdata[20];

char  u2_rx_buffer[U2_BUFFER_SIZE];
uint32_t  u2_rx_point_head = 0; 
uint32_t  u2_rx_point_tail = 0;  

//----------------------------------------------------------------------------- 
void u2_increase_point_value(uint32_t * data_p)
{
    (* data_p) ++;
    if(U2_BUFFER_SIZE == (* data_p))
    {
        (* data_p) = 0;
    }
}
//-----------------------------------------------------------------------------  
bool Uart2_Is_Empty(void)
{
    if(u2_rx_point_head == u2_rx_point_tail)
    {
        return TRUE;
    }
    return FALSE;
}
//-----------------------------------------------------------------------------  
void Uart2_EnQueue(uint8_t data)
{
    u2_rx_buffer[u2_rx_point_head] = data;
    u2_increase_point_value(&u2_rx_point_head);
}
//-----------------------------------------------------------------------------  
uint16_t Uart2_DeQueue(void)
{
    uint16_t retVal = u2_rx_buffer[u2_rx_point_tail];
    u2_increase_point_value(&u2_rx_point_tail);
//    printf("Rx : 0x%02X\n", retVal);    
    return retVal;
}
//-----------------------------------------------------------------------------  
