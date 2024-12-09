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

#ifndef PLAYER_ENGINE_H
#define PLAYER_ENGINE_H

#include <stdbool.h>
#include <unistd.h>

struct player_engine;
typedef struct player_engine PlayerEngine;

typedef void (* PlayerEngineFree)(PlayerEngine *this);
typedef int (* PlayerEngineSetNote)(PlayerEngine *this, int channel, int note, int velocity, bool state);
typedef int (* PlayerEngineWait)(PlayerEngine *this, useconds_t usec);
typedef int (* PlayerEngineOpen)(PlayerEngine *this);
typedef int (* PlayerEngineClose)(PlayerEngine *this);
typedef void (* PlayerEngineInfo)(PlayerEngine *this);
typedef void (* PlayerEngineShowProgress)(PlayerEngine *this);
typedef void (* PlayerEngineShowProgressOpen)(PlayerEngine *this);
typedef void (* PlayerEngineShowProgressClose)(PlayerEngine *this);

struct player_engine_class_def {
	char const *name;
	PlayerEngineFree free;
	PlayerEngineSetNote set_note;
	PlayerEngineWait wait;
	PlayerEngineOpen open;
	PlayerEngineClose close;
	PlayerEngineInfo info;
	PlayerEngineShowProgress show_progress_open;
	PlayerEngineShowProgress show_progress;
	PlayerEngineShowProgress show_progress_close;
};

typedef struct player_engine_class_def PlayerEngineClassDef;

struct player_engine {
	PlayerEngineClassDef *class_def;
	//... 
	//... private engine data
	//...
};

void player_engine_free(PlayerEngine *this);
int player_engine_set_note(PlayerEngine *this, int channel, int note, int velocity, bool state);
void player_engine_wait(PlayerEngine *this, useconds_t usec);
int player_engine_open(PlayerEngine *this);
int player_engine_close(PlayerEngine *this);
void player_engine_info(PlayerEngine *this);
void player_engine_show_progress_open(PlayerEngine *this);
void player_engine_show_progress(PlayerEngine *this);
void player_engine_show_progress_close(PlayerEngine *this);

#endif
