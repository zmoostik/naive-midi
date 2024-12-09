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

#include "sampler_module.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <common.h>

ModuleClassDef sampler_module_class_def;

SamplerModule *sampler_module_new(void)
{
	SamplerModule *this = malloc(sizeof(SamplerModule));
	this->class_def = &sampler_module_class_def;
	this->output = NULL;
	this->channels = 2;
	this->gain = 0.2;
	this->samplerate = 44100;
	this->samplesize = 4;
	this->autopan = false;

	this->codec_wav = NULL;
	this->sampler = NULL;

	return this;
}

void sampler_module_free(SamplerModule *this)
{
	if (this->codec_wav != NULL) {
		codec_wav_free(this->codec_wav);
	}
	if (this->sampler != NULL) {
		sampler_free(this->sampler);
	}
	free(this);
}

static const struct option sampler_options[] = {
	{ "output", 1, NULL, 'o' },
	{ "channels", 1, NULL, 'c' },
	{ "sample-rate", 1, NULL, 'r' },
	{ "sample-bits", 1, NULL, 'S' },
	{ "gain", 1, NULL, 'g' },
	{ "autopan", 0, NULL, 'P' },
	{ NULL, 0, NULL, 0}
};

static struct option const *sampler_module_get_options(SamplerModule *this)
{
	return sampler_options;
}

static void sampler_module_usage(SamplerModule *this, char const *app)
{

	printf("   -o,  --output                     Set output filename\n");
	printf("   -c,  --channels                   Set output channels (default: %d)\n", SAMPLER_DEFAULT_CHANNELS);
	printf("   -r,  --sample-rate                Set output sample rate (default: %d hz)\n", SAMPLER_DEFAULT_SAMPLE_RATE);
	printf("   -S,  --sample-bits                Set output sample size (default: %d bits)\n", SAMPLER_DEFAULT_SAMPLE_SIZE * 8);
	printf("   -g,  --gain                       Set gain (default: %.0f%%)\n", SAMPLER_DEFAULT_GAIN * 100.0);
	printf("   -P,  --autopan                    Pan right/left low/high notes\n");
}

static int sampler_module_parse_arg(SamplerModule *this, char key, char const *optarg)
{
	switch (key) {
		case 'o':
			this->output = optarg;
			return 1;
		case 'c': {
			int channels = atoi(optarg);
			if (channels < 1 || channels > 16) {
				print_error("Unexpected channels number: %d", channels);
				return -1;
			}
			this->channels = channels;
			return 1;
		}
		case 'r': {
			int samplerate = atoi(optarg);
			if (samplerate < 12000) {
				print_error("Unexpected sample rate: %d", samplerate);
				return -1;
			}
			this->samplerate = samplerate;
			return 1;
		}
		case 'S': {
			int bits = atoi(optarg);
			if (bits != 8 && bits != 16 && bits != 32) {
				print_error("Unexpected sample size: %d bits", bits);
				return -1;
			}
			this->samplesize = bits / 8;
			return 1;
		}
		case 'g':
			this->gain = (float) atoi(optarg) / 100.0;
			return 1;
		case 'P':
			this->autopan = true;
			return 1;
	}
	return 0;
}

static PlayerEngine *sampler_module_get_engine(SamplerModule *this, Temperament *temperament)
{
	if (this->output == NULL) {
		return NULL;
	}

	this->codec_wav = codec_wav_new(this->samplesize, this->channels, this->samplerate, this->output);
	if (this->codec_wav == NULL) {
		return NULL;
	}
	this->sampler = sampler_new(this->gain, this->autopan, this->codec_wav, temperament);
	return (PlayerEngine *) this->sampler;
}

ModuleClassDef sampler_module_class_def = {
	.name = "Sampler",
	.get_options = (ModuleGetOptions) sampler_module_get_options,
	.parse_arg = (ModuleParseArg) sampler_module_parse_arg,
	.usage = (ModuleUsage) sampler_module_usage,
	.get_engine = (ModuleGetEngine) sampler_module_get_engine,
	.free = (ModuleFree) sampler_module_free
};
