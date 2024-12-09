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

#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include "list.h"
#include "player_engine.h"

struct event {
	int time;
	int channel;
	int note;
	int velocity;
	bool state;
};

typedef struct event Event;

struct player {
	PlayerEngine *engine;
	List *events;
	int time;
	float speed;
	int transposition;
};

typedef struct player Player;

Player *player_new(PlayerEngine *engine, float speed, int transposition);
void player_free(Player *this);
void player_time_forward(Player *this, int ms);
void player_time_reset(Player *this);
int player_set_note(Player *this, int channel, int note, int velocity, bool state);
int player_play(Player *this);
void player_info(Player *this);

#endif

