#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <inttypes.h>
#include <string.h>
#include "lib_erreur.h"

#include "lib_serial.h"

int uart0_filestream = -1;

int lib_serial_init(char* portname)
{
    uart0_filestream = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);

    if (uart0_filestream == -1)
    {
		lib_erreur(0,0,"Erreur ouverture port");
		return 0;
    }
	return 1;
}

void lib_serial_config(void)
{
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);
}

int lib_serial_println(const char *line, int len)
{
    if (uart0_filestream != -1) {
        char *cpstr = (char *)malloc((len+1) * sizeof(char));
        strcpy(cpstr, line);
        cpstr[len-1] = '\r';
        cpstr[len] = '\n';

        int count = write(uart0_filestream, cpstr, len+1);
        if (count < 0) {
            lib_erreur(0,0,"Erreur ecriture port");
			return 0;
        }
        free(cpstr);
		return 1;
    }
	else
	{
		lib_erreur(0,0,"Impossible d'ecrire: Port non defini");
		return 0;
	}
	
}

// Read a line from UART.
// Return a 0 len string in case of problems with UART
int lib_serial_readln(char *buffer, int len)
{
    char c;
    char *b = buffer;
    int rx_length = -1;
	int nbchar=0;
	if(uart0_filestream!=-1)
	{
		while(nbchar<len-1) 
		{
			rx_length = read(uart0_filestream, (void*)(&c), 1);

			if (rx_length <= 0) {
				//wait for messages
				sleep(1);
			} else {
				if (c == '\n') {
					*b++ = '\0';
					break;
				}
				*b++ = c;
				nbchar++;
			}
		}
		return 1;
	}
	else
	{
		lib_erreur(0,0,"Impossible de lire: Port non defini");
		return 0;
	}
}

void lib_serial_close(void)
{
    close(uart0_filestream);
}

