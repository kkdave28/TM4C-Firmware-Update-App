

/**
 * main.c
 */

#include "main.h"

static void UART_puts(uint32_t Base, uint8_t *buf, uint16_t len);
static uint16_t UART_gets(uint32_t Base, uint8_t *buf);
static void UART_get_chunk(uint32_t Base, uint8_t * buf, uint16_t len);
static void UART_put_chunk(uint32_t Base, uint8_t * buf, uint16_t len);
static uint8_t transaction_buf[CHUNK_SZ];
static void Error(const char * s);
void UART_Handler( void );
static void Delay();
static void Small_Delay();
static void FW_Provide();
static void Delta_FW_Provide();
int main(void)
{

    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinConfigure(GPIO_PB0_U1RX);
    GPIOPinConfigure(GPIO_PB1_U1TX);
    GPIOPinConfigure(GPIO_PC5_U1CTS);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_5);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_3);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 115200,
        (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
//    UARTFlowControlSet(UART1_BASE, UART_FLOWCONTROL_TX);
//    UARTIntEnable(UART1_BASE, UART_INT_CTS);
//    UARTIntRegister(UART1_BASE, UART_Handler);
    // UARTCharPut(UART0_BASE, 'E');
    // UARTCharPut(UART0_BASE, 'n');
    // UARTCharPut(UART0_BASE, 't');
    // UARTCharPut(UART0_BASE, 'e');
    // UARTCharPut(UART0_BASE, 'r');
    // UARTCharPut(UART0_BASE, ' ');
    // UARTCharPut(UART0_BASE, 'T');
    // UARTCharPut(UART0_BASE, 'e');
    // UARTCharPut(UART0_BASE, 'x');
    // UARTCharPut(UART0_BASE, 't');
    // UARTCharPut(UART0_BASE, ':');
    // UARTCharPut(UART0_BASE, ' ');
    while (1)
    {
        unsigned char c = UARTCharGet(BOARD_UART);
        if(c == 's')
        {
            UARTCharPut(BOARD_UART, 'b');
            Delta_FW_Provide();
        }

        //Delay();
    }
}

void UART_puts(uint32_t Base, uint8_t *buf, uint16_t len)
{
    unsigned long index = 0;
    while(len != 0)
    {
        UARTCharPut(Base, buf[index++]);
        len--;
    }
    return;
}
void UART_Handler(void)
{
    UARTIntClear(UART1_BASE, UART_INT_CTS);
    //UART_puts(handler_message);
    Small_Delay();
}

static void Delay()
{
    uint32_t i = 0;
    for(; i< 100000; i++);
}
static void Small_Delay()
{
    uint32_t i = 0;
    for(; i< 1000; i++);
}
uint16_t UART_gets(uint32_t Base, uint8_t *buf)
{
    unsigned char c = 0;
    uint16_t index = 0;
    while(c != '\n')
    {
        c = UARTCharGet(Base);
        buf[index++] = c;
    }
    buf[index] = '\0';
    return index;
}
static void FW_Provide()
{
    uint32_t sz_info = 0;
    uint16_t current_chunk_sz = 0;
    uint16_t rec_sz = 0;
    unsigned char c = 0;
    UARTCharPut(HOST_UART, 'u');
    while (1)
    {
        c = UARTCharGet(HOST_UART);
        if(c == 's')
        {
            rec_sz = UART_gets(HOST_UART, transaction_buf);
            sz_info = atoi(transaction_buf);
            int current_app_sz = sz_info;
            UARTCharPut(BOARD_UART, 'l');
            UART_puts(BOARD_UART, transaction_buf, rec_sz);
            current_chunk_sz = (sz_info > CHUNK_SZ) ? CHUNK_SZ : sz_info;
            c = UARTCharGet(BOARD_UART);
            if(c == 'c')
            {
                UARTCharPut(HOST_UART, 'f');
                while(current_app_sz)
                {
                    UART_get_chunk(HOST_UART, transaction_buf, current_chunk_sz);
                    c = UARTCharGet(BOARD_UART);
                    if(c == 'r')
                    {
                        UART_put_chunk(BOARD_UART, transaction_buf, current_chunk_sz);
                    }
                    current_app_sz -= ((current_app_sz - CHUNK_SZ) > 0) ? CHUNK_SZ : current_app_sz;
                    current_chunk_sz = (current_app_sz > CHUNK_SZ) ? CHUNK_SZ  : current_app_sz;
                    //memset(transaction_buf, 0, CHUNK_SZ);
                    if(current_app_sz)
                        UARTCharPut(HOST_UART, 'f');
                }
            }
            while(1)
            {
                uint16_t len = 0;
                len = UART_gets(BOARD_UART, transaction_buf);
                UART_puts(HOST_UART, transaction_buf, len);
            }

        }
        else
            continue;
    }
}
static void Delta_FW_Provide()
{
    uint32_t sz_info = 0;
    uint16_t current_chunk_sz = 0;
    uint16_t rec_sz = 0;
    unsigned char c = 0;
    UARTCharPut(HOST_UART, 'v');
    while(1)
    {

    }
}
static void UART_get_chunk(uint32_t Base, uint8_t * buf, uint16_t len)
{
    uint16_t i = 0;
    while(len != 0)
    {
        buf[i++] = UARTCharGet(Base);
        len--;
    }
}
static void UART_put_chunk(uint32_t Base, uint8_t * buf, uint16_t len)
{
    uint16_t i = 0;
    while(len != 0)
    {
        UARTCharPut(Base, buf[i++]);
        len--;
    }
}
static void Error(const char * s)
{
    UARTCharPut(UART0_BASE, FW_ERR);
    uint16_t index = 0;
    while(s[index])
        UARTCharPut(UART0_BASE, s[index++]);
    while(1);
}
