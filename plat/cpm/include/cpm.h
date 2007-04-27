/*
 * unistd.h - standard system calls
 */
/* $Id$ */

#ifndef _CPM_H
#define _CPM_H

#include <stdint.h>

/* These interface provides a very bare-bones interface to the CP/M BDOS. Set
 * the following four variables as you wish, call cpm_bdos(), and the contents
 * of the variables will have been updated accordingly. */
 
extern uint8_t cpm_a_register;
extern uint16_t cpm_bc_register;
extern uint16_t cpm_de_register;
extern uint16_t cpm_hl_register;

extern void cpm_bdos(void);

/* Describes the available CP/M BDOS calls. They're a fairly conservative set
 * taken from the CP/M 2.0 manual. */

enum
{
	CPM_BDOS_SYSTEM_RESET,
	CPM_BDOS_CONSOLE_INPUT,
	CPM_BDOS_CONSOLE_OUTPUT,
	CPM_BDOS_READER_INPUT,
	CPM_BDOS_PUNCH_OUTPUT,
	CPM_BDOS_LIST_OUTPUT,
	CPM_BDOS_CONSOLE_IO,
	CPM_BDOS_GET_IO_BYTE,
	CPM_BDOS_SET_IO_BYTE,
	CPM_BDOS_PRINT_STRING,
	CPM_BDOS_READ_CONSOLE_BUFFER,
	CPM_BDOS_GET_CONSOLE_STATUS,
	CPM_BDOS_GET_VERSION_NUMBER,
	CPM_BDOS_RESET_DISK_SYSTEM,
	CPM_BDOS_SELECT_DISK,
	CPM_BDOS_OPEN_FILE,
	CPM_BDOS_CLOSE_FILE,
	CPM_BDOS_SEARCHFIRST,
	CPM_BDOS_SEARCHNEXT,
	CPM_BDOS_DELETE_FILE,
	CPM_BDOS_READ_SEQ,
	CPM_BDOS_WRITE_SEQ,
	CPM_BDOS_MAKE_FILE,
	CPM_BDOS_RENAME_FILE,
	CPM_BDOS_GET_LOGIN_VECTOR,
	CPM_BDOS_GET_CURRENT_DISK,
	CPM_BDOS_SET_DMA_ADDRESS,
	CPM_BDOS_GET_ALLOC_VECTOR,
	CPM_BDOS_WRITE_PROTECT,
	CPM_BDOS_GET_RO_VECTOR,
	CPM_BDOS_SET_FILE_ATTR,
	CPM_BDOS_GET_DISK_PARMS,
	CPM_BDOS_SETGET_USER,
	CPM_BDOS_READ_RANDOM,
	CPM_BDOS_WRITE_RANDOM,
	CPM_BDOS_GET_FILE_SIZE,
	CPM_BDOS_SET_RANDOM
};

#endif
