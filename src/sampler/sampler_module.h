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

#ifndef SAMPLER_MODULE_H
#define SAMPLER_MODULE_H

#include <module.h>

#include "codec_wav.h"
#include "sampler.h"

#define SAMPLER_DEFAULT_CHANNELS 2
#define SAMPLER_DEFAULT_SAMPLE_RATE 44100
#define SAMPLER_DEFAULT_SAMPLE_SIZE 4
#define SAMPLER_DEFAULT_GAIN 0.2

struct sampler_module {
	ModuleClassDef *class_def;

//--- private module data
	char const *output;
	int channels;
	float gain;
	int samplerate;
	int samplesize;
	bool autopan;
	CodecWav *codec_wav;
	Sampler *sampler;
};

typedef struct sampler_module SamplerModule;

SamplerModule *sampler_module_new(void);

#endif
