#ifndef BUS_BUF_H
#define BUS_BUF_H // what the hell am i doing with my life
	
#include <stddef.h>
#include <stdint.h>
	
struct bus_buf {
	uint8_t *data;
	size_t size;
};

#endif //BUS_BUF_H
