#ifndef  __KEY_INTERYPT__H__
#define __KEY_INTERYPT__H__
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xgpio.h"
#include <stdio.h>

extern XGpio   LEDInst;     //LED�ӿ�״̬
extern XGpio   BTNInst;    //�����ӿ�״̬
extern XScuGic INTCInst;  //�ж�״̬

//��������
#define INTC_DEVICE_ID     XPAR_PS7_SCUGIC_0_DEVICE_ID  //�ж�ID
#define LED_DEVICE_ID       XPAR_GPIO_0_DEVICE_ID              //LED�豸����ַID
#define BTNS_DEVICE_ID    XPAR_GPIO_1_DEVICE_ID              //�����豸����ַID
#define INTC_GPIO_INTERRUPT_ID  XPAR_FABRIC_AXI_GPIO_1_IP2INTC_IRPT_INTR
#define BTN_INT XGPIO_IR_CH1_MASK     //ͨ��1�ж�����

#define DELAY 100000000
#define INT_CFG0_OFFSET 0x00000C00
#define INT_TYPE_RISING_EDGE    0x03
#define INT_TYPE_HIGHLEVEL      0x01
#define INT_TYPE_MASK           0x03

void BTN_Intr_Handler(void *baseaddr_p);
int InterruptSystemSetup(XScuGic *XScuGicInstancePtr);
void IntcTypeSetup(XScuGic *InstancePtr, int intId, int intType);
int IntcInitFunction(u16 DeviceId, XGpio *GpioInstancePtr);

#endif
