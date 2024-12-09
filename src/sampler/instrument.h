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

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include <stdbool.h>

struct instrument_profile {
	float spectrum[20];

	//ADSR enveloppe setting
	float a0;
	float a1;
	float d0;
	float ds0;
	float d1;
	float s0;
	float s1;
	float r0;
	float r1;
};

typedef struct instrument_profile InstrumentProfile;

struct instrument {
	int velocity;
	float fondamental;
	InstrumentProfile *profile;
};

typedef struct instrument Instrument;

Instrument *instrument_new(float fondamental, int velocity, InstrumentProfile *profile);
void instrument_free(Instrument *this);
float instrument_compute(Instrument *this, float duration, float t, bool *terminated);

extern InstrumentProfile instrument_organ;
extern InstrumentProfile instrument_harp;
extern InstrumentProfile instrument_piano;
extern InstrumentProfile instrument_xylophone;
extern InstrumentProfile instrument_glockenspiel;

#endif
