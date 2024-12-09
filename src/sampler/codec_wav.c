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

#include "codec_wav.h"

#include <stdlib.h>
#include <string.h>

#include <common.h>

#include "endianness.h"

CodecWav *codec_wav_new(int sample_size, int channels, int sample_rate, char const *filename)
{
	CodecWav *this = malloc(sizeof(CodecWav));
	if (this == NULL) {
		return NULL;
	}

	this->sample_size = sample_size;
	this->channels = channels;
	this->sample_rate = sample_rate;
	this->length = 0;
	this->filename = filename;
	this->f = NULL;

	return this;
}

void codec_wav_free(CodecWav *this)
{
	if (this->f != NULL) {
		fclose(this->f);
	}
	free(this);
}

struct codec_wav_header_footprint {
	char chunk_id[4];
	le32_t chunk_size;
	char format[4];
	char subchunk1_id[4];
	le32_t subchunk1_size;
	le16_t audio_format;
	le16_t num_channels;
	le32_t sample_rate;
	le32_t byte_rate;
	le16_t block_align;
	le16_t bits_per_sample;
	char subchunk2_id[4];
	le32_t subchunk2_size;
} __attribute__((packed));

static int codec_wav_write_header(CodecWav *this)
{
	size_t data_size = this->length * this->channels * this->sample_size;

	struct codec_wav_header_footprint header;
	memcpy(&header.chunk_id, "RIFF", 4);
	le32_set(&header.chunk_size, 36 + data_size);
	memcpy(&header.format, "WAVE", 4);

	memcpy(&header.subchunk1_id, "fmt ", 4);
	le32_set(&header.subchunk1_size, 16);
	le16_set(&header.audio_format, 1);
	le16_set(&header.num_channels, this->channels);
	le32_set(&header.sample_rate, this->sample_rate);
	le32_set(&header.byte_rate, this->sample_rate * this->channels * this->sample_size);
	le16_set(&header.block_align, this->channels * this->sample_size);
	le16_set(&header.bits_per_sample, this->sample_size * 8);

	memcpy(&header.subchunk2_id, "data", 4);
	le32_set(&header.subchunk2_size, data_size);

	if (fwrite(&header, sizeof(header), 1, this->f) != sizeof(header)) {
		return -1;
	}

	return 0;
}

int codec_wav_open(CodecWav *this)
{
	FILE *f = fopen(this->filename, "w");
	if (f == NULL) {
		print_error("Could not open output file: '%s'", this->filename);
		return -1;
	}
	this->f = f;

	struct codec_wav_header_footprint header;
	bzero(&header, sizeof(header));

	fwrite(&header, 1, sizeof(header), this->f);
	return 0;
}

int codec_wav_write_pcmbuf(CodecWav *this, Pcmbuf *pcmbuf)
{
	if (pcmbuf_fwrite_interlaced_le(pcmbuf, this->f) != 0) {
		return -1;
	}
	this->length += pcmbuf->length;
	return 0;
}

int codec_wav_close(CodecWav *this)
{
	if (this->f == NULL) {
		return -1;
	}

	if (fseek(this->f, 0, SEEK_SET) != 0) {
		return -1;
	}

	if (codec_wav_write_header(this) != 0) {
		return -1;
	}

	if (fclose(this->f) != 0) {
		return -1;
	}
	
	this->f = NULL;
	return 0;
}

