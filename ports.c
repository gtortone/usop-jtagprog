/*******************************************************/
/* file: ports.c                                       */
/* abstract:  This file contains the routines to       */
/*            output values on the JTAG ports, to read */
/*            the TDO bit, and to read a byte of data  */
/*            from the prom                            */
/* Revisions:                                          */
/* 12/01/2008:  Same code as before (original v5.01).  */
/*              Updated comments to clarify instructions.*/
/*              Add print in setPort for xapp058_example.exe.*/
/*******************************************************/
#include "ports.h"
#include "fastgpio.h"

#include <stdio.h>
#include <unistd.h>

#define MAX_BUFFER 10000
static unsigned char buf[MAX_BUFFER]; //buffer to hold incoming bytes
static unsigned int bufBytes = 0, bufPointer = 0;
static unsigned long int rdBytes = 0;
FILE *in;

#define JTAGDATASETTLE  2

void xsvf_setup(void) {
 
   unsigned int i;
   
   // JTAG reset
   setPort(TMS, 1);
   usleep(JTAGDATASETTLE);
	
   for(i=0; i<10; i++) {
      setPort(TCK, 1);
      setPort(TCK, 0);
   }
   
   setPort(TMS, 0);
   usleep(JTAGDATASETTLE);
   
   setPort(TDI, 0);
   setPort(TCK, 0);
   setPort(TMS, 0);
   
   bufBytes = 0;
   rdBytes = 0;
   
   setPort(TMS, 1);
}

/* setup ports for BeagleBone Black*/
void setupPort(void) {
   
   fastgpio_export(JTAG_TMS);
   fastgpio_export(JTAG_TDI);
   fastgpio_export(JTAG_TCK);
   fastgpio_export(JTAG_TDO);
   
   fastgpio_set_dir(JTAG_TMS, OUT);
   fastgpio_set_dir(JTAG_TDI, OUT);
   fastgpio_set_dir(JTAG_TCK, OUT);
   fastgpio_set_dir(JTAG_TDO, IN);
   
/*
   gpio_export(GPIO_TMS);
   gpio_export(GPIO_TDI);
   gpio_export(GPIO_TCK);
   gpio_export(GPIO_TDO);
   
   gpio_set_dir(GPIO_TMS, OUT);
   gpio_set_dir(GPIO_TDI, OUT);
   gpio_set_dir(GPIO_TCK, OUT);
   gpio_set_dir(GPIO_TDO, IN);
*/
}

/* setPort:  Implement to set the named JTAG signal (p) to the new value (v).*/
/* if in debugging mode, then just set the variables */
void setPort(short p,short val)
{
    /* Printing code for the xapp058_example.exe.  You must set the specified
       JTAG signal (p) to the new value (v).  See the above, old Win95 code
       as an implementation example. */
   
    if (p==TMS)
       fastgpio_set_value(JTAG_TMS, val);
    else if (p==TDI)
       fastgpio_set_value(JTAG_TDI, val);
    else if (p==TCK)
       fastgpio_set_value(JTAG_TCK, val);

/*
    if (p==TMS)
       gpio_set_value(GPIO_TMS, val);
    else if (p==TDI)
       gpio_set_value(GPIO_TDI, val);
    else if (p==TCK)
       gpio_set_value(GPIO_TCK, val);
*/
}


int setFile(char *filename) {
   
   in = fopen(filename, "rb");
   return(errno);
}

/* toggle tck LH.  No need to modify this code.  It is output via setPort. */
void pulseClock()
{
    setPort(TCK,0);  /* set the TCK port to low  */
    setPort(TCK,1);  /* set the TCK port to high */
}

/* readByte:  Implement to source the next byte from your XSVF file location */
/* read in a byte of data from the prom */
void readByte(unsigned char *data)
{
   if (bufBytes == 0) {
      bufBytes  = fread((unsigned char *) buf, sizeof(unsigned char), MAX_BUFFER, in);
      bufBytes /= sizeof(unsigned char);
      rdBytes += bufBytes;
      printf("\n\rread bytes: %ld", rdBytes);
      bufPointer = 0;
   }

   (*data) = buf[bufPointer];
   
   bufPointer++;
   bufBytes--;
}

unsigned char readTDOBit()
{
    /* You must return the current value of the JTAG TDO signal. */
   unsigned int val;
   /* gpio_get_value(GPIO_TDO, &val); */
   fastgpio_get_value(JTAG_TDO, &val);
  
   return(val);
}

/* waitTime:  Implement as follows: */
/* REQUIRED:  This function must consume/wait at least the specified number  */
/*            of microsec, interpreting microsec as a number of microseconds.*/
/* REQUIRED FOR SPARTAN/VIRTEX FPGAs and indirect flash programming:         */
/*            This function must pulse TCK for at least microsec times,      */
/*            interpreting microsec as an integer value.                     */
/* RECOMMENDED IMPLEMENTATION:  Pulse TCK at least microsec times AND        */
/*                              continue pulsing TCK until the microsec wait */
/*                              requirement is also satisfied.               */
void waitTime(long microsec)
{
/*
    static long tckCyclesPerMicrosec    = 1;
    long        tckCycles   = microsec * tckCyclesPerMicrosec;
    long        i;
*/
    
    while(--microsec > 0) {
       setPort(TCK,0);
       /* usleep(1); */
       setPort(TCK,1);
       /* usleep(1); */
    }

    /* This implementation is highly recommended!!! */
    /* This implementation requires you to tune the tckCyclesPerMicrosec 
       variable (above) to match the performance of your embedded system
       in order to satisfy the microsec wait time requirement. */
/*
    for ( i = 0; i < tckCycles; ++i )
    {
        pulseClock();
    }
*/

#if 0
    /* Alternate implementation */
    /* For systems with TCK rates << 1 MHz;  Consider this implementation. */
    /* This implementation does not work with Spartan-3AN or indirect flash
       programming. */
    if ( mvicrosec >= 50L )
    {
        /* Make sure TCK is low during wait for XC18V00/XCFxxS */
        /* Or, a running TCK implementation as shown above is an OK alternate */
        setPort( TCK, 0 );

        /* Use Windows Sleep().  Round up to the nearest millisec */
        _sleep( ( microsec + 999L ) / 1000L );
    }
    else    /* Satisfy FPGA JTAG configuration, startup TCK cycles */
    {
        for ( i = 0; i < microsec;  ++i )
        {
            pulseClock();
        }
    }
#endif

#if 0
    /* Alternate implementation */
    /* This implementation is valid for only XC9500/XL/XV, CoolRunner/II CPLDs, 
       XC18V00 PROMs, or Platform Flash XCFxxS/XCFxxP PROMs.  
       This implementation does not work with FPGAs JTAG configuration. */
    /* Make sure TCK is low during wait for XC18V00/XCFxxS PROMs */
    /* Or, a running TCK implementation as shown above is an OK alternate */
    setPort( TCK, 0 );
    /* Use Windows Sleep().  Round up to the nearest millisec */
    _sleep( ( microsec + 999L ) / 1000L );
#endif
}
