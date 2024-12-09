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

#ifndef CODEC_WAV
#define CODEC_WAV

#include <stddef.h>
#include <stdio.h>

#include "pcmbuf.h"

struct codec_wav {
	int sample_size;
	int channels;
	int sample_rate;
	size_t length;
	char const *filename;
	FILE *f;
};

typedef struct codec_wav CodecWav;

CodecWav *codec_wav_new(int sample_size, int channels, int sample_rate, char const *filename);
void codec_wav_free(CodecWav *this);
int codec_wav_open(CodecWav *this);
int codec_wav_close(CodecWav *this);
int codec_wav_write_pcmbuf(CodecWav *this, Pcmbuf *pcmbuf);

#endif

