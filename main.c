#include <stdio.h>

#include "ports.h"
#include "micro.h"

int main(int argc, char **argv) {
	
   int xsvferrcode = 0;
   
   printf("BeagleBone Black JTAG programming utility v0.5\n\n\r");
   
   if(setFile(argv[1])) {
      
      printf("file open error !\n\r");
      return -1;
   }
   
   setupPort();
   xsvf_setup();
   
   xsvferrcode = xsvfExecute();
   
   if(xsvferrcode == XSVF_ERROR_NONE)
      printf("\n\r-> JTAG programming DONE !\n\r");
   else
      printf("\n\r->JTAG programming FAILED ! - XSVF errocode = %d\n\r", xsvferrcode);
   
	return 0;
}
