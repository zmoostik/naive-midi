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

#include "instrument.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <common.h>
#include <temperament.h>

Instrument *instrument_new(float fondamental, int velocity, InstrumentProfile *profile)
{
	Instrument *this = malloc(sizeof(Instrument));
	this->fondamental = fondamental;
	this->velocity = velocity;
	this->profile = profile;
	return this;
}

void instrument_free(Instrument *this)
{
	free(this);
}

float instrument_compute_partial(Instrument *this, int n, float t)
{
	float freq = this->fondamental * (float) n;
	return sin(t * freq * 2.0 * M_PI);
}

//--- ADSR helper functions
static float f(float x, float c)
{
	return (powf(2.0, 16.0 * c * x) - 1.0) / (powf(2.0, 16.0 * c) - 1.0);
}

static float a(InstrumentProfile *profile, float x) {
	return f(x / profile->a0, profile->a1);
}

static float d(InstrumentProfile *profile, float x) {
	return 1.0 - f(x / profile->d0, profile->d1) * (1.0 - profile->s1);
}

static float ds(InstrumentProfile *profile, float x)
{
	return x < profile->d0
		? d(profile, x)
		: profile->s1;
}

static float r(InstrumentProfile *profile, float s, float x)
{
	return s * (1.0 - f(x / profile->r0, profile->r1));
}

float instrument_compute_envelope(Instrument *this, float duration, float t)
{
	InstrumentProfile *profile = this->profile;

	//- Attack
	if (t < profile->a0) {
		return a(profile, t); 
	}

	if (duration > 0) {
		//-- Release
		float min_duration = profile->a0 + profile->ds0;
		if (duration < min_duration) {
			duration = min_duration;
		}

		if (t > duration) {
			float dt = t - duration;
			return (dt < profile->r0)
				? r(profile, ds(profile, duration), dt)
				: -1;
		}
	}

	//-- Decay, Sustain
	return ds(profile, t - profile->a0);
}

float instrument_compute(Instrument *this, float duration, float t, bool *terminated)
{
	float envelope = instrument_compute_envelope(this, duration, t);
	if (envelope < 0) {
		*terminated = true;
		return 0;
	}

	float value = 0;
	for (int j = 0; j < ARRAY_SIZE(this->profile->spectrum); j++) {
		float gain = this->profile->spectrum[j] * (float) this->velocity / 127.0;
		if (gain > 0) {
			value += instrument_compute_partial(this, j + 1, t) * gain;
		}
	}
	return value * envelope;
}

struct instrument_profile instrument_organ = {
	.spectrum = { 0.8, 0.8, 0.8, 1, 0.8, 0.8, 0.6, 0.4, 0.2, 0.1 },
	.a0 = 0.01,
	.a1 = -0.45,
	.d0 = 0.1,
	.ds0 = 0,
	.d1 = -0.48,
	.s1 = 0.8,
	.r0 = 0.5,
	.r1 = -0.5
};

struct instrument_profile instrument_harp = {
	.spectrum = { 1, 0.6, 0.4, 0.2, 0.1, 0.05, 0.01, 0.005, 0.002, 0.001, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	.a0 = 0.02,
	.a1 = -0.45,
	.d0 = 1,
	.ds0 = 0.5,
	.d1 = -0.35,
	.s1 = 0.05,
	.r0 = 0.5,
	.r1 = -0.25
};

struct instrument_profile instrument_piano = {
	.spectrum = { 1, 0.25, 0.2, 0.125, 0.125, 0.125, 0.025, 0.025, 0.002, 0.001, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	.a0 = 0.02,
	.a1 = -0.3,
	.d0 = 1,
	.ds0 = 0,
	.d1 = -0.35,
	.s1 = 0.1,
	.r0 = 1,
	.r1 = -0.5
};

struct instrument_profile instrument_xylophone = {
	.spectrum = { 1, 0.1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	.a0 = 0.02,
	.a1 = -0.3,
	.d0 = 1,
	.ds0 = 0,
	.d1 = -0.35,
	.s1 = 0.1,
	.r0 = 1,
	.r1 = -0.5
};

struct instrument_profile instrument_glockenspiel = {
	.spectrum = { 1, 0.25, 0.05, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	.a0 = 0.02,
	.a1 = -0.3,
	.d0 = 1,
	.ds0 = 0,
	.d1 = -0.35,
	.s1 = 0.1,
	.r0 = 1,
	.r1 = -0.5
};

