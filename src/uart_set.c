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
  * �������ƣ� int32 open_com_dev(char *Dev) 
  * ˵    ���� 
  * ��    ���� 
  ******************************************************************************
  */

int32 open_com_dev(char *Dev) 
{ 
	int32 fd = open(Dev,O_RDWR|O_NOCTTY|O_NDELAY); 
	//O_NOCTTY������Unix����������Ϊ�������նˡ����Ƶĳ��򣬲�˵�������־�Ļ����κ����붼��Ӱ����ĳ���
	//O_NDELAY������Unix������򲻹���DCD�ź���״̬���������˿��Ƿ����У���˵�������־�Ļ����ó���ͻ���DCD�ź���Ϊ�͵�ƽʱֹͣ��	

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

	fcntl(fd, F_SETFL, O_NONBLOCK);//����Ϊ��������ʽ��

	//printf("open_com_dev %s = %d.\n",Dev,fd);
	

	return fd; 

	
} 

/*
  ******************************************************************************
  * �������ƣ�close_com_dev(int32 fd) 
  * ˵    ���� 
  * ��    ���� 
  ******************************************************************************
  */
void close_com_dev(int32 fd) 
{ 
	close(fd); 	
} 



/*
  ******************************************************************************
  * �������ƣ�set_com_para(int32 fd,int32 speed, int32 databits,int32 stopbits,int32 parity) 
  * ˵    �������ô�������λ��ֹͣλ��Ч��λ 
  * ��    ����	speed   �����ʣ�����ֵ�������������lu32name_arr[];
  			databits����λȡֵ Ϊ 7����8;
  			stopbitsֹͣλ ȡֵΪ 1����2;
  			parity  Ч������ȡֵΪN,E,O,S.
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
	cfmakeraw(&newtio);  //����Ϊԭʼģʽ��
	
	for (i=0;i<(sizeof(lu32speed_arr)/sizeof(int32));i++) { 
		if(speed == lu32name_arr[i]){ 
			cfsetispeed(&newtio,lu32speed_arr[i]);  
			cfsetospeed(&newtio,lu32speed_arr[i]); 
			newtio.c_cflag |= lu32speed_arr[i];
			break;				    
		} 
		    
	} 

	newtio.c_cflag &= ~CSIZE;
	switch (databits){ //��������λ��
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

	switch (parity) {	//����У�鷽ʽ
		case 'n': 
		case 'N': 
			newtio.c_cflag &= ~PARENB; // Clear parity enable 
			break; 

		case 'o': 
		case 'O': 
			newtio.c_cflag |= PARENB;  
        		newtio.c_cflag |= PARODD;  //����Ϊ��Ч��
        		newtio.c_iflag |= (INPCK | ISTRIP);// Disnable parity checking
			break; 
		case 'e': 
		case 'E': 
			newtio.c_iflag |= (INPCK |ISTRIP);  //Disnable parity checking
			newtio.c_cflag |= PARENB;  // Enable parity
			newtio.c_cflag &= ~PARODD;  //ת��ΪżЧ��  
			break; 
		default: 
			newtio.c_cflag &= ~PARENB; // Clear parity enable 
			break;

	} 

	switch (stopbits) {	//����ֹͣλ
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





  

