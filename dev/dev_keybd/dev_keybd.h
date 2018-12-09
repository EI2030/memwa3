/*
 * memwa2 keyboard utility
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


#ifndef _DEV_KEYBD_H
#define _DEV_KEYBD_H

#include "stm32h7xx_hal.h"
#include "dev_term.h"
#include "if.h"

typedef enum
{
    KEYBD_STATE_RELEASED,
    KEYBD_STATE_PRESSED
} keybd_state_t;

void dev_keybd_init();
void dev_keybd_poll();
keybd_state_t dev_keybd_key_state();
keybd_state_t dev_keybd_get_shift_state();
keybd_state_t dev_keybd_get_ctrl_state();
uint8_t dev_keybd_get_active_key();
uint8_t *dev_keybd_get_active_keys();
uint8_t dev_keybd_get_active_keys_hash();
uint8_t dev_keybd_get_active_number_of_keys();
uint8_t dev_keybd_get_active_ascii_key();
if_keybd_map_t *dev_keybd_get_default_map();
void dev_keybd_populate_map(uint8_t *conf_text, if_keybd_map_t *keybd_map_p);

#endif
