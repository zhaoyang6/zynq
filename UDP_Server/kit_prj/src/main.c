#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "myudp.h"

int main()
{
    init_platform();

    My_UDP_Init();

    char* A;
    char* B;
    char* C;
    C=malloc(3);
    C[0]=0xFF;
    C[1]=0xFE;
    C[2]=0xFD;
    printf("mmmm\r\n");
    while(1)
    {
    	A=UDP_Recv(4);
    	if((A[0]==0xAA)&&(A[1]==0xBB))
    	{
    		B=malloc(2);
    		B[0]=A[2];
    		B[1]=A[3];
    		UDP_Send(B,2);
    	}
    	else
    	{
    		UDP_Send(C,3);
    		printf("解析不正确，进入UDP发送模式\r\n");
    	}
    	free(A);free(B);
    }

    cleanup_platform();
    return 0;
}
