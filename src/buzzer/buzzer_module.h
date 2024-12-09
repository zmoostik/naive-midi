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

#ifndef BUZZER_MODULE_H
#define BUZZER_MODULE_H

#include <module.h>

#include "buzzer_pool.h"

struct buzzer_module {
	ModuleClassDef *class_def;

//--- private module data
	char const *devices;
	BuzzerPool *buzzer_pool;
};

typedef struct buzzer_module BuzzerModule;

BuzzerModule *buzzer_module_new(void);

#endif
