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

#ifndef TEMPERAMENT_H
#define TEMPERAMENT_H

struct temperament;
typedef struct temperament Temperament;

typedef Temperament *(* TemperamentNew)(void);
typedef void (* TemperamentFree)(Temperament *this);
typedef float (* TemperamentGetFreq)(Temperament *this, int note);
typedef int (* TemperamentSetPitch)(Temperament *this, float freq);
typedef void (* TemperamentInfo)(Temperament *this);

struct temperament_class_def {
	char const *id;
	char const *name;
	TemperamentNew new;
	TemperamentFree free;
	TemperamentGetFreq get_freq;
	TemperamentSetPitch set_pitch;
	TemperamentInfo info;
};

typedef struct temperament_class_def TemperamentClassDef;

struct temperament {
	TemperamentClassDef *class_def;
	//... 
	//... private temperament data
	//...
};

Temperament *temperament_new(char const *id);
void temperament_free(Temperament *this);
float temperament_get_freq(Temperament *this, int note);
int temperament_set_pitch(Temperament *this, float freq);
void temperament_info(Temperament *this);

void temperaments_list(char const *prefix);

#endif
