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

#ifndef SAMPLER_EVENT_H
#define SAMPLER_EVENT_H

#include <stdint.h>
#include <stdbool.h>

#include "pcmbuf.h"
#include "sampler.h"
#include "instrument.h"

struct sampler_event {
	Sampler *sampler;
	Instrument *instrument;
	unsigned int position;
	float duration;
	float gain;
	int channel;
	int note;
	int velocity;
	bool ending;
	bool terminated;
	float pan;
};

typedef struct sampler_event SamplerEvent;

SamplerEvent *sampler_event_new(Sampler *sampler, unsigned int position, int channel, int note, int velocity, float gain, float pan);
void sampler_event_free(SamplerEvent *this);
void sampler_event_render(SamplerEvent *this, unsigned long position, Pcmbuf *pcmbuf);
void sampler_event_set_end(SamplerEvent *this, unsigned long end);

#endif
