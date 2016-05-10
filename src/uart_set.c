/*
  *******************************************************************************
  * @file    uart_set.c
  * @author  zjjin
  * @version V0.0.0
  * @date    02-24-2016
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 

#include "includes.h"
#include "uart_set.h"
  

/*
  ******************************************************************************
  * 函数名称： int32 open_com_dev(char *Dev) 
  * 说    明： 
  * 参    数： 
  ******************************************************************************
  */

int32 open_com_dev(char *Dev) 
{ 
	int32 fd = open(Dev,O_RDWR|O_NOCTTY|O_NDELAY); 
	//O_NOCTTY：告诉Unix这个程序不想成为“控制终端”控制的程序，不说明这个标志的话，任何输入都会影响你的程序。
	//O_NDELAY：告诉Unix这个程序不关心DCD信号线状态，即其他端口是否运行，不说明这个标志的话，该程序就会在DCD信号线为低电平时停止。	

	if (-1 == fd) { 
		printf("Can't Open Serial Port.\n"); 
		return -1; 
	} 

	fcntl(fd, F_SETFL, FNDELAY);

	//if(fcntl(fd,F_SETFL,0) < 0){
	//	printf("open_com_dev fcntl failed.\n");
	//}

	if(isatty(STDIN_FILENO) == 0){
		//printf("standard input is not a terminal device.\n");
	}

	fcntl(fd, F_SETFL, O_NONBLOCK);//设置为非阻塞方式。

	//printf("open_com_dev %s = %d.\n",Dev,fd);
	

	return fd; 

	
} 

/*
  ******************************************************************************
  * 函数名称：close_com_dev(int32 fd) 
  * 说    明： 
  * 参    数： 
  ******************************************************************************
  */
void close_com_dev(int32 fd) 
{ 
	close(fd); 	
} 



/*
  ******************************************************************************
  * 函数名称：set_com_para(int32 fd,int32 speed, int32 databits,int32 stopbits,int32 parity) 
  * 说    明：设置串口数据位，停止位和效验位 
  * 参    数：	speed   波特率，允许值详见下面数数组lu32name_arr[];
  			databits数据位取值 为 7或者8;
  			stopbits停止位 取值为 1或者2;
  			parity  效验类型取值为N,E,O,S.
  ******************************************************************************
  */

int8 set_com_para(int32 fd,int32 speed, int32 databits,int32 stopbits,int32 parity) 
{ 
	int i = 0;
	struct termios newtio; 
	struct termios oldtio;
	int32 lu32speed_arr[] = {B115200,B38400,B19200,B9600,B4800,B2400,B1200,B300}; 
	int32 lu32name_arr[] = {115200,38400,19200,9600,4800,2400,1200,300}; 
    //printf("[%s][%s][%d] speed: %d, databits: %d, stopbits: %d, parity: %c\n", FILE_LINE, speed, databits, stopbits, parity);
	if(tcgetattr(fd,&oldtio) != 0)  
	{  
	    perror("SetupSerial 1.\n");	
	    return -1;  
	} 

	tcflush(fd,TCIOFLUSH);  

	bzero(&newtio,sizeof(newtio));  
	cfmakeraw(&newtio);  //配置为原始模式。
	
	for (i=0;i<(sizeof(lu32speed_arr)/sizeof(int32));i++) { 
		if(speed == lu32name_arr[i]){ 
			cfsetispeed(&newtio,lu32speed_arr[i]);  
			cfsetospeed(&newtio,lu32speed_arr[i]); 
			newtio.c_cflag |= lu32speed_arr[i];
			break;				    
		} 
		    
	} 

	newtio.c_cflag &= ~CSIZE;
	switch (databits){ //设置数据位数
		case 7: 
			newtio.c_cflag |= CS7; 
			break; 
		case 8: 
			newtio.c_cflag |= CS8; 
			break; 
		default: 
			newtio.c_cflag |= CS8; 
			break; 

	} 

	switch (parity) {	//设置校验方式
		case 'n': 
		case 'N': 
			newtio.c_cflag &= ~PARENB; // Clear parity enable 
			break; 

		case 'o': 
		case 'O': 
			newtio.c_cflag |= PARENB;  
        		newtio.c_cflag |= PARODD;  //设置为奇效验
        		newtio.c_iflag |= (INPCK | ISTRIP);// Disnable parity checking
			break; 
		case 'e': 
		case 'E': 
			newtio.c_iflag |= (INPCK |ISTRIP);  //Disnable parity checking
			newtio.c_cflag |= PARENB;  // Enable parity
			newtio.c_cflag &= ~PARODD;  //转换为偶效验  
			break; 
		default: 
			newtio.c_cflag &= ~PARENB; // Clear parity enable 
			break;

	} 

	switch (stopbits) {	//设置停止位
		case 1: 
			newtio.c_cflag &= ~CSTOPB; 
			break; 
		case 2: 
			newtio.c_cflag |= CSTOPB; 
			break; 
		default: 
			newtio.c_cflag &= ~CSTOPB; 
			break;

	} 
	
	newtio.c_iflag = IGNPAR;
     newtio.c_cflag |= HUPCL | CREAD | CLOCAL;
     

	newtio.c_cc[VTIME] = 50;  
     newtio.c_cc[VMIN] = 1;  
  
     tcflush(fd,TCIOFLUSH);  
	
     if((tcsetattr(fd,TCSANOW,&newtio)) != 0){  
       	 printf("com set error.\n");  
       	 return FALSE;  
     }  
	
   	return TRUE; 


} 





  

