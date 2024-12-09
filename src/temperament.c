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

#include "temperament.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "temperament_equal.h"
#include "temperament_dom_bedos.h"

static TemperamentClassDef *temperament_classes[] = {
	&temperament_equal_class_def,
	&temperament_dom_bedos_class_def
};

Temperament *temperament_new(char const *name)
{
	for (int i = 0; i < ARRAY_SIZE(temperament_classes); i++) {
		if (strcmp(name, temperament_classes[i]->id) == 0) {
			return temperament_classes[i]->new();
		}
	}
	return NULL;
}

void temperament_info(Temperament *this)
{
	printf("Temperament: %s\n", this->class_def->name);
	this->class_def->info(this);
}

void temperament_free(Temperament *this)
{
	this->class_def->free(this);
}

float temperament_get_freq(Temperament *this, int note)
{
	return this->class_def->get_freq(this, note);
}

int temperament_set_pitch(Temperament *this, float freq)
{
	return this->class_def->set_pitch(this, freq);
}

void temperaments_list(char const *prefix)
{
	for (int i = 0; i < ARRAY_SIZE(temperament_classes); i++) {
		if (i > 0) printf("\n");
		printf("%s'%s': %s", prefix, temperament_classes[i]->id, temperament_classes[i]->name);
	}
}
