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

#include "buzzer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <common.h>

Buzzer *buzzer_new(char const *device)
{
	Buzzer *this = malloc(sizeof(Buzzer));
	if (this == NULL) {
		return NULL;
	}

	this->device = strdup(device);
	if (this->device == NULL) {
		return NULL;
	}

	this->note = -1;
	this->channel = -1;

	return this;
}

void buzzer_free(Buzzer *this)
{
	free(this->device);
	free(this);
}

char *buzzer_get_file(Buzzer *this, char const *resource)
{
	size_t size = strlen(this->device) + 1 + strlen(resource) + 1;
	char *str = malloc(size);
	if (str == NULL) {
		return NULL;
	}
	snprintf(str, size, "%s/%s", this->device, resource);
	return str;
}

void buzzer_assign(Buzzer *this, int channel, int note)
{
	this->channel = channel;
	this->note = note;
}

#ifdef BUZZER_DEBUG

int buzzer_set_state(Buzzer *this, bool state)
{
	char *path = buzzer_get_file(this, "state");
	if (path == NULL) {
		return -1;
	}

	printf("echo \"%s\" > %s\n", state == true ? "on" : "off", path);

	free(path);
	return 0;
}

int buzzer_set_freq(Buzzer *this, int freq)
{
	char *path = buzzer_get_file(this, "freq");
	if (path == NULL) {
		return -1;
	}

	printf("echo \"%d\" > %s\n", freq, path);
	free(path);
	return 0;
}

#else

int buzzer_set_state(Buzzer *this, bool state)
{
	char *path = buzzer_get_file(this, "state");
	if (path == NULL) {
		return -1;
	}

	FILE *f = fopen(path, "w");
	free(path);
	if (f == NULL) {
		return -1;
	}

	fprintf(f, "%s\n", state == true ? "on" : "off");
	fclose(f);
	return 0;
}

int buzzer_set_freq(Buzzer *this, int freq)
{
	char *path = buzzer_get_file(this, "freq");
	if (path == NULL) {
		return -1;
	}

	FILE *f = fopen(path, "w");
	free(path);
	if (f == NULL) {
		return -1;
	}

	fprintf(f, "%d\n", freq);
	fclose(f);
	return 0;
}
#endif

