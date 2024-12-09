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

#ifndef SAMPLER_H
#define SAMPLER_H

#include <stdint.h>

#include <player_engine.h>
#include <temperament.h>
#include <list.h>

#include "codec_wav.h"

struct sampler {
	PlayerEngineClassDef *class_def;

//--- private engine data
	unsigned int position;
	unsigned int samplerate;
	float gain;
	bool autopan;
	List *events;
	CodecWav *codec_wav;
	Temperament *temperament;
};

typedef struct sampler Sampler;

Sampler *sampler_new(float gain, bool autopan, CodecWav *codec_wav, Temperament *temperament);
void sampler_free(Sampler *this);

#endif
