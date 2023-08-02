/*
 * main.h
 *
 *  Created on: Jul 12, 2023
 *      Author: GAMING
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#define FW_START 's'
#define FW_ACK   'a'
#define FW_ERR   'e'
#define FW_VER   'v'
#define VER_ERR "ERROR: Version\n"
#define CHUNK_SZ 2048
#define HOST_UART UART0_BASE
#define BOARD_UART UART1_BASE


#endif /* MAIN_H_ */
