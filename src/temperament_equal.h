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

#ifndef TEMPERAMENT_EQUAL_H
#define TEMPERAMENT_EQUAL_H

#include "temperament.h"

struct temperament_equal {
	TemperamentClassDef *class_def;

//--- private temperament data
	float freq_a3;
};

typedef struct temperament_equal TemperamentEqual;

TemperamentEqual *temperament_equal_new(void);

extern TemperamentClassDef temperament_equal_class_def;

#endif
