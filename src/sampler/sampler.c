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

#include "sampler.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <common.h>

#include "sampler_event.h"
#include "pcmbuf.h"
#include "codec_wav.h"

PlayerEngineClassDef sampler_engine_class_def;

Sampler *sampler_new(float gain, bool autopan, CodecWav *codec_wav, Temperament *temperament)
{
	Sampler *this = malloc(sizeof(Sampler));
	if (this == NULL) {
		return NULL;
	}

	this->class_def = &sampler_engine_class_def;

	this->position = 0;
	this->samplerate = codec_wav->sample_rate;
	this->gain = gain;
	this->autopan = autopan;
	this->codec_wav = codec_wav;
	this->temperament = temperament;
	this->events = list_new();
	return this;
}

void sampler_free(Sampler *this)
{
	list_free(this->events, (ListFreeCB) sampler_event_free);
	free(this);
}

void sampler_debug(Sampler *this)
{
	ListNode *node;
	LIST_FOREACH(this->events, node) {
		SamplerEvent *event = (SamplerEvent *) node->item;
		if (node != this->events->first) {
			printf(", ");
		}
		printf("ch%02d:%03d", event->channel, event->note);
	}
	printf("\n");
}

SamplerEvent *sampler_find_event(Sampler *this, int channel, int note)
{
	ListNode *node;
	LIST_FOREACH(this->events, node) {
		SamplerEvent *event = (SamplerEvent *) node->item;
		if (event->channel == channel && event->note == note && event->ending == false) {
			return event;
		}
	}
	return NULL;
}

int sampler_set_note(Sampler *this, int channel, int note, int velocity, bool state)
{
//	printf("channel: %d, note: %d, velocity: %d, state: %s\n", channel, note, velocity, state ? "on" : "off");
	SamplerEvent *event = sampler_find_event(this, channel, note);

	if (state == true) {
		if (event != NULL) {
			return -1;
		}

		float pan = this->autopan == true ? (float) note / 127.0 : -1.0;
		event = sampler_event_new(this, this->position, channel, note, velocity, this->gain, pan);
		list_append(this->events, (ListItem *) event);
	}
	else {
		if (event == NULL) {
			return -1;
		}
		sampler_event_set_end(event, this->position);
	}

//	sampler_debug(this);
	return 0;
}

int sampler_open(Sampler *this)
{
	return codec_wav_open(this->codec_wav);
}

void sampler_wait(Sampler *this, useconds_t usec)
{
	unsigned int length = roundf((double) usec * (double) this->samplerate / 1000000.0);

	Pcmbuf *pcmbuf = pcmbuf_new(length, this->codec_wav->sample_size, this->codec_wav->channels);
	if (pcmbuf == NULL) {
		return;
	}
	pcmbuf_clear(pcmbuf);

	ListNode *node;
	ListNode *next;
	for(node = this->events->first; node != NULL; node = next) {
		next = node->next;

		SamplerEvent *event = (SamplerEvent *) node->item;
		sampler_event_render(event, this->position, pcmbuf);

		if (event->terminated == true) {
			sampler_event_free(event);
			list_remove(this->events, (ListItem *) event);
		}
	}
	this->position += length;

	codec_wav_write_pcmbuf(this->codec_wav, pcmbuf);
	pcmbuf_free(pcmbuf);
}

int sampler_close(Sampler *this)
{
	sampler_wait(this, 1000000);
	return codec_wav_close(this->codec_wav);
}

void sampler_info(Sampler *this)
{
	temperament_info(this->temperament);
	printf("Gain: %.0f%%\n", this->gain * 100.0);
	printf("Samplerate: %d hz\n", this->samplerate);
	printf("Samples size: %d bits\n", this->codec_wav->sample_size * 8);
	printf("Channels: %d\n", this->codec_wav->channels);
	if (this->codec_wav->channels == 2) {
		printf("Autopan: %s\n", this->autopan == true ? "on" : "off");
	}
}

void sampler_show_progress_open(Sampler *this)
{
}

void sampler_show_progress(Sampler *this)
{
	printf("Processing... ");
	int sec = (int) round(this->position / this->samplerate);
	int s = sec % 60;
	int m = sec / 60;
	if (m > 0) {
		printf("%d:%02d", m, s);
	}
	else {
		printf("%ds", s);
	}
	printf("\r");
	fflush(stdout);
}

void sampler_show_progress_close(Sampler *this)
{
	printf("\n");
}

PlayerEngineClassDef sampler_engine_class_def = {
	.name = "Sampler",
	.free = (PlayerEngineFree) sampler_free,
	.set_note = (PlayerEngineSetNote) sampler_set_note,
	.wait = (PlayerEngineWait) sampler_wait,
	.open = (PlayerEngineOpen) sampler_open,
	.close = (PlayerEngineClose) sampler_close,
	.info = (PlayerEngineInfo) sampler_info,
	.show_progress_open = (PlayerEngineShowProgressOpen) sampler_show_progress_open,
	.show_progress = (PlayerEngineShowProgress) sampler_show_progress,
	.show_progress_close = (PlayerEngineShowProgressClose) sampler_show_progress_close
};
