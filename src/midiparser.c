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

#include "midiparser.h"

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "streambuf.h"

Midiparser *midiparser_new(void)
{
	Midiparser *this = malloc(sizeof(Midiparser));
	if (this == NULL) {
		return NULL;
	}
	this->format = 0;
	this->ntrks = 0;
	this->division = 0xf0;
	this->tempo = 697674;
	this->last_evt = 0;
	return this;
}

void midiparser_free(Midiparser *this)
{
	free(this);
}

void midiparser_dump(Midiparser *this)
{
	printf("[ Header ]\n");

	printf("format: 0x%x\n", this->format);
	printf("ntrks: 0x%x\n", this->ntrks);
	printf("division: 0x%x\n", this->division);
	printf("tempo: %d\n", this->tempo);
}

int midiparser_ticks_to_ms(Midiparser *this, int ticks)
{
	if (this->division & 0x10000) {
/*
		int negative_smpte_format = (this->division & 0xff00) >> 8;
		int ticks_per_frame = (this->division & 0x00ff);
*/
		print_error("not implemented division");
		return -1;
	}
	else {
		int ticks_per_quarter_note = this->division & 0xffff;
		return (long) ticks * (long) this->tempo / (long) ticks_per_quarter_note;
	}
	return -1;
}

int midiparser_parse_header(Midiparser *this, Streambuf *buf)
{
	//Read magic
	uint32_t magic;
	if (streambuf_read_u32(buf, &magic) != 0) {
		return -1;
	}
	if (magic != MIDIFILE_MAGIC_MTHD) { //MThd
		print_error("Unexpected magic");
		return -1;
	}

	//Length of header data
	uint32_t length;
	if (streambuf_read_u32(buf, &length) != 0) {
		return -1;
	}

	if (length != 6) {
		print_error("Unexpected size");
		return -1;
	}

	//format
	uint16_t format;
	if (streambuf_read_u16(buf, &format) != 0) {
		return -1;
	}

	//ntrks
	uint16_t ntrks;
	if (streambuf_read_u16(buf, &ntrks) != 0) {
		return -1;
	}

	//division
	uint16_t division;
	if (streambuf_read_u16(buf, &division) != 0) {
		return -1;
	}

	this->format = format;
	this->ntrks = ntrks;
	this->division = division;
	return 0;
}

int midiparser_parse_event_time_signature(Midiparser *this, uint8_t *data, uint8_t length)
{
	//FF 58 04 nn dd cc bb Time Signature
	if (length != 4) {
		print_error("Unexpected length");
		return -1;
	}

#ifdef DEBUG
	uint8_t n = data[0];
	uint8_t d = data[1];
	uint8_t c = data[2];
	uint8_t b = data[3];
	DMSG("time signature: n:%d d:%d c:%d b:%d", n, d, c, b);
#endif
	return 0;
}

int midiparser_parse_event_key_signature(Midiparser *this, uint8_t *data, uint8_t length)
{
	//FF 59 02 sf mi
	if (length != 2) {
		print_error("Unexpected length");
		return -1;
	}

#ifdef DEBUG
	uint8_t sf = data[0];
	uint8_t mi = data[1];
	DMSG("key signature: sf:%d mi:%d", sf, mi);
#endif
	return 0;
}

int midiparser_parse_event_set_tempo(Midiparser *this, uint8_t *data, uint8_t length)
{
	//FF 51 03 tttttt
	if (length != 3) {
		print_error("Unexpected length");
		return -1;
	}

	this->tempo = (data[0] << 16) | (data[1] << 8) | (data[2] << 0);
	return 0;
}

int midiparser_parser_smpte_offset(Midiparser *this, uint8_t *data, uint8_t length)
{
	//FF 54 05 hr mn se fr ff SMPTE Offset
	if (length != 5) {
		print_error("Unexpected length");
		return -1;
	}

#ifdef DEBUG
	uint8_t hr = data[0];
	uint8_t mn = data[1];
	uint8_t se = data[2];
	uint8_t fr = data[3];
	uint8_t ff = data[4];
	DMSG("SMPTE Offset: hr: %d0x, mn: %d0x, se: %d0x, fr: %d0x, ff: %d0x", hr, mn, se, fr, ff);
#endif
	return 0;
}

int midiparser_parser_channel_prefix(Midiparser *this, uint8_t *data, uint8_t length)
{
	//FF 20 01 cc MIDI Channel Prefix
	if (length != 1) {
		print_error("Unexpected length");
		return -1;
	}

#ifdef DEBUG
	uint8_t cc = data[0];
	DMSG("Channel prefix: 0x%x", cc);
#endif
	return 0;
}

inline static int midiparser_parse_message_arg(Streambuf *buf, uint8_t *value)
{
	uint8_t v;
	if (streambuf_read_u8(buf, &v) != 0) {
		return -1;
	}
	if ((v & 0x80) != 0) {
		print_error("Unexpected message argument: 0x%x", v);
		return -1;
	}
	*value = v;
	return 0;
}

int midiparser_parse_variable_length_quantity(Midiparser *this, Streambuf *buf, uint32_t *value)
{
	*value = 0;
	uint8_t u8;
	for (int i = 0; i < 4; i++) {
		if (streambuf_read_u8(buf, &u8) != 0) {
			return -1;
		}
		*value = (*value << 7) | (u8 & 0x7f);

		if ((u8 & 0x80) == 0) {
			return 0;
		}
	}
	return -1;
}

int midiparser_parse_meta_event(Midiparser *this, Streambuf *buf)
{
	uint8_t code;
	if (streambuf_read_u8(buf, &code) != 0) {
		return -1;
	}

	uint32_t length;
	if (midiparser_parse_variable_length_quantity(this, buf, &length) != 0) {
		return -1;
	}

	int ret = 0;
	uint8_t *data = malloc(length);
	if (data == NULL) {
		return -1;
	}

	for (int i = 0; i < length; i++) {
		if (streambuf_read_u8(buf, &data[i]) != 0) {
			ret = -1;
			goto exit;
		}
	}

	DMSG("[ meta event ] code: 0x%x, length: %d", code, length);

	switch (code) {
	case 0x01:
		DMSG("[Text Event]");
		break;
	case 0x02:
		DMSG("[Copyright Notice]");
		break;
	case 0x03:
		DMSG("[Track Name]");
		break;
	case 0x04:
		DMSG("[Instrument]");
		break;
	case 0x06:
		DMSG("[Marker]");
		break;
	case 0x08:
		DMSG("[Program Name]");
		break;
	case 0x9:
		DMSG("[Output device]");
		break;
	case 0x0a:
		DMSG("[Author]");
		break;
	case 0x20:
		midiparser_parser_channel_prefix(this, data, length);
		break;
	case 0x51:
		midiparser_parse_event_set_tempo(this, data, length);
		break;
	case 0x54:
		midiparser_parser_smpte_offset(this, data, length);
		break;
	case 0x58:
		midiparser_parse_event_time_signature(this, data, length);
		break;
	case 0x59:
		midiparser_parse_event_key_signature(this, data, length);
		break;
	case 0x2f:
		DMSG("end of track");
		ret = 1;
		break;
	case 0x7f:
		DMSG("[ Unknown ]");
		break;
	case 0x21:
		DMSG("[ Unknown2 ]");
		break;
	default:
		print_error("Unexpected meta event: 0x%x", code);
		ret = -1;
		break;
	}
exit:
	free(data);
	return ret;
}


int midiparser_parse_system_exclusive_message(Midiparser *this, Streambuf *buf)
{
	uint32_t length;
	if (midiparser_parse_variable_length_quantity(this, buf, &length) != 0) {
		return -1;
	}

	DMSG("[ System exclusive ] length: %d", length);

	uint8_t v;
	for (int i = 0; i < length; i++) {
		if (streambuf_read_u8(buf, &v) != 0) {
			return -1;
		}
		DMSG("0x%x", v);
	}
	return 0;
}

int midiparser_parse_system_common_message(Midiparser *this, Streambuf *buf, uint8_t status)
{
	DMSG("system common message: 0x%x", status);

	switch (status) {
	case 0xf0:
		return midiparser_parse_system_exclusive_message(this, buf);

	case 0xf1:
		//undefined
		break;

	case 0xf2: {
		//Song Position Pointer
		uint8_t l;
		if (midiparser_parse_message_arg(buf, &l) != 0) {
			return -1;
		}
		uint8_t m;
		if (midiparser_parse_message_arg(buf, &m) != 0) {
			return -1;
		}
		DMSG("[ Song position pointer ] l: %d, m: %d", l, m);
		break;
	}
	case 0xf3: {
		//Song Select
		uint8_t s;
		if (midiparser_parse_message_arg(buf, &s) != 0) {
			return -1;
		}
		DMSG("[ Song select ] s: %d", s);
		break;
	}
	case 0xf7:
		return midiparser_parse_system_exclusive_message(this, buf);

	case 0xff:
		//Meta event
		return midiparser_parse_meta_event(this, buf);

	default:
		print_error("Not implemented system common message: 0x%x", status);
		return -1;
	}
	return 0;
}

int midiparser_parse_channel_message(Midiparser *this, Player *player, Streambuf *buf, uint8_t status, uint8_t arg1)
{
	int code = (status & 0xf0) >> 4;
	int channel = status & 0x0f;
	DMSG("[ channel message ] code: 0x%x, channel: %d", code, channel);

	switch (code) {
	//1000nnnn 0kkkkkkk 0vvvvvvv: note OFF
	case 0x8: {
		uint8_t k = arg1;
		uint8_t v;
		if (midiparser_parse_message_arg(buf, &v) != 0) {
			return -1;
		}

		DMSG("note OFF: k:%d v:%d", k, v);
		player_set_note(player, channel, k, v, false);

		return 0;
	}

	//1001nnnn 0kkkkkkk 0vvvvvvv: note ON
	case 0x9: {
		uint8_t k = arg1;
		uint8_t v;
		if (midiparser_parse_message_arg(buf, &v) != 0) {
			return -1;
		}

		DMSG("note ON: k:%d v:%d", k, v);
		player_set_note(player, channel, k, v, v != 0);

		return 0;
	}

	//1001nnnn 0kkkkkkk 0vvvvvvv
	case 0xa: {
		uint8_t v;
		if (midiparser_parse_message_arg(buf, &v) != 0) {
			return -1;
		}

		DMSG("Polyphonic Key Pressure: k:%d v:%d", arg1, v);
		return 0;
	}

	//1011nnnn 0ccccccc 0vvvvvvv
	case 0xb: {
		uint8_t v;
		if (midiparser_parse_message_arg(buf, &v) != 0) {
			return -1;
		}

		DMSG("Control Change: c:%d v:%d", arg1, v);
		return 0;
	}

	//1100nnnn 0ppppppp
	case 0xc: {
		DMSG("Program Change: p:%d", arg1);
		return 0;
	}

	//1101nnnn 0vvvvvvv
	case 0xd: {
		DMSG("Channel pressure: p:%d", arg1);
		return 0;
	}

	//1110nnnn 0lllllll 0mmmmmmm
	case 0xe: {
		uint8_t m;
		if (midiparser_parse_message_arg(buf, &m) != 0) {
			return -1;
		}

		DMSG("Pitch Wheel Change: l:%d m:%d", arg1, m);
		return 0;
	}

	default:
		print_error("Not implemented channel message: 0x%x", code);
		return -1;
	}
	return -1;
}

bool midiparser_is_channel_message(uint8_t status)
{
	return ((status & 0x80) != 0 && (status & 0xf0) != 0xf0);
}

int midiparser_parse_event(Midiparser *this, Player *player, Streambuf *buf)
{
	DMSG("[ Event ]");

	uint32_t delta_time;
	if (midiparser_parse_variable_length_quantity(this, buf, &delta_time) != 0) {
		return -1;
	}
	DMSG("delta-time: 0x%x", delta_time);

	int ms = midiparser_ticks_to_ms(this, delta_time);
	if (ms < 0) {
		return -1;
	}
	player_time_forward(player, ms);

	uint8_t evt;
	if (streambuf_read_u8(buf, &evt) != 0) {
		return -1;
	}

	DMSG("evt: 0x%x", evt);

	if ((evt & 0x80) == 0) {
		DMSG("repeat evt: 0x%x", evt);

		//Repeat last channel message
		if (midiparser_is_channel_message(this->last_evt) == false) {
			print_error("Previous event is not a channel message: 0x%x (offset: 0x%0x)", this->last_evt, buf->read_offset);
			return -1;
		}
		DMSG("repeat last evt: 0x%x", this->last_evt);
		return midiparser_parse_channel_message(this, player, buf, this->last_evt, evt);
	}
	else {
		this->last_evt = evt;

		//Channel message
		if (midiparser_is_channel_message(evt) == true) {
			uint8_t arg1;
			if (midiparser_parse_message_arg(buf, &arg1) != 0) {
				return -1;
			}

			return midiparser_parse_channel_message(this, player, buf, evt, arg1);
		}

		//System common message
		else { 
			return midiparser_parse_system_common_message(this, buf, evt);
		}
	}
	return -1;
}

int midiparser_parse_track(Midiparser *this, Player *player, Streambuf *buf)
{
	DMSG("[ Track ]");

	//Read magic
	uint32_t magic;
	if (streambuf_read_u32(buf, &magic) != 0) {
		return -1;
	}
	if (magic != MIDIFILE_MAGIC_MTRK) { //MTrk
		print_error("Unexpected magic value");
		return -1;
	}

	uint32_t length;
	if (streambuf_read_u32(buf, &length) != 0) {
		return -1;
	}
	DMSG("length: 0x%x", length);
	size_t from_offset = buf->read_offset;
	for (;;) {
		int ret = midiparser_parse_event(this, player, buf);
		if (ret < 0) {
			print_error("Could not parse midi event");
			return -1;
		}
		if (ret == 1) {
			break;
		}
	}
	if (length != buf->read_offset - from_offset) {
		print_error("Unexpected end of track");
		return -1;
	}
	return 0;
}

int midiparser_parse(Midiparser *this, Player *player, Streambuf *buf)
{
	if (midiparser_parse_header(this, buf) != 0) {
		return -1;
	}

	for (int i = 0; i < this->ntrks; i++) {
		if (this->format != 2) {
			player_time_reset(player);
		}
		if (midiparser_parse_track(this, player, buf) != 0) {
			print_error("Could not parse track #%d", i + 1);
			return -1;
		}
	}
#ifdef DEBUG
	midiparser_dump(this);
#endif
	return 0;
}

int midiparser_parse_file(Midiparser *this, Player *player, char const *filename)
{
	int ret = -1;

	Streambuf *buf = streambuf_new();
	if (buf == NULL) {
		return -1;
	}

	if (streambuf_open(buf, filename) != 0) {
		goto exit;
	}
	ret = midiparser_parse(this, player, buf);

exit:
	streambuf_free(buf);

	return ret;
}
