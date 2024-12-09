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

#include "sampler_event.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <common.h>

#include "instrument.h"

SamplerEvent *sampler_event_new(Sampler *sampler, unsigned int position, int channel, int note, int velocity, float gain, float pan)
{
	SamplerEvent *this = malloc(sizeof(SamplerEvent));
	this->sampler = sampler;

	float fondamental = temperament_get_freq(sampler->temperament, note);
	this->instrument = instrument_new(fondamental, velocity, &instrument_piano);

	this->note = note;
	this->position = position;
	this->duration = 0;
	this->channel = channel;
	this->velocity = velocity;
	this->ending = false;
	this->terminated = false;
	this->gain = gain;
	this->pan = pan;
	return this;
}

void sampler_event_free(SamplerEvent *this)
{
	instrument_free(this->instrument);
	free(this);
}

void sampler_event_render(SamplerEvent *this, unsigned long position, Pcmbuf *pcmbuf)
{
	float duration = this->ending == true ? this->duration : -1;

	for (unsigned int i = 0; i < pcmbuf->length; i++)  {
		float t = (float) (position - this->position + i) / (float) this->sampler->samplerate;

		float value = instrument_compute(this->instrument, duration, t, &this->terminated);

		if (this->terminated == true) {
			break;
		}
		value *= this->gain;

		if (this->pan >= 0 && pcmbuf->channels == 2) {
			pcmbuf_inc(pcmbuf, i, 0, value * (1 - this->pan));
			pcmbuf_inc(pcmbuf, i, 1, value * this->pan);
		}
		else {
			int output_channel = this->channel % pcmbuf->channels;
			pcmbuf_inc(pcmbuf, i, output_channel, value);
		}
	}
}

void sampler_event_set_end(SamplerEvent *this, unsigned long end)
{
	this->duration = (end - this->position) / (float) this->sampler->samplerate;
	this->ending = true;
}

