#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <inttypes.h>



int lib_serial_init(char* portname);
void lib_serial_config(void);
int lib_serial_println(const char *line, int len);
int lib_serial_readln(char *buffer, int len);
void lib_serial_close(void);

#endif
