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

#include "buzzer_pool.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <common.h>
#include <temperament.h>

PlayerEngineClassDef buzzer_pool_engine_class_def;

BuzzerPool *buzzer_pool_new(Temperament *temperament)
{
	BuzzerPool *this = malloc(sizeof(BuzzerPool));
	if (this == NULL) {
		return NULL;
	}

	this->class_def = &buzzer_pool_engine_class_def;
	this->count = 0;
	this->temperament = temperament;
	return this;
}

void buzzer_pool_free(BuzzerPool *this)
{
	for (int i = 0; i < this->count; i++) {
		buzzer_free(this->buzzers[i]);
	}
	free(this);
}

int buzzer_pool_add(BuzzerPool *this, Buzzer *buzzer)
{
	if (this->count >= BUZZER_POOL_MAX) {
		return -1;
	}

	this->buzzers[this->count] = buzzer;
	this->count++;
	return 0;
}

int buzzer_pool_add_device(BuzzerPool *this, char const *device)
{
	Buzzer *buzzer = buzzer_new(device);
	if (buzzer == NULL) {
		return -1;
	}
	return buzzer_pool_add(this, buzzer);
}

int buzzer_pool_add_devices(BuzzerPool *this, char const *devices)
{
	int ret = 0;
	char const *cur = devices;
	char const *sep = NULL;
	while ((sep = strchr(cur, ':')) != NULL) {
		char *device = strndup(cur, sep - cur);

		ret |= buzzer_pool_add_device(this, device);

		free(device);
		cur = sep + 1;
	}

	ret |= buzzer_pool_add_device(this, cur);
	return ret;
}

int buzzer_pool_get_available_buzzer(BuzzerPool *this)
{
	for (int i = 0; i < this->count; i++) {
		if (this->buzzers[i]->note == -1) {
			return i;
		}
	}
	return -1;
}

int buzzer_pool_choose_buzzer(BuzzerPool *this, int channel)
{
	if (channel < this->count && this->buzzers[channel]->note == -1) {
		return channel;
	}
	return buzzer_pool_get_available_buzzer(this);
}

int buzzer_pool_get_playing_buzzer(BuzzerPool *this, int channel, int note)
{
	for (int i = 0; i < this->count; i++) {
		if (this->buzzers[i]->channel == channel && this->buzzers[i]->note == note) {
			return i;
		}
	}
	return -1;
}

void buzzer_pool_show_progress_open(BuzzerPool *this)
{
}

void buzzer_pool_show_progress(BuzzerPool *this)
{
	printf("|");
	for (int i = 0; i < this->count; i++) {
		Buzzer *buzzer = this->buzzers[i];
		if (buzzer->channel == -1) {
			printf(" --------");
		}
		else {
			printf(" ch%02d:%03d", buzzer->channel, buzzer->note);
		}
		printf(" |");
	}
	printf("\n");
}

void buzzer_pool_show_progress_close(BuzzerPool *this)
{
}

int buzzer_pool_set_note(BuzzerPool *this, int channel, int note, int velocity, bool state)
{
	if (state == true) {
		//exit if the note is already played
		if (buzzer_pool_get_playing_buzzer(this, channel, note) >= 0) {
			DMSG("ch%02d:%03d already playing", channel, note);
			return 0;
		}

		int buzzer = buzzer_pool_choose_buzzer(this, channel);
		if (buzzer == -1) {
			DMSG("No available buzzer");
			return -1;
		}

		int freq = roundf(temperament_get_freq(this->temperament, note));
		buzzer_set_freq(this->buzzers[buzzer], freq);
		buzzer_set_state(this->buzzers[buzzer], true);

		buzzer_assign(this->buzzers[buzzer], channel, note);
	}
	else {
		int buzzer = buzzer_pool_get_playing_buzzer(this, channel, note);
		if (buzzer < 0) {
			DMSG("ch%02d:%03d not playing", channel, note);
			return 0;
		}
		buzzer_set_state(this->buzzers[buzzer], false);

		buzzer_assign(this->buzzers[buzzer], -1, -1);
	}
	return 0;
}

int buzzer_pool_open(BuzzerPool *this)
{
	return 0;
}

int buzzer_pool_close(BuzzerPool *this)
{
	for (int i = 0; i < this->count; i++) {
		buzzer_set_state(this->buzzers[i], false);
	}
	return 0;
}

void buzzer_pool_wait(BuzzerPool *this, useconds_t usec)
{
	usleep(usec);
}

void buzzer_pool_info(BuzzerPool *this)
{
	temperament_info(this->temperament);
	printf("Channels: %d\n", this->count);
}

PlayerEngineClassDef buzzer_pool_engine_class_def = {
	.name = "BuzzerPool",
	.free = (PlayerEngineFree) buzzer_pool_free,
	.set_note = (PlayerEngineSetNote) buzzer_pool_set_note,
	.wait = (PlayerEngineWait) buzzer_pool_wait,
	.open = (PlayerEngineOpen) buzzer_pool_open,
	.close = (PlayerEngineClose) buzzer_pool_close,
	.info = (PlayerEngineInfo) buzzer_pool_info,
	.show_progress_open = (PlayerEngineShowProgressOpen) buzzer_pool_show_progress_open,
	.show_progress = (PlayerEngineShowProgress) buzzer_pool_show_progress,
	.show_progress_close = (PlayerEngineShowProgressClose) buzzer_pool_show_progress_close
};
