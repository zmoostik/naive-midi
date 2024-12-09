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

#ifndef ENDIANNES_H
#define ENDIANNES_H

#include <stdint.h>

typedef uint8_t le32_t[4];
typedef uint8_t le16_t[2];

inline static uint32_t le32_get(le32_t *this)
{
	return ((uint32_t) (*this)[0] << 0) |
		((uint32_t) (*this)[1] << 8) |
		((uint32_t) (*this)[2] << 16) |
		((uint32_t) (*this)[3] << 24);
}

inline static void le32_set(le32_t *this, uint32_t value)
{
	(*this)[0] = (value >> 0) & 0xff;
	(*this)[1] = (value >> 8) & 0xff;
	(*this)[2] = (value >> 16) & 0xff;
	(*this)[3] = (value >> 24) & 0xff;
}

inline static uint16_t le16_get(le16_t *this)
{
	return ((uint16_t) (*this)[0] << 0) |
		((uint16_t) (*this)[1] << 8);
}

inline static void le16_set(le16_t *this, uint16_t value)
{
	(*this)[0] = (value >> 0) & 0xff;
	(*this)[1] = (value >> 8) & 0xff;
}

#endif
