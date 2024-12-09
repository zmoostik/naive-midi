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

#include "buzzer_module.h"

#include <stdlib.h>
#include <stdio.h>

#include <common.h>

ModuleClassDef buzzer_module_class_def;

BuzzerModule *buzzer_module_new(void)
{
	BuzzerModule *this = malloc(sizeof(BuzzerModule));
	this->class_def = &buzzer_module_class_def;
	this->devices = getenv("BUZZER_DEVICES");
	this->buzzer_pool = NULL;
	return this;
}

void buzzer_module_free(BuzzerModule *this)
{
	if (this->buzzer_pool != NULL) {
		buzzer_pool_free(this->buzzer_pool);
	}
	free(this);
}

static const struct option buzzer_options[] = {
	{ "buzzer-devices", 1, NULL, 'B' },
	{ NULL, 0, NULL, 0}
};

static struct option const *buzzer_module_get_options(BuzzerModule *this)
{
	return buzzer_options;
}

static void buzzer_module_usage(BuzzerModule *this, char const *app)
{
	printf("   -B,  --buzzer-devices <devices>   Set buzzers devices\n"
	       "                                     Multiple devices can be separated by ':'\n"
	       "                                     This can be set by the environment variable 'BUZZER_DEVICES'\n");
}

static int buzzer_module_parse_arg(BuzzerModule *this, char key, char const *optarg)
{
	switch (key) {
		case 'B':
			this->devices = optarg;
			return 1;
	}
	return 0;
}

static PlayerEngine *buzzer_module_get_engine(BuzzerModule *this, Temperament *temperament)
{
	if (this->devices == NULL) {
		return NULL;
	}

	this->buzzer_pool = buzzer_pool_new(temperament);
	if (this->buzzer_pool == NULL) {
		return NULL;
	}

	buzzer_pool_add_devices(this->buzzer_pool, this->devices);
	return (PlayerEngine *) this->buzzer_pool;
}

ModuleClassDef buzzer_module_class_def = {
	.name = "Buzzer",
	.get_options = (ModuleGetOptions) buzzer_module_get_options,
	.parse_arg = (ModuleParseArg) buzzer_module_parse_arg,
	.usage = (ModuleUsage) buzzer_module_usage,
	.get_engine = (ModuleGetEngine) buzzer_module_get_engine,
	.free = (ModuleFree) buzzer_module_free
};
