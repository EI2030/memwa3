/*
 * Term
 *
 * Copyright (c) 2016 Mathias Edman <mail@dicetec.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 * USA
 *
 */


#ifndef _DEV_TERM_H
#define _DEV_TERM_H

#include "stm32h7xx_hal.h"
#include <string.h>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define dev_term_printf(type, string, args...) \
	switch(type) \
	{ \
		case DEV_TERM_PRINT_TYPE_INFO: \
			printf("I[%s:%d]: ", __FILENAME__, __LINE__); \
			break; \
		case DEV_TERM_PRINT_TYPE_WARNING: \
			printf("W[%s:%d]: ", __FILENAME__, __LINE__); \
			break; \
		case DEV_TERM_PRINT_TYPE_ERROR: \
			printf("E[%s:%d]: ", __FILENAME__, __LINE__); \
			break; \
		case DEV_TERM_PRINT_TYPE_DEBUG: \
			printf("D[%s:%d]: ", __FILENAME__, __LINE__); \
			break; \
	} \
	printf(string, ##args); \
	printf("\n");

	    /*stage_set_message(string_p);
	    if(sm_get_state() == SM_STATE_EMULATOR && sm_get_ltdc_stats_flag())
	    {
	        stage_draw_info(INFO_PRINT, 0   );
	    }*/

typedef enum
{
	DEV_TERM_PRINT_TYPE_INFO,
	DEV_TERM_PRINT_TYPE_WARNING,
	DEV_TERM_PRINT_TYPE_ERROR,
	DEV_TERM_PRINT_TYPE_DEBUG
} dev_term_print_type_t;

void dev_term_init();

#endif