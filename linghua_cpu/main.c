

#include <sys/io.h>
#include <linux/kernel.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <ctype.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <linux/errno.h>   
#include <linux/kernel.h>    



typedef unsigned long long int  UINT8;
typedef unsigned  int           UINT4;
typedef unsigned short          UINT2;
typedef unsigned char           UINT1;
typedef long long int           INT8;
typedef int                     INT4;
typedef short                   INT2;
typedef char                    INT1;


#define SMBUS_BASE  0x9000
#define HST_STS     (SMBUS_BASE+0)
#define HST_CNT     (SMBUS_BASE+0x02)
#define HST_CMD     (SMBUS_BASE+0x03)
#define XMIT_SLVA   (SMBUS_BASE+0x04)
#define HST_D0      (SMBUS_BASE+0x05)
#define HST_D1      (SMBUS_BASE+0x06)


#define HOST_BUSY   0x01
#define INTR        0x02


#define SLVA_WRITE_POS    0x5A
#define SLVA_READ_POS     0x5B
UINT1 setbank() 
{
    UINT1 a=0;
    UINT1 sts=0;
    //printf("\tSet bank\n");
    outb(0x1E,HST_STS);  
    usleep(1000);  
    outb(SLVA_WRITE_POS,XMIT_SLVA);  
    usleep(1000);  
    outb(0xff,HST_CMD);
    usleep(1000);
    outb(0x48,HST_CNT);
    usleep(1000);
    sts=inb(HST_STS);
    //printf("STS:%X\n",sts);
    while(1)
    {
        sts=inb(HST_STS);
        if((sts&0x1C)!=0)
        {
            printf("HST_STS error! %x\n",sts);
            return 0;
        }
        if((sts&INTR)!=0
            &&(sts&HOST_BUSY)==0)
            break;
        usleep(1000);
    }
    //printf("bank : %x\n",inb(HST_D0));
    outb(0x00,HST_D0);
    usleep(1000);
    return a;
}
UINT1 getbank() 
{
    UINT1 a=0;
    UINT1 sts=0;
    //printf("\tGet bank\n");
    outb(0x1E,HST_STS);  
    usleep(1000);  
    outb(SLVA_WRITE_POS,XMIT_SLVA);  
    usleep(1000);  
    outb(0xff,HST_CMD);
    usleep(1000);
    outb(0x48,HST_CNT);
    usleep(1000);
    sts=inb(HST_STS);
    //printf("STS:%X\n",sts);
    while(1)
    {
        sts=inb(HST_STS);
        if((sts&0x1C)!=0)
        {
            printf("HST_STS error! %x\n",sts);
            return 0;
        }
        if((sts&INTR)!=0
            &&(sts&HOST_BUSY)==0)
            break;
        usleep(1000);
    }
    //printf("bank : %x\n",inb(HST_D0));
    usleep(1000);
    return a;
}
UINT1 read_data(UINT4 POS) 
{
    UINT1 a=0;
    UINT1 sts=0;
    //printf("read pos:%X\n",POS);
    outb(0x1E,HST_STS);  
    usleep(1000);  
    outb(SLVA_READ_POS,XMIT_SLVA);  
    usleep(1000);  
    outb(POS,HST_CMD);
    usleep(1000);
    outb(0x48,HST_CNT);
    usleep(1000);
    sts=inb(HST_STS);
    /*
    printf("STS:%X\n",sts);
    */
    while(1)
    {
        sts=inb(HST_STS);
        if((sts&0x1C)!=0)
        {
            printf("HST_STS error! %x\n",sts);
            return 0;
        }
        if((sts&INTR)!=0
            &&(sts&HOST_BUSY)==0)
            break;
        usleep(1000);
    }
     /*   
    printf("HST_CNT:%x\n",inb(HST_CNT));
    printf("HST_STS:%x\n",inb(HST_STS));
    printf("HST_D0:%x\n",inb(HST_D0));
    printf("-----\n");
    */
    a=inb(HST_D0);
    usleep(1000);
    return a;
}
float getTem(UINT4 POS)
{

    //printf("***************************\n");
    char a=read_data(POS);
    char b=read_data(POS+1);
    float tem=0;
    //printf("a:%d\n",a);
    //printf("b:%d\n",b);
    tem=(float)a;
    if(tem>=0)
    {
        tem+=0.125*(b&0x7);
    }
    else
    {
        tem-=0.125*(b&0x7);
    }
    return tem;
}
void showVresion()
{
	printf("+---------------------------------+\n");
	printf("|                                 |\n");
	printf("|              GreenNet           |\n");
	printf("|              CPU  TEM           |\n");
	printf("|           Version 1.0.2         |\n");
    printf("|        For linghua 2U server    |\n");
	printf("|      compiled at %s    |\n",__DATE__);
	printf("|    author:zhengchong@GreenNet   |\n");
    printf("|                                 |\n");
	printf("+---------------------------------+\n");
}


int main(int argc, char * argv[])
{
    if(argc>=2&&(strncmp(argv[1],"-v",2)&&strncmp(argv[1],"-V",2))==0)
	{
		showVresion();
        return 0;
	}
    float data=0;
    if(iopl(3)!=0)
    {
        switch (errno)
        {
            case EINVAL:
                printf("EINVAL\n");
                break;
            case ENOSYS:
                printf("ENOSYS\n");
                break;
            case EPERM:
                printf("EPERM\n");
                break;
            default:
                printf("%d\n",errno);
        }
        return 0;
    }
    printf("success to read data:\n");
    setbank();
    getbank();
    data=getTem(0xA0);
    printf("CPU0 %0.2fC\n",data);
    data=getTem(0xA2);
    printf("CPU1 %0.2fC\n",data);
    data=getTem(0x42);
    printf("SYS0 %0.2fC\n",data);
    data=getTem(0x46);
    printf("SYS1 %0.2fC\n",data);
    data=getTem(0x4A);
    printf("SYS2 %0.2fC\n",data);
    usleep(1000);
    return 0;
    
}