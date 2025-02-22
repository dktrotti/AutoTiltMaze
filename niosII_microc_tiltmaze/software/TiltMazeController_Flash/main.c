/*
 * main.c
 *
 * The following code is based on hello_ucosii.c from the Altera
 * Hello World UCOSII template.
 *
 *  Created on: Feb 7, 2017
 *      Author: dktrotti
 */


#include <stdio.h>
#include <stdlib.h>
#include "includes.h"
#include "uart.h"
#include "command.h"

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK    task_uart_stk[TASK_STACKSIZE];
OS_STK    task_main_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define TASK_UART_PRIORITY      2
#define TASK_MAIN_PRIORITY      1

/* Definition of global variables */
OS_EVENT *cmdQ;
void *cmdarr[16];

void maintask(void* pdata) {
	INT8U err;
	cmd_t command_type;
	INT16U command_arg;
	char *buf;
	char resp_buf[CMD_LEN + CMD_ARGLEN]; // Buffer for query responses

	// TODO: Remove these variables
	INT16U tempx;
	INT16U tempy;

	while (1) {
		buf = (char *) OSQPend(cmdQ, 0, &err);

		if (err != OS_NO_ERR) {
			printf("Queue pend error\n");
		}

		command_type = process_cmd_buf(buf, &command_arg);

		switch (command_type) {
		case PING:
			uart_write(ACK_STR, ACK_LEN);
			printf("Received PING\n");
			break;
		case SET_X_ACC:
			tempx = command_arg;
			uart_write(ACK_STR, ACK_LEN);
			printf("Received SET_X_ACC with arg: %d\n", command_arg);
			break;
		case SET_Y_ACC:
			tempy = command_arg;
			uart_write(ACK_STR, ACK_LEN);
			printf("Received SET_Y_ACC with arg: %d\n", command_arg);
			break;
		case GET_X_ACC:
			//sprintf(resp_buf, "%s%c%c", CMD_GETX, (char) ((tempx & 0xff00) >> 8), (char) (tempx & 0x00ff));
			sprintf(resp_buf, "%s%.4x", CMD_GETX, tempx);
			uart_write(resp_buf, CMD_LEN + CMD_ARGLEN);
			printf("Received GET_X_ACC\n");
			break;
		case GET_Y_ACC:
			//sprintf(resp_buf, "%s%c%c", CMD_GETY, (char) ((tempy & 0xff00) >> 8), (char) (tempy & 0x00ff));
			sprintf(resp_buf, "%s%.4x", CMD_GETY, tempy);
			uart_write(resp_buf, CMD_LEN + CMD_ARGLEN);
			printf("Received GET_Y_ACC\n");
			break;
		case INVALID:
			uart_write(NACK_STR, NACK_LEN);
			printf("Received INVALID\n");
			break;
		}

		free(buf);
	}
}

int main(void)
{
	printf("Hello world\n");

	cmdQ = OSQCreate(&cmdarr[0], 16);

	uart_init(cmdQ);

	OSTaskCreateExt(uarttask,
			NULL,
			(void *)&task_uart_stk[TASK_STACKSIZE-1],
			TASK_UART_PRIORITY,
			TASK_UART_PRIORITY,
			task_uart_stk,
			TASK_STACKSIZE,
			NULL,
			0);

	OSTaskCreateExt(maintask,
			NULL,
			(void *)&task_main_stk[TASK_STACKSIZE-1],
			TASK_MAIN_PRIORITY,
			TASK_MAIN_PRIORITY,
			task_main_stk,
			TASK_STACKSIZE,
			NULL,
			0);

	OSStart();
	return 0;
}
