/*******************************************************/
/* file: ports.h                                       */
/* abstract:  This file contains extern declarations   */
/*            for providing stimulus to the JTAG ports.*/
/*******************************************************/

#ifndef ports_dot_h
#define ports_dot_h

#define GPIO_TMS  47 
#define GPIO_TDI  44
#define GPIO_TCK  45
#define GPIO_TDO  46

/* these constants are used to send the appropriate ports to setPort */
/* they should be enumerated types, but some of the microcontroller  */
/* compilers don't like enumerated types */
#define TCK (short) 0
#define TMS (short) 1
#define TDI (short) 2

void xsvf_setup(void);

/* setup ports for BeagleBone Black*/
void setupPort(void);

/* set the port "p" (TCK, TMS, or TDI) to val (0 or 1) */
void setPort(short p, short val);

int setFile(char *filename);

/* read the TDO bit and store it in val */
unsigned char readTDOBit();

/* make clock go down->up->down*/
void pulseClock();

/* read the next byte of data from the xsvf file */
void readByte(unsigned char *data);

void waitTime(long microsec);

#endif
