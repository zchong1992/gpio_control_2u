#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/io.h>
#include <linux/kernel.h>
#include <unistd.h>

typedef unsigned long long int  UINT8;
typedef unsigned int            UINT4;
typedef unsigned short          UINT2;
typedef unsigned char           UINT1;
typedef long long int           INT8;
typedef int                     INT4;
typedef short                   INT2;
typedef char                    INT1;
#define MINUTE 1
#define SECOND 0
void showVresion()
{
	printf("+---------------------------------+\n");
	printf("|                                 |\n");
	printf("|              GreenNet           |\n");
	printf("|              WatchDog           |\n");
	printf("|           Version 1.0.2         |\n");
    printf("|        For linghua 2U server    |\n");
	printf("|      compiled at %s    |\n",__DATE__);
	printf("|    author:zhengchong@GreenNet   |\n");
    printf("|                                 |\n");
	printf("+---------------------------------+\n");
}
int getRestime()
{
    UINT1 value=0;
    outb(0x87,0x2e);
	outb(0x87,0x2e); //enter CFG mode
	outb(0x07,0x2e);
    outb(0x08,0x2f); //select WAT LDN 08
    outb(0xF1,0x2e); //WDT time setting
	value=inl(0x2f);
	outb(0xF0,0x2e);
    outb(0xAA,0x2e);  //exit CFG mode
	return value;
}
int gettimemode()
{
    UINT1 value=0;
	outb(0x87,0x2e);
	outb(0x87,0x2e); //enter CFG mode
	outb(0x07,0x2e);
	outb(0x08,0x2f); //select WAT LDN 08
	outb(0xF0,0x2e); //WDT mode register
	value=inl(0x2f);
    outb(0xF0,0x2e);
    outb(0xAA,0x2e); //exit CFG mode
	return value;
}
int setRestime(UINT1 time)
{
	outb(0x87,0x2e);
	outb(0x87,0x2e); //enter CFG mode
	outb(0x07,0x2e);
	outb(0x08,0x2f); //select WAT LDN 08
	outb(0xF0,0x2e); //WDT mode register
    outb(0xf1,0x2e);  //WDT time setting
    outb(time,0x2f);  //update value
    outb(0xAA,0x2e);  //exit CFG mode
	return 0;
}


int settimemode(UINT1 mode)
{
    UINT1 value=0;
	outb(0x87,0x2e);
	outb(0x87,0x2e); //enter CFG mode
	outb(0x07,0x2e);
	outb(0x08,0x2f); //select WAT LDN 08
	outb(0xF0,0x2e); //WDT mode register
	value=inl(0x2f);
    printf("%X\n",value);
    value=(value&0xf7)|(mode<<3);
    printf("%X\n",value);
    outb(value,0x2f); //WDT time seting update
    //outl(0xf1,0x2f);  //WDT time setting
    outb(0xAA,0x2e);  //exit CFG mode
	return 0;
}
int setwatchfuntionon()
{
    UINT1 value=0;
	outb(0x87,0x2e);
	outb(0x87,0x2e); //enter CFG mode
	outb(0x07,0x2e);
	outb(0x08,0x2f); //select WAT LDN 08
	outb(0xF0,0x2e); //WDT mode register
	value=inl(0x2f);
    outb((value&0xef)|(1<<4),0x2f); //WDT time setting
    outb(0xAA,0x2e); //exit CFG mode
    return 0;
}
int setwatchfuntionoff()
{
    UINT1 value=0;
	outb(0x87,0x2e);
	outb(0x87,0x2e); //enter CFG mode
	outb(0x07,0x2e);
	outb(0x08,0x2f); //select WAT LDN 08
	outb(0xF0,0x2e); //WDT mode register
	value=inl(0x2f);
    outb((value&0xef),0x2f); //WDT time setting
    outb(0xAA,0x2e); //exit CFG mode
    return 0;
}
void showhelp()
{
    printf("\t\t-v  \n");
	printf("\t\t--help  \n");
	printf("\t\t-getrestime  \n");
    printf("\t\t-setrestime value[0 255] \n");
    printf("\t\t-gettimemode  \n");
    printf("\t\t-settimemode min/sec \n");
    printf("\t\t-setfunctionOFF  \n");
    printf("\t\t-setfunctionON \n");
	
}
int main(int argc,char *argv[])
{
    UINT4 value=0;
    value=ioperm(0x00,0xFF,1);
    if(value!=0)
        printf("%d\n",value);
	if(argc>=2
		&&*(UINT2*)argv[1]==*(UINT2* )"-v")
	{
		showVresion();
		return 0;
	}
	else if(argc>=2
		&&!strncmp(argv[1],"-getrestime",11))
	{
		value=getRestime();
        printf("%u\n",value&0xff);

	}
    else if(argc>=3
		&&!strncmp(argv[1],"-setrestime",11))
	{
	    value=atoi(argv[2]);
        printf("%d\n",value);
        if(value>255)
        {
            printf("please input value from 0 to 255\n");
            return 0;
        }
		setRestime((UINT1)(value&0xff));
	}
    else if(argc>=3
		&&!strncmp(argv[1],"-settimemode",12))
	{
	    if(!strncmp(argv[2],"min",3))
		    settimemode(1);
        else if(!strncmp(argv[2],"sec",3))
		    settimemode(0);
        else 
            showhelp();
		return 0;
	}
    else if(argc>=2
		&&!strncmp(argv[1],"-gettimemode",12))
	{
		value=gettimemode();
        if((value&0x08)==0)
            printf("second\n");
        else
        {
            printf("minute\n");
        }
	}
    else if(argc>=2
		&&!strncmp(argv[1],"-setfunctionOFF",15))
	{
		setRestime(0x0);
	}
    else if(argc>=2
		&&!strncmp(argv[1],"-setfunctionON",15))
	{
	    value=getRestime();
        if(value==0)
		    setRestime(0xff);
	}
	else
    {
        showhelp();
        return 0;
    }
    ioperm(0x00,0xFF,0);
	
	return 0;
}

