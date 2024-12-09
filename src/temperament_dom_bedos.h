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

#ifndef TEMPERAMENT_DOM_BEDOS_H
#define TEMPERAMENT_DOM_BEDOS_H

#include "temperament.h"

struct temperament_dom_bedos {
	TemperamentClassDef *class_def;

//--- private temperament data
	float freq_c3;
	float scale[12];
};

typedef struct temperament_dom_bedos TemperamentDomBedos;

TemperamentDomBedos *temperament_dom_bedos_new(void);

extern TemperamentClassDef temperament_dom_bedos_class_def;

#endif
