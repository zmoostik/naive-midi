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

#include "player_engine.h"

#include <stdio.h>

#include "common.h"

void player_engine_free(PlayerEngine *this)
{
	this->class_def->free(this);
}

int player_engine_set_note(PlayerEngine *this, int channel, int note, int velocity, bool state)
{
	return this->class_def->set_note(this, channel, note, velocity, state);
}

void player_engine_wait(PlayerEngine *this, useconds_t usec)
{
	this->class_def->wait(this, usec);
}

int player_engine_open(PlayerEngine *this)
{
	return this->class_def->open(this);
}

int player_engine_close(PlayerEngine *this)
{
	return this->class_def->close(this);
}

void player_engine_info(PlayerEngine *this)
{
	printf("Selected engine: %s\n", this->class_def->name);
	this->class_def->info(this);
}

void player_engine_show_progress_open(PlayerEngine *this)
{
	this->class_def->show_progress_open(this);
}

void player_engine_show_progress(PlayerEngine *this)
{
	this->class_def->show_progress(this);
}

void player_engine_show_progress_close(PlayerEngine *this)
{
	this->class_def->show_progress_close(this);
}

