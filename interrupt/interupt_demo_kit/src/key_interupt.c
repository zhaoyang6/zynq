#include "key_interupt.h"
static int btn_value;  //按键值
void IntcTypeSetup(XScuGic *InstancePtr, int intId, int intType)
{
	int mask;
    intType &= INT_TYPE_MASK;
    mask = XScuGic_DistReadReg(InstancePtr, INT_CFG0_OFFSET + (intId/16)*4);
    mask &= ~(INT_TYPE_MASK << (intId%16)*2);
    mask |= intType << ((intId%16)*2);

    XScuGic_DistWriteReg(InstancePtr, INT_CFG0_OFFSET + (intId/16)*4, mask);
}
int IntcInitFunction(u16 DeviceId, XGpio *GpioInstancePtr)
{
    //中断控制器配置实例
    XScuGic_Config *IntcConfig;
    int status;
    // Interrupt controller initialization
    //找到scugic实例(根据设备ID查找中断向量)
    IntcConfig = XScuGic_LookupConfig(DeviceId);
    //初始化scugic(状态检测，对中断初始化，成功返回XST_SUCCESS)
    status = XScuGic_CfgInitialize(&INTCInst, IntcConfig, IntcConfig->CpuBaseAddress);
    if(status != XST_SUCCESS) return XST_FAILURE;

    // Call interrupt setup function
    status = InterruptSystemSetup(&INTCInst);
    if(status != XST_SUCCESS) return XST_FAILURE;

    // 注册中断函数
    status = XScuGic_Connect(&INTCInst,
                             INTC_GPIO_INTERRUPT_ID,
                             (Xil_ExceptionHandler)BTN_Intr_Handler,
                             (void *)GpioInstancePtr);
    if(status != XST_SUCCESS) return XST_FAILURE;

    //设置中断触发类型为上升沿
    IntcTypeSetup(&INTCInst,INTC_GPIO_INTERRUPT_ID,INT_TYPE_RISING_EDGE);
    // Enable GPIO interrupts
    XGpio_InterruptEnable(GpioInstancePtr, 1);
    XGpio_InterruptGlobalEnable(GpioInstancePtr);

    // 使能我们的按键中断(中断号61)
    XScuGic_Enable(&INTCInst, INTC_GPIO_INTERRUPT_ID);

    return XST_SUCCESS;
}

int InterruptSystemSetup(XScuGic *XScuGicInstancePtr)
{
    // Register GIC interrupt handler
    //通用异常处理程序,中断后统一由GIC先处理，然后在HanderTable中查找相应的处理函数
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
                                 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
                                 XScuGicInstancePtr);
    //使能异常处理
    Xil_ExceptionEnable();

    return XST_SUCCESS;

}
//中断处理函数
void BTN_Intr_Handler(void *InstancePtr)
{
    unsigned char led_val = 0;
    // Ignore additional button presses
    if ((XGpio_InterruptGetStatus(&BTNInst) & BTN_INT) !=BTN_INT)
    {
	  // Disable GPIO interrupts
		XGpio_InterruptDisable(&BTNInst, BTN_INT);
		 return;
    }
    btn_value = XGpio_DiscreteRead(&BTNInst, 1);
    printf("btn value:%d\n",btn_value);
    switch (btn_value)
    {
		case 1: led_val = 0x01; break;
		case 2: led_val = 0x02; break;
		case 8: led_val = 0x08; break;
		case 4: led_val = 0x04; break;
		default:  break;
    }
    XGpio_DiscreteWrite(&LEDInst,1,led_val);
      // Acknowledge GPIO interrupts
      (void)XGpio_InterruptClear(&BTNInst, BTN_INT);
      // Enable GPIO interrupts
      XGpio_InterruptEnable(&BTNInst, BTN_INT);
}
