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

#include "temperament_dom_bedos.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "temperament.h"
#include "common.h"

TemperamentDomBedos *temperament_dom_bedos_new(void)
{
	TemperamentDomBedos *this = malloc(sizeof(TemperamentDomBedos));
	if (this == NULL) {
		return NULL;
	}

	this->class_def = &temperament_dom_bedos_class_def;
	this->freq_c3 = 264.0;

	this->scale[0] = 1;             //C
	this->scale[7] = 3.0 / 2.0;     //G = C + fifth
	this->scale[2] = 9.0 / 8.0;     //D = G + fifth - octave => 3/2 * 3/2 * 1/2
	this->scale[4] = 5.0 / 4.0;     //E = C + third

	this->scale[11] = 15.0 / 8.0;   //B = E + fifth
	this->scale[6] = 45.0 / 32.0;   //F# = B + fifth - octave

	this->scale[5] = 4.0 / 3.0;     //F = C - fifth + octave
	this->scale[10] = 16.0 / 9.0;   //Bb = F - fifth + octave
	this->scale[3] = 32.0 / 27.0;   //Eb = Bb - fifth
	this->scale[8] = 128.0 / 81.0;  //Ab = Eb - fifth + octave
	this->scale[1] = 256.0 / 243.0; //Db = Ab - fifth
	
	this->scale[9] = 161.0 / 96.0;  //A = D + fifth (162/96)
	                                //A = E - fifth + octave (160/96)

	return this;
}

void temperament_dom_bedos_free(TemperamentDomBedos *this)
{
	free(this);
}

float temperament_dom_bedos_get_freq(TemperamentDomBedos *this, int note)
{

	int delta_octave = (note / ARRAY_SIZE(this->scale)) - 5;
	int n = note % ARRAY_SIZE(this->scale);

	return (float) this->freq_c3 * this->scale[n] * pow(2.0, (float) delta_octave);
}

int temperament_dom_bedos_set_pitch(TemperamentDomBedos *this, float freq)
{
	if (freq <= 0) {
		return -1;
	}
	this->freq_c3 = freq / this->scale[9];
	return 0;
}

void temperament_dom_bedos_info(TemperamentDomBedos *this)
{
	printf("C3 frequency: %.1f hz\n", this->freq_c3);
}

TemperamentClassDef temperament_dom_bedos_class_def = {
	.name = "Dom Bedos Temperament",
	.id = "dom-bedos",
	.new = (TemperamentNew) temperament_dom_bedos_new,
	.free = (TemperamentFree) temperament_dom_bedos_free,
	.get_freq = (TemperamentGetFreq) temperament_dom_bedos_get_freq,
	.set_pitch = (TemperamentSetPitch) temperament_dom_bedos_set_pitch,
	.info = (TemperamentInfo) temperament_dom_bedos_info
};

