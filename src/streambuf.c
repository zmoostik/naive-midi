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

#include "streambuf.h"

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

Streambuf *streambuf_new(void)
{
	Streambuf *this = malloc(sizeof(Streambuf));
	if (this == NULL) {
		return NULL;
	}

	this->data = NULL;
	this->read_offset = 0;
	this->size = 0;
	return this;
}

void streambuf_free(Streambuf *this)
{
	if (this->data != NULL) {
		free(this->data);
	}
	free(this);
}

int streambuf_open(Streambuf *this, char const *filename)
{
	int ret = -1;

	FILE *f = fopen(filename, "r");
	if (f == NULL) {
		print_error("Could not open inpuf file: '%s'", filename);
		return -1;
	}

	//Get file size
	if (fseek(f, 0, SEEK_END) != 0) {
		goto exit;
	}

	size_t size = ftell(f);

	if (fseek(f, 0, SEEK_SET) != 0) {
		goto exit;
	}

	//Read file content
	this->data = malloc(size);
	if (this->data == NULL) {
		goto exit;
	}

	this->size = size;
	this->read_offset = 0;

	if (fread(this->data, 1, this->size, f) != this->size) {
		goto exit;
	}

	ret = 0;

exit:
	fclose(f);

	return ret;
}

int streambuf_read_u8(Streambuf *this, uint8_t *value)
{
	if (this->read_offset >= this->size) {
		return -1;
	}

	*value = this->data[this->read_offset];
	this->read_offset++;
	return 0;
}

int streambuf_read_u16(Streambuf *this, uint16_t *value)
{
	*value = 0;
	uint8_t u8;
	for (int i = 0; i < 2; i++) {
		if (streambuf_read_u8(this, &u8) != 0) {
			return -1;
		}
		*value = (*value << 8) | u8;
	}
	return 0;
}

int streambuf_read_u32(Streambuf *this, uint32_t *value)
{
	*value = 0;
	uint8_t u8;
	for (int i = 0; i < 4; i++) {
		if (streambuf_read_u8(this, &u8) != 0) {
			return -1;
		}
		*value = (*value << 8) | u8;
	}
	return 0;
}

