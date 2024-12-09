/* 
 * This file is part of naive-midi-player.
 * Copyright (c) 2024 VION Nicolas.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STREAMBUF_H
#define STREAMBUF_H

#include <stdint.h>
#include <stddef.h>

struct streambuf {
	uint8_t *data;
	size_t read_offset;
	size_t size;
};

typedef struct streambuf Streambuf;

Streambuf *streambuf_new(void);
void streambuf_free(Streambuf *this);
int streambuf_open(Streambuf *this, char const *filename);
int streambuf_read_u8(Streambuf *this, uint8_t *value);
int streambuf_read_u16(Streambuf *this, uint16_t *value);
int streambuf_read_u32(Streambuf *this, uint32_t *value);

#endif
