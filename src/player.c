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

#include "player.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "list_qsort.h"
#include "common.h"

Event *event_new(int time, int channel, int note, int velocity, bool state)
{
	Event *this = malloc(sizeof(Event));
	if (this == NULL) {
		return NULL;
	}
	this->time = time;
	this->channel = channel;
	this->note = note;
	this->velocity = velocity;
	this->state = state;
	return this;
}

void event_free(Event *this)
{
	free(this);
}

int event_cmp_time(Event *a, Event *b)
{
	if (a->time == b->time) {
		if (a->state == b->state) {
			return 0;
		}
		return a->state ? 1 : -1;
	}
	return a->time > b->time ? 1 : -1;
}

Player *player_new(PlayerEngine *engine, float speed, int transposition)
{
	Player *this = malloc(sizeof(Player));
	if (this == NULL) {
		return NULL;
	}
	this->events = list_new();
	if (this->events == NULL) {
		free(this);
		return NULL;
	}

	this->engine = engine;
	this->time = 0;
	this->speed = speed;
	this->transposition = transposition;
	return this;
}

void player_free(Player *this)
{
	list_free(this->events, (ListFreeCB) event_free);
	free(this);
}

void player_time_forward(Player *this, int ms)
{
	this->time += ms;
}

void player_time_reset(Player *this)
{
	this->time = 0;
}

int player_set_note(Player *this, int channel, int note, int velocity, bool state)
{
	Event *event = event_new(this->time, channel, note, velocity, state);
	if (event == NULL) {
		return -1;
	}

	list_append(this->events, (ListItem *) event);
	return 0;
}

int player_play(Player *this)
{
	player_engine_open(this->engine);
	player_engine_show_progress_open(this->engine);

	int time = 0;
	list_qsort(this->events, (ListCmpCB) event_cmp_time);
	ListNode *node;
	LIST_FOREACH(this->events, node) {
		Event *event = (Event *) node->item;
		if (event->time > time) {
			int delay = (float) (event->time - time) / this->speed;
			DMSG("wait(%d)", delay);
			player_engine_wait(this->engine, delay);
			player_engine_show_progress(this->engine);
		}
		if (sig_int == true) {
			break;
		}
		time = event->time;
		player_engine_set_note(this->engine, event->channel, event->note + this->transposition, event->velocity, event->state);
	}

	player_engine_close(this->engine);
	player_engine_show_progress_close(this->engine);
	return 0;
}

void player_info(Player *this)
{
	player_engine_info(this->engine);
	printf("speed: %.0f%%\n", this->speed * 100);
	if (this->transposition != 0) printf("transposition: %d\n", this->transposition);
}
