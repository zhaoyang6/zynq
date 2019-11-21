#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "key_interupt.h"
XGpio   LEDInst;     //LED接口状态
XGpio   BTNInst;    //按键接口状态
XScuGic INTCInst;  //中断状态


int main()
{
	int status;
	init_platform(); //初始化平台
	// 初始化按键
	status = XGpio_Initialize(&BTNInst, BTNS_DEVICE_ID);
	if(status != XST_SUCCESS) return XST_FAILURE;
	  //初始化LED
	status = XGpio_Initialize(&LEDInst, LED_DEVICE_ID);
	if(status != XST_SUCCESS) return XST_FAILURE;
	// 设置按键IO的方向为输入
	XGpio_SetDataDirection(&BTNInst, 1, 0xFF);
	//设置LED IO的方向为输出
	XGpio_SetDataDirection(&LEDInst, 1, 0x00);
	status = IntcInitFunction(INTC_DEVICE_ID, &BTNInst);
	if(status != XST_SUCCESS) return XST_FAILURE;
	print("Hello World11\n\r");
	while(1){
	}
    cleanup_platform();
    return 0;
}
