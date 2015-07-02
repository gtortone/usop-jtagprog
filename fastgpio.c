#include "fastgpio.h"
#include "gpio.h"

static volatile uint32_t *map;
static char mapped = FALSE;

int fastgpio_init() {
	if(!mapped) {
		int fd;
		fd = open("/dev/mem", O_RDWR | O_SYNC);
		if(fd == -1) { 
			perror("Unable to open /dev/mem");
			exit(EXIT_FAILURE);
		}
		map = (uint32_t*)mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, MMAP_OFFSET);
		if(map == MAP_FAILED) {
			close(fd);
			perror("Unable to map /dev/mem");
			exit(EXIT_FAILURE);
		}
		mapped = TRUE;
	}
	return mapped;
}

int fastgpio_export(PIN p) {
   return(gpio_export(p.gpio));
}

int fastgpio_unexport(PIN p) {
   return(gpio_unexport(p.gpio));
}

int fastgpio_set_dir(PIN p, unsigned int out_flag) {
   return(gpio_set_dir(p.gpio, out_flag));
}

int fastgpio_set_value(PIN p, unsigned int value) {
	
   if(!mapped)
      fastgpio_init();
   
	map[(p.gpio_bank-MMAP_OFFSET+GPIO_OE)/4] &= ~(1<<p.bank_id);
	if(value == HIGH) 
      map[(p.gpio_bank-MMAP_OFFSET+GPIO_DATAOUT)/4] |= (1<<p.bank_id);
   else 
      map[(p.gpio_bank-MMAP_OFFSET+GPIO_DATAOUT)/4] &= ~(1<<p.bank_id);

	return 1;
}

int fastgpio_get_value(PIN p, unsigned int *value) {
	
   if(!mapped)
      fastgpio_init();
   
   *value = (map[(p.gpio_bank-MMAP_OFFSET+GPIO_DATAIN)/4] & (1<<p.bank_id)) >> p.bank_id;
	return 0;
}

int adc_init() {
	fastgpio_init();

	// enable the CM_WKUP_ADC_TSC_CLKCTRL with CM_WKUP_MODUELEMODE_ENABLE
	map[(CM_WKUP_ADC_TSC_CLKCTRL-MMAP_OFFSET)/4] |= CM_WKUP_MODULEMODE_ENABLE;

	// wait for the enable to complete
	while(!(map[(CM_WKUP_ADC_TSC_CLKCTRL-MMAP_OFFSET)/4] & CM_WKUP_MODULEMODE_ENABLE)) {
		// waiting for adc clock module to initialize
		//printf("Waiting for CM_WKUP_ADC_TSC_CLKCTRL to enable with MODULEMODE_ENABLE\n");
	}
	// software reset, set bit 1 of sysconfig high?
	// make sure STEPCONFIG write protect is off
	map[(ADC_CTRL-MMAP_OFFSET)/4] |= ADC_STEPCONFIG_WRITE_PROTECT_OFF;

	// set up each ADCSTEPCONFIG for each ain pin
	map[(ADCSTEPCONFIG1-MMAP_OFFSET)/4] = 0x00<<19 | ADC_AVG16;
	map[(ADCSTEPDELAY1-MMAP_OFFSET)/4]  = (0x0F)<<24;
	map[(ADCSTEPCONFIG2-MMAP_OFFSET)/4] = 0x01<<19 | ADC_AVG16;
	map[(ADCSTEPDELAY2-MMAP_OFFSET)/4]  = (0x0F)<<24;
	map[(ADCSTEPCONFIG3-MMAP_OFFSET)/4] = 0x02<<19 | ADC_AVG16;
	map[(ADCSTEPDELAY3-MMAP_OFFSET)/4]  = (0x0F)<<24;
	map[(ADCSTEPCONFIG4-MMAP_OFFSET)/4] = 0x03<<19 | ADC_AVG16;
	map[(ADCSTEPDELAY4-MMAP_OFFSET)/4]  = (0x0F)<<24;
	map[(ADCSTEPCONFIG5-MMAP_OFFSET)/4] = 0x04<<19 | ADC_AVG16;
	map[(ADCSTEPDELAY5-MMAP_OFFSET)/4]  = (0x0F)<<24;
	map[(ADCSTEPCONFIG6-MMAP_OFFSET)/4] = 0x05<<19 | ADC_AVG16;
	map[(ADCSTEPDELAY6-MMAP_OFFSET)/4]  = (0x0F)<<24;
	map[(ADCSTEPCONFIG7-MMAP_OFFSET)/4] = 0x06<<19 | ADC_AVG16;
	map[(ADCSTEPDELAY7-MMAP_OFFSET)/4]  = (0x0F)<<24;
	map[(ADCSTEPCONFIG8-MMAP_OFFSET)/4] = 0x07<<19 | ADC_AVG16;
	map[(ADCSTEPDELAY8-MMAP_OFFSET)/4]  = (0x0F)<<24;

	// enable the ADC
	map[(ADC_CTRL-MMAP_OFFSET)/4] |= 0x01;
 
        return(0);
}

int analogRead(PIN p) {
	fastgpio_init();
	
	// the clock module is not enabled
	if(map[(CM_WKUP_ADC_TSC_CLKCTRL-MMAP_OFFSET)/4] & CM_WKUP_IDLEST_DISABLED)
		adc_init();
	
	// enable the step sequencer for this pin
	map[(ADC_STEPENABLE-MMAP_OFFSET)/4] |= (0x01<<(p.bank_id+1));

	// return the the FIFO0 data register
	return map[(ADC_FIFO0DATA-MMAP_OFFSET)/4] & ADC_FIFO_MASK;
}

