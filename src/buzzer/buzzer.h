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

#ifndef BUZZER_H
#define BUZZER_H

//#define BUZZER_DEBUG

#include <stdbool.h>

struct buzzer {
	char *device;
	int channel;
	int note;
};

typedef struct buzzer Buzzer;

Buzzer *buzzer_new(char const *device);
void buzzer_free(Buzzer *this);
char *buzzer_get_file(Buzzer *this, char const *resource);
void buzzer_assign(Buzzer *this, int channel, int note);
int buzzer_set_state(Buzzer *this, bool state);
int buzzer_set_freq(Buzzer *this, int freq);

#endif

