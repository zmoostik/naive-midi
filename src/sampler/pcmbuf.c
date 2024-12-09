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

#include "pcmbuf.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

PcmbufClassDef pcmbuf_u8_def;
PcmbufClassDef pcmbuf_i16_def;
PcmbufClassDef pcmbuf_i32_def;

Pcmbuf *pcmbuf_new(size_t length, size_t sample_size, unsigned int channels)
{
	PcmbufClassDef *class_def = NULL;
	switch (sample_size) {
	case 1: 
		class_def = &pcmbuf_u8_def;
		break;
	case 2: 
		class_def = &pcmbuf_i16_def;
		break;
	case 4: 
		class_def = &pcmbuf_i32_def;
		break;
	default:
		return NULL;
	}

	Pcmbuf *this = malloc(sizeof(Pcmbuf));
	if (this == NULL) {
		return NULL;
	}
	this->data = calloc(length, sample_size * channels);
	if (this->data == NULL) {
		free(this);
		return NULL;
	}
	this->length = length;
	this->sample_size = sample_size;
	this->channels = channels;
	this->class_def = class_def;
	return this;
}

void pcmbuf_free(Pcmbuf *this)
{
	free(this->data);
	free(this);
}

static void *pcmbuf_get_ptr(Pcmbuf *this, unsigned int sample, unsigned int channel)
{
	if (sample >= this->length || channel >= this->channels) {
		return NULL;
	}
	size_t offset = ((sample * this->channels) + channel) * this->sample_size;
	return this->data + offset;
}

int pcmbuf_set(Pcmbuf *this, unsigned int sample, unsigned int channel, float value)
{
	void *ptr = pcmbuf_get_ptr(this, sample, channel);
	if (ptr == NULL) {
		return -1;
	}

	if (value > 1) value = 1;
	if (value < -1) value = -1;

	return this->class_def->set(this, ptr, value);
}

int pcmbuf_inc(Pcmbuf *this, unsigned int sample, unsigned int channel, float value)
{
	void *ptr = pcmbuf_get_ptr(this, sample, channel);
	if (ptr == NULL) {
		return -1;
	}

	return this->class_def->inc(this, ptr, value);
}

void pcmbuf_clear(Pcmbuf *this)
{
	return this->class_def->clear(this);
}

size_t pcmbuf_fwrite_interlaced_le(Pcmbuf *this, FILE *f)
{
	size_t size = this->length * this->sample_size * this->channels;
	char *buffer = malloc(size);
	if (buffer == NULL) {
		return -1;
	}

	size_t offset = 0;
	int ret = -1;
	for (size_t i = 0; i < this->length; i++) {
		for (unsigned int j = 0; j < this->channels; j++) {
			void *src = pcmbuf_get_ptr(this, i, j);
			if (src == NULL) {
				goto exit;
			}
			if (this->class_def->copy_le(this, src, buffer + offset) != 0) {
				goto exit;
			}
			offset += this->sample_size;
		}
	}

	if (fwrite(buffer, 1, size, f) != size) {
		goto exit;
	}
	ret = 0;
exit:
	free(buffer);
	return ret;
}

//------------- 8 bits PCM

#define U8_NEUTRAL 128
#define U8_AMPLITUDE 127
#define U8_MIN 0
#define U8_MAX 255

int pcmbuf_u8_set(Pcmbuf *this, uint8_t *ptr, float value)
{
	*ptr = U8_NEUTRAL + roundf(value * (float) U8_AMPLITUDE);
	return 0;
}

float pcmbuf_u8_get(Pcmbuf *this, uint8_t *ptr)
{
	return ((float) (*ptr) - (float) U8_NEUTRAL) / (float) U8_AMPLITUDE;
}

int pcmbuf_u8_copy(Pcmbuf *this, uint8_t *ptr, unsigned char *dst)
{
	*dst = *ptr;
	return 0;
}

int pcmbuf_u8_inc(Pcmbuf *this, uint8_t *ptr, float value)
{
	int i = *ptr + roundf(value * (float) U8_AMPLITUDE);
	if (i > U8_MAX) i = U8_MAX;
	if (i < U8_MIN) i = U8_MIN;
	*ptr = i;
	return 0;
}

int pcmbuf_u8_clear(Pcmbuf *this, uint8_t *ptr, float value)
{
	memset(this->data, U8_NEUTRAL, this->length * this->sample_size * this->channels);
}

PcmbufClassDef pcmbuf_u8_def = {
	.set = (PcmbufSetCB) pcmbuf_u8_set,
	.inc = (PcmbufIncCB) pcmbuf_u8_inc,
	.get = (PcmbufGetCB) pcmbuf_u8_get,
	.copy_le = (PcmbufCopyCB) pcmbuf_u8_copy,
	.clear = (PcmbufClearCB) pcmbuf_u8_clear
};

//------------- 16 bits PCM

#define I16_MAX 0x7fff
#define I16_MIN (-I16_MAX)

int pcmbuf_i16_set(Pcmbuf *this, int16_t *ptr, float value)
{
	*ptr = roundf(value * I16_MAX);
	return 0;
}

float pcmbuf_i16_get(Pcmbuf *this, int16_t *ptr)
{
	return (float) (*ptr) / (float) I16_MAX;
}

int pcmbuf_i16_copy_le(Pcmbuf *this, int32_t *ptr, unsigned char *dst)
{
	int16_t i16 = *ptr;
	dst[0] = (i16 >> 0) & 0xff;
	dst[1] = (i16 >> 8) & 0xff;
	return 0;
}

int pcmbuf_i16_inc(Pcmbuf *this, int16_t *ptr, float value)
{
	int i = *ptr + roundf(value * (float) I16_MAX);
	if (i > I16_MAX) i = I16_MAX;
	if (i < I16_MIN) i = I16_MIN;
	*ptr = i;
	return 0;
}

int pcmbuf_i16_clear(Pcmbuf *this, uint8_t *ptr, float value)
{
	bzero(this->data, this->length * this->sample_size * this->channels);
}

PcmbufClassDef pcmbuf_i16_def = {
	.set = (PcmbufSetCB) pcmbuf_i16_set,
	.inc = (PcmbufIncCB) pcmbuf_i16_inc,
	.get = (PcmbufGetCB) pcmbuf_i16_get,
	.copy_le = (PcmbufCopyCB) pcmbuf_i16_copy_le,
	.clear = (PcmbufClearCB) pcmbuf_i16_clear
};

//------------- 32 bits PCM

#define I32_MAX (0x7fffffff)
#define I32_MIN (-I32_MAX)

int pcmbuf_i32_set(Pcmbuf *this, int32_t *ptr, float value)
{
	*ptr = roundf(value * I32_MAX);
	return 0;
}

float pcmbuf_i32_get(Pcmbuf *this, int32_t *ptr)
{
	return (float) (*ptr) / (float) I32_MAX;
}

int pcmbuf_i32_copy_le(Pcmbuf *this, int32_t *ptr, unsigned char *dst)
{
	int32_t i32 = *ptr;
	dst[0] = (i32 >> 0) & 0xff;
	dst[1] = (i32 >> 8) & 0xff;
	dst[2] = (i32 >> 16) & 0xff;
	dst[3] = (i32 >> 24) & 0xff;
	return 0;
}

int pcmbuf_i32_inc(Pcmbuf *this, int32_t *ptr, float value)
{
	int i = *ptr + roundf(value * (float) I32_MAX);
	if (i > I32_MAX) i = I32_MAX;
	if (i < I32_MIN) i = I32_MIN;
	*ptr = i;
	return 0;
}

int pcmbuf_i32_clear(Pcmbuf *this, uint8_t *ptr, float value)
{
	bzero(this->data, this->length * this->sample_size * this->channels);
}

PcmbufClassDef pcmbuf_i32_def = {
	.set = (PcmbufSetCB) pcmbuf_i32_set,
	.inc = (PcmbufIncCB) pcmbuf_i32_inc,
	.get = (PcmbufGetCB) pcmbuf_i32_get,
	.copy_le = (PcmbufCopyCB) pcmbuf_i32_copy_le,
	.clear = (PcmbufClearCB) pcmbuf_i32_clear
};

