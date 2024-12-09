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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <getopt.h>
#include <signal.h>
#include <string.h>
#include <math.h>

#include "common.h"
#include "midiparser.h"
#include "temperament_equal.h"
#include "temperament_dom_bedos.h"
#include "module.h"

#ifdef CONFIG_SAMPLER
#include <sampler/sampler_module.h>
#endif

#ifdef CONFIG_BUZZER
#include <buzzer/buzzer_module.h>
#endif

bool sig_int = false;

struct Options_ {
	char const *input;
	float speed;
	float pitch;
	char const *temperament;
	int transposition;
};

#define DEFAULT_SPEED 1
#define DEFAULT_TEMPERAMENT "equal"
#define DEFAULT_PITCH 440.0

struct Options_ options = {
	.input = NULL,
	.speed = DEFAULT_SPEED,
	.temperament = DEFAULT_TEMPERAMENT,
	.pitch = DEFAULT_PITCH,
	.transposition = 0
};

void print_error(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);

	fprintf(stderr, "ERROR: ");
	vfprintf(stderr, fmt, ap);
	fprintf(stderr, "\n");

	va_end(ap);
}

static void usage(List *modules, char const *app)
{
	printf("USAGE: %s [OPTIONS] <input>\n", app);
	printf("OPTIONS:\n");
	printf("   -v,  --version                    Show program version\n");
	printf("   -h,  --help                       Show this help message\n");
	printf("   -s,  --speed                      Set speed (default: %.0f%%)\n", DEFAULT_SPEED * 100.0);
	printf("   -t,  --transpose                  Set transposition\n");
	printf("   -T,  --temperament                Set temperament (default: '%s')\n", DEFAULT_TEMPERAMENT);
	printf("                                     Available temperaments:\n");
	temperaments_list("                                     * ");
	printf("\n");
	printf("   -p,  --pitch                      Set pitch (default: %.1f hz)\n", DEFAULT_PITCH);

	modules_usage(modules);
}

static void version(char const *app)
{
	printf("%s " PACKAGE_VERSION "\n", app);
}

#define OPTIONS_MAX 32
int parse_args(List *modules, int argc, char const *argv[])
{
	static const struct option core_long_options[] = {
		{ "speed", 1, NULL, 's' },
		{ "temperament", 1, NULL, 'T' },
		{ "pitch", 1, NULL, 'p' },
		{ "transpose", 1, NULL, 't' },
		{ "version", 0, NULL, 'v' },
		{ "help", 0, NULL, 'h' },
		{ NULL, 0, NULL, 0}
	};

	//Concatenate long options
	struct option long_options[OPTIONS_MAX];
	memcpy(long_options, core_long_options, sizeof(core_long_options));
	modules_cat_options(modules, long_options, ARRAY_SIZE(long_options));

	//Concatenate short options
	char short_options[OPTIONS_MAX * 2];
	if (getopt_get_short_options(long_options, short_options, sizeof(short_options)) != 0) {
		return -1;
	}

	int key;
	while ((key = getopt_long(argc, (char **) argv, short_options, long_options, NULL)) != -1) {
		switch (key) {
			case 's':
				options.speed = (float) atoi(optarg) / 100.0;
				break;
			case 't':
				options.transposition = atoi(optarg);
				break;
			case 'T':
				options.temperament = optarg;
				break;
			case 'p':
				options.pitch = atoi(optarg);
				break;
			case 'v':
				version(argv[0]);
				exit(EXIT_SUCCESS);
			case 'h':
				usage(modules, argv[0]);
				exit(EXIT_SUCCESS);
			default: {
				int ret = modules_parse_arg(modules, key, optarg);
				if (ret == 0) {
					print_error("Invalid argument: '%s'", argv[optind - 1]);
					return -1;
				}
				else if (ret < 0) {
					return -1;
				}
			}
		}
	}

	if (optind + 1 != argc) {
		usage(modules, argv[0]);
		return -1;
	}

	options.input = argv[optind++];

	if (options.speed == 0) {
		print_error("Unexpected 'speed'");
		return -1;
	}

	return 0;
}

void sig_term(int signum)
{
	sig_int = true;
}

void sig_init(void)
{
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	action.sa_handler = sig_term;
	sigaction(SIGINT, &action, NULL);
}

int main(int argc, char const *argv[])
{
	//Init modules
	List *modules = list_new();
#ifdef CONFIG_SAMPLER
	list_append(modules, (ListItem *) sampler_module_new());
#endif
#ifdef CONFIG_BUZZER
	list_append(modules, (ListItem *) buzzer_module_new());
#endif

	if (parse_args(modules, argc, argv) != 0) {
		return EXIT_FAILURE;
	}

	sig_init();

	PlayerEngine *player_engine = NULL;
	Player *player = NULL;
	Midiparser *midiparser = NULL;
	Temperament *temperament = NULL;
	int ret = -1;

	//Init temperament
	temperament = temperament_new(options.temperament);
	if (temperament == NULL) {
		print_error("temperament not defined");
		goto exit;
	}
	temperament_set_pitch(temperament, options.pitch);

	//Get player engine
	player_engine = modules_get_engine(modules, temperament);
	if (player_engine == NULL) {
		print_error("player engine not defined");
		goto exit;
	}

	//Init Player
	player = player_new(player_engine, options.speed, options.transposition);
	if (player == NULL) {
		goto exit;
	}

	//Init Midiparser
	midiparser = midiparser_new();
	if (midiparser == NULL) {
		goto exit;
	}

	if (midiparser_parse_file(midiparser, player, options.input) == 0) {
		player_info(player);
		ret = player_play(player);
	}

exit:
	if (temperament != NULL) temperament_free(temperament);
	if (midiparser != NULL) midiparser_free(midiparser);
	if (player != NULL) player_free(player);
	if (modules != NULL) modules_free(modules);

	return ret == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
