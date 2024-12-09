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
 
#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

void print_error(const char *fmt, ...);

extern bool sig_int;

#define DEBUG_POINT printf("%s():%d\n", __FUNCTION__, __LINE__);

//#define DEBUG

#ifdef DEBUG
#define DMSG(msg, ...) printf("DEBUG: " msg "\n", ##__VA_ARGS__)
#else
#define DMSG(msg, ...) ;
#endif

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

