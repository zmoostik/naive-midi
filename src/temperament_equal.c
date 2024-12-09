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

#include "temperament_equal.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "temperament.h"

TemperamentEqual *temperament_equal_new(void)
{
	TemperamentEqual *this = malloc(sizeof(TemperamentEqual));
	if (this == NULL) {
		return NULL;
	}

	this->class_def = &temperament_equal_class_def;
	this->freq_a3 = 440.0;
	return this;
}

void temperament_equal_free(TemperamentEqual *this)
{
	free(this);
}

#define NOTE_C3 (60)
#define NOTE_A3 (NOTE_C3 + 9)

float temperament_equal_get_freq(TemperamentEqual *this, int note)
{
	float n = note - NOTE_A3;
	float r = pow(2.0, 1.0 / 12.0);
	float f = this->freq_a3 * pow(r, n);
	return f;
}

int temperament_equal_set_pitch(TemperamentEqual *this, float freq)
{
	if (freq <= 0) {
		return -1;
	}
	this->freq_a3 = freq;
	return 0;
}

void temperament_equal_info(TemperamentEqual *this)
{
	printf("A3 frequency: %.1f hz\n", this->freq_a3);
}

TemperamentClassDef temperament_equal_class_def = {
	.name = "Equal Temperament",
	.id = "equal",
	.new = (TemperamentNew) temperament_equal_new,
	.free = (TemperamentFree) temperament_equal_free,
	.get_freq = (TemperamentGetFreq) temperament_equal_get_freq,
	.set_pitch = (TemperamentSetPitch) temperament_equal_set_pitch,
	.info = (TemperamentInfo) temperament_equal_info
};

