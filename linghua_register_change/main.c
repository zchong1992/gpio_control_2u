

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
#define MAP_SIZE 4096UL
#define MAP_MASK (MAP_SIZE - 1)
#define NEED_LEN 11
#define LOG(_log_)  printf("[%s][line %d]:(%s)\n", __FILE__,__LINE__,  _log_);


#define SUCCESS 1
#define FAIL 0
int getintfromchar(char ch)
{
	if(ch>='0' &&ch<='9')
		return ch-'0';
	else if(ch>='a' &&ch<='f')
		return ch-'a'+10;
	else if(ch>='A' &&ch<='F')
		return ch-'A'+10;
	else 
		return -1;
}

UINT8 praseStr(char * str)
{
	int i;
	UINT8 addr=0;
	UINT4 vaule[4]={0};
	int int_tmp;
	UINT1 flag=0;
	for(i=0;i<NEED_LEN;i++)
	{
		int_tmp=getintfromchar(str[i]);
		if(int_tmp!=-1)
		{
			vaule[flag]=(vaule[flag]<<4) + int_tmp;
		}
		else
		{
			flag++;
		}
	}
	addr= (vaule[3]<<24)+(vaule[2]<<16)+(vaule[1]<<8)+vaule[0];
	return addr;
}

#define LAN 100
#define WAN 101

int setlan()
{
    UINT1 data=0;
    ioperm(0x000,0x5FF,1);
    usleep(10000);    
    data=inb(0x548); 
    usleep(1000);    
    data&=0x80;
    data|=0x0e;
    outb(data,0x548);
    usleep(1000);
    ioperm(0x000,0x5FF,0);
    return 0;
}
int setwan_and_enable_rlck()
{
    UINT1 data=0;
    ioperm(0x000,0x5FF,1);
    usleep(10000);   
    data=inb(0x548);  
    usleep(1000);    
    data&=0x80;
    data|=0x71;
    outb(data,0x548);
    usleep(1000);
    data=inb(0x538);
    usleep(1000);
    data|=0x03;
    outb(data,0x538);
    usleep(1000);
    ioperm(0x000,0x5FF,0);
    return 0;
}
UINT1 getstatus()
{
    UINT1 data=0;
    ioperm(0x000,0x5FF,1);
    usleep(10000);    //10ms
    data=inb(0x548);  //gpio base + 0x48
    usleep(10000);    //10ms
    ioperm(0x000,0x5FF,0);
    if((data&0x7e)==0x0e)
    {
        printf("NOW is LAN mode\n");
    }
    else if((data&0x7e)==0x70&&(data&0x01))
    {
        printf("NOW is WAN mode rclk enable \n");
    }
    else if((data&0x7e)==0x70&&!(data&0x01))
    {
        printf("NOW is WAN mode rclk disable \n");
    }
    else
    {
        printf("error mode code:\t0x%X\n",data);
    }
    return data;
    
}
int setwan_and_disable_rlck()
{
    UINT1 data=0;
    ioperm(0x000,0x5FF,1);
    usleep(10000);     //10ms
    data=inb(0x548);  //gpio base + 0x48
    usleep(1000);       //10ms
    data&=0x80;
    data|=0x70;
    outb(data,0x548);
    usleep(1000);
    data=inb(0x538);
    usleep(1000);
    data|=0x03;
    outb(data,0x538);
    usleep(1000);
    ioperm(0x000,0x5FF,0);
    return 0;
}
void showhelp()
{
    printf("this command can be used\n");
    printf("\t0\t-v/-V\n");
    printf("\t1\tstatus/get_status\n");
    printf("\t2\tlan/LAN\n");
    printf("\t3\twan/WAN(default disable rclk)\n");
    printf("\t4\twan/WAN enable/Enable  [rclk]\n");
    printf("\t5\twan/WAN disable/Disable  [rclk]\n");
}
void showVresion()
{
	printf("+---------------------------------+\n");
	printf("|                                 |\n");
	printf("|              GreenNet           |\n");
	printf("|             LAN / WAN           |\n");
	printf("|           Version 1.0.2         |\n");
    printf("|        For linghua 2U server    |\n");
	printf("|      compiled at %s    |\n",__DATE__);
	printf("|    author:zhengchong@GreenNet   |\n");
    printf("|                                 |\n");
	printf("+---------------------------------+\n");
}
#define DISABLE 0
#define ENABLE 1
int main(int argc, char **argv) 
{
	unsigned long   value=0;
	int mode=LAN;
    int enable=DISABLE;
	if(argc==1)
	{
		showhelp();
		return 0;
	}
    if(argc>=2&&(strncmp(argv[1],"-v",2)&&strncmp(argv[1],"-V",2))==0)
	{
		showVresion();
        return 0;
	}
	if(argc>=2&&(strncmp(argv[1],"lan",3)&&strncmp(argv[1],"LAN",3))==0)
	{
		mode=LAN;
	}
	else if((strncmp(argv[1],"wan",3)&&strncmp(argv[1],"WAN",3))==0)
	{
		mode=WAN;
	}
    else if((strncmp(argv[1],"status",6)&&strncmp(argv[1],"get_status",6))==0)
	{
		getstatus();
        return 0;
	}
	else
	{
		showhelp();
        return 0;
	}
    if(argc>=3&&mode==WAN&&(strncmp(argv[2],"disable",7)&&strncmp(argv[2],"Disable",7))==0)
	{
		enable=DISABLE;
	}
    else if(argc>=3&&mode==WAN&&(strncmp(argv[2],"enable",6)&&strncmp(argv[2],"Enable",6))==0)
	{
		enable=ENABLE;
	}
    else if(argc>=3&&mode==WAN)
    {
        showhelp();
        return 0;
    }
	int fd;
	void *map_base, *virt_addr; 
	argc= argc;
	argv[0][0]=argv[0][0];
	UINT8 offset;
	FILE * fp;
	char buffer[1024];
	fp=popen("lspci -d 8086:10fc -xx |grep \"^10\"","r");
	fgets(buffer,sizeof(buffer),fp);
	if(mode==LAN)
	{
		value=0xFFFF;
	}
	else if(mode==WAN)
	{
		value=0xFFFFF;
	}
	//printf("value:\t0x%lX\n",value);
	if(feof(fp))
	{
		printf("only use in linghua 2U server or no found wan ka \n");
        return 0;
	}
	else
	{
		while(!feof(fp))  
		{
			if(*(UINT2*)"10"==*(UINT2*)buffer)
			{
				offset=praseStr(buffer+4);
				offset=offset&0xFFFFFF00;
				offset=offset+ 0x4248;
				if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) 
					return 0;
			    map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset & ~MAP_MASK);
			    if(map_base == (void *) -1) 
			    {
			    	printf("mmap fail %llu\n",offset);
					return 0;
			    }
			    virt_addr = map_base + (offset & MAP_MASK) ;
				printf("offset:0x%llx\t\t",offset);
				printf("current value:\t0x%lX\t",*((unsigned long *) virt_addr));
				*((unsigned long *) virt_addr) = value;
				printf("update value:\t0x%lX\t",*((unsigned long *) virt_addr));
			    if(munmap(map_base, MAP_SIZE) == -1)
		    	{
		    		printf("munmap fail %llu\n",offset);
					return 0;
		    	}
			}
			printf("\n");
			fgets(buffer,sizeof(buffer),fp);
		}
	}
	pclose(fp);
    if(mode ==LAN)
    {
        setlan();
    }
    else if(mode ==WAN && enable==DISABLE)
    {
        setwan_and_disable_rlck();
    }
    else if(mode ==WAN && enable==ENABLE)
    {
        setwan_and_enable_rlck();
    }
    getstatus();
	return 0;
}





