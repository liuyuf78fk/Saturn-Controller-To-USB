
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "usbd_hid.h"


void delay_us(uint16_t us)
{
    uint16_t differ = 0xffff - us - 5;
    HAL_TIM_Base_Start(&htim2);
    __HAL_TIM_SET_COUNTER(&htim2, differ);
    while(differ < 0xffff - 5)
    {

        differ = __HAL_TIM_GET_COUNTER(&htim2);
    }
    HAL_TIM_Base_Stop(&htim2);

}

void SystemClock_Config(void);

int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
    return ch;
}
extern USBD_HandleTypeDef hUsbDeviceFS;
;

uint8_t key_Z, key_Y, key_X, key_R;
uint8_t key_B, key_C, key_A, key_St;
uint8_t key_Up, key_Dn, key_Lt, key_Rt;
uint8_t key_L;

#define SCAN_DELAY_TIME	30		//us

void scan_saturn(void)
{
    //s0 s1 0 0
    HAL_GPIO_WritePin(GPIOB, S0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, S1_Pin, GPIO_PIN_RESET);
    delay_us(SCAN_DELAY_TIME);
    key_Z = HAL_GPIO_ReadPin(GPIOB, D0_Pin);
    key_Y = HAL_GPIO_ReadPin(GPIOB, D1_Pin);
    key_X = HAL_GPIO_ReadPin(GPIOB, D2_Pin);
    key_R = HAL_GPIO_ReadPin(GPIOB, D3_Pin);

    //s0 s1 1 0
    HAL_GPIO_WritePin(GPIOB, S0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, S1_Pin, GPIO_PIN_RESET);
    delay_us(SCAN_DELAY_TIME);
    key_B = HAL_GPIO_ReadPin(GPIOB, D0_Pin);
    key_C = HAL_GPIO_ReadPin(GPIOB, D1_Pin);
    key_A = HAL_GPIO_ReadPin(GPIOB, D2_Pin);
    key_St = HAL_GPIO_ReadPin(GPIOB, D3_Pin);

    //s0 s1 0 1
    HAL_GPIO_WritePin(GPIOB, S0_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, S1_Pin, GPIO_PIN_SET);
    delay_us(SCAN_DELAY_TIME);
    key_Up = HAL_GPIO_ReadPin(GPIOB, D0_Pin);
    key_Dn = HAL_GPIO_ReadPin(GPIOB, D1_Pin);
    key_Lt = HAL_GPIO_ReadPin(GPIOB, D2_Pin);
    key_Rt = HAL_GPIO_ReadPin(GPIOB, D3_Pin);

    //s0 s1 1 1
    HAL_GPIO_WritePin(GPIOB, S0_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, S1_Pin, GPIO_PIN_SET);
    delay_us(SCAN_DELAY_TIME);

    key_L = HAL_GPIO_ReadPin(GPIOB, D3_Pin);

    //    printf("____________________________________\n");
    //    printf("key_Z,key_Y,key_X,key_R :%d %d %d %d\n",key_Z,key_Y,key_X,key_R);
    //    printf("key_B,key_C,key_A,key_St :%d %d %d %d\n",key_B,key_C,key_A,key_St);
    //    printf("key_Up,key_Dn,key_Lt,key_Rt :%d %d %d %d\n",key_Up,key_Dn,key_Lt,key_Rt);
    //    printf("key_L:%d\n",key_L);
    //    printf("\n");

}

int main(void)
{

    uint16_t report_data = 0;
    HAL_Init();
    SystemClock_Config();
    //MX_USART1_UART_Init();

    //printf("boot ok\n");
    MX_GPIO_Init();

    MX_USB_DEVICE_Init();
    MX_TIM2_Init();

    while(1)
    {
        if(hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED)
        {
            scan_saturn();
            report_data = 0;
            report_data |= key_L << 0;
            report_data |= key_Z << 1;
            report_data |= key_Y << 2;
            report_data |= key_X << 3;
            report_data |= key_R << 4;
            report_data |= key_B << 5;
            report_data |= key_C << 6;
            report_data |= key_A << 7;
            report_data |= key_St << 8;
            report_data |= key_Up << 9;
            report_data |= key_Dn << 10;
            report_data |= key_Lt << 11;
            report_data |= key_Rt << 12;

            report_data ^= 0x1FFF;

            USBD_HID_SendReport(&hUsbDeviceFS, (uint8_t *) &report_data, sizeof(uint16_t));
            HAL_Delay(20);
        }
    }

}


void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        Error_Handler();
    }
}

void Error_Handler(void)
{

}
