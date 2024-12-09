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

#ifndef MODULE_H
#define MODULE_H

#include <stddef.h>
#include <getopt.h>
#include <list.h>

#include <temperament.h>
#include <player_engine.h>

struct module;
typedef struct module Module;

typedef struct option const *(* ModuleGetOptions)(Module *this);
typedef int (* ModuleParseArg)(Module *this, char key, char const *optarg);
typedef void (* ModuleUsage)(Module *this);
typedef void (* ModuleFree)(Module *this);
typedef PlayerEngine *(* ModuleGetEngine)(Module *this, Temperament *temperament);

struct module_class_def {
	char const *name;
	ModuleGetOptions get_options;
	ModuleParseArg parse_arg;
	ModuleUsage usage;
	ModuleGetEngine get_engine;
	ModuleFree free;
};

typedef struct module_class_def ModuleClassDef;

struct module {
	ModuleClassDef *class_def;
	//... 
	//... private module data
	//...
};

// Module wrapping functions
void module_usage(Module *this);
struct option const *module_get_options(Module *this);
int module_parse_arg(Module *this, char key, char const *optarg);

// Modules list
int modules_cat_options(List *list, struct option *dst, size_t length);
void modules_usage(List *list);
int modules_parse_arg(List *list, char key, char const *optarg);
PlayerEngine *modules_get_engine(List *list, Temperament *temperament);
void modules_free(List *list);

//Getopt Helper function
int getopt_get_short_options(struct option *src, char *dst, size_t capacity);
int getopt_cat_options(struct option const *src, struct option *dst, size_t length);


#endif
