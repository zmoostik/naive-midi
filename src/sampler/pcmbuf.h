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

#ifndef PCMBUF_H
#define PCMBUF_H

#include <stddef.h>
#include <stdio.h>

struct pcmbuf;
typedef struct pcmbuf Pcmbuf;

typedef int (*PcmbufSetCB)(Pcmbuf *this, void *ptr, float value);
typedef int (*PcmbufIncCB)(Pcmbuf *this, void *ptr, float value);
typedef float (*PcmbufGetCB)(Pcmbuf *this, void *ptr);
typedef int (*PcmbufCopyCB)(Pcmbuf *this, void *ptr, unsigned char *dst);
typedef void (*PcmbufClearCB)(Pcmbuf *this);

struct pcmbuf_class_def {
	PcmbufSetCB set;
	PcmbufGetCB get;
	PcmbufCopyCB copy_le;
	PcmbufIncCB inc;
	PcmbufClearCB clear;
};

typedef struct pcmbuf_class_def PcmbufClassDef;

struct pcmbuf {
	PcmbufClassDef *class_def;
	
	size_t length;
	size_t sample_size;
	float max_value;
	unsigned int channels;
	void *data;
};


Pcmbuf *pcmbuf_new(size_t length, size_t sample_size, unsigned int channels);
void pcmbuf_free(Pcmbuf *this);
int pcmbuf_set(Pcmbuf *this, unsigned int sample, unsigned int channel, float value);
int pcmbuf_inc(Pcmbuf *this, unsigned int sample, unsigned int channel, float value);
void pcmbuf_clear(Pcmbuf *this);
size_t pcmbuf_fwrite_interlaced_le(Pcmbuf *this, FILE *f);

#endif

