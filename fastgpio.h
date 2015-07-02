#ifndef _FASTGPIO_H_
#define _FASTGPIO_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include "am335x.h"

#define HIGH (1)
#define LOW  (0)

#define OUT 1
#define IN  0

int fastgpio_init(); /*!< mmap /dev/mem into memory */
int fastgpio_export(PIN p);
int fastgpio_unexport(PIN p);
int fastgpio_set_dir(PIN p, unsigned int out_flag);
int fastgpio_set_value(PIN p, unsigned int value);
int fastgpio_get_value(PIN p, unsigned int *value);
int analogRead(PIN p);

#endif