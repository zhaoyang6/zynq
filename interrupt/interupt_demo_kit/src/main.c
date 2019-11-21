#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "key_interupt.h"
XGpio   LEDInst;     //LED�ӿ�״̬
XGpio   BTNInst;    //�����ӿ�״̬
XScuGic INTCInst;  //�ж�״̬


int main()
{
	int status;
	init_platform(); //��ʼ��ƽ̨
	// ��ʼ������
	status = XGpio_Initialize(&BTNInst, BTNS_DEVICE_ID);
	if(status != XST_SUCCESS) return XST_FAILURE;
	  //��ʼ��LED
	status = XGpio_Initialize(&LEDInst, LED_DEVICE_ID);
	if(status != XST_SUCCESS) return XST_FAILURE;
	// ���ð���IO�ķ���Ϊ����
	XGpio_SetDataDirection(&BTNInst, 1, 0xFF);
	//����LED IO�ķ���Ϊ���
	XGpio_SetDataDirection(&LEDInst, 1, 0x00);
	status = IntcInitFunction(INTC_DEVICE_ID, &BTNInst);
	if(status != XST_SUCCESS) return XST_FAILURE;
	print("Hello World11\n\r");
	while(1){
	}
    cleanup_platform();
    return 0;
}
