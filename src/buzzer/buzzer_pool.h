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

#ifndef BUZZER_POOL_H
#define BUZZER_POOL_H

#include <stdbool.h>

#include <player_engine.h>
#include <temperament.h>
#include "buzzer.h"

#define BUZZER_POOL_MAX 32

struct buzzer_pool {
	PlayerEngineClassDef *class_def;

//--- private engine data
	Temperament *temperament;
	Buzzer *buzzers[BUZZER_POOL_MAX];
	int count;
};

typedef struct buzzer_pool BuzzerPool;

BuzzerPool *buzzer_pool_new(Temperament *temperament);
void buzzer_pool_free(BuzzerPool *this);
int buzzer_pool_add(BuzzerPool *this, Buzzer *buzzer);
int buzzer_pool_add_devices(BuzzerPool *this, char const *devices);
int buzzer_pool_set_note(BuzzerPool *this, int channel, int note, int velocity, bool state);
void buzzer_pool_debug(BuzzerPool *this);
int buzzer_pool_open(BuzzerPool *this);
int buzzer_pool_close(BuzzerPool *this);
void buzzer_pool_wait(BuzzerPool *this, useconds_t usec);

#endif

