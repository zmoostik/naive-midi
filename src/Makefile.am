SUBDIRS =

bin_PROGRAMS = naive-midi-player

naive_midi_player_SOURCES = \
	naive-midi-player.c \
	module.c \
	streambuf.c \
	list.c \
	list_qsort.c \
	midiparser.c \
	player.c \
	player_engine.c \
	temperament.c \
	temperament_equal.c \
	temperament_dom_bedos.c

naive_midi_player_LDADD = -lm
naive_midi_player_CPPFLAGS =

if ENABLE_SAMPLER
SUBDIRS += sampler
naive_midi_player_LDADD += sampler/libsampler.a
naive_midi_player_CPPFLAGS += -DCONFIG_SAMPLER
endif

if ENABLE_BUZZER
SUBDIRS += buzzer
naive_midi_player_LDADD += buzzer/libbuzzer.a
naive_midi_player_CPPFLAGS += -DCONFIG_BUZZER
endif

AM_CFLAGS = -Wall -lm
