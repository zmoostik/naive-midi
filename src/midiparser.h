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

#ifndef MIDIFILE_H
#define MIDIFILE_H

#include <stdint.h>
#include <stddef.h>

#include "player.h"

struct midiparser {
	int ntrks;
	int format;
	int division;
	int tempo;
	uint8_t last_evt;
};

typedef struct midiparser Midiparser;

Midiparser *midiparser_new(void);
void midiparser_free(Midiparser *this);
int midiparser_parse_file(Midiparser *this, Player *player, char const *filename);

#define MIDIFILE_MAGIC_MTHD 0x4d546864
#define MIDIFILE_MAGIC_MTRK 0x4d54726b

#endif
