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

#include "module.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void module_usage(Module *this)
{
	this->class_def->usage(this);
}

struct option const *module_get_options(Module *this)
{
	return this->class_def->get_options(this);
}

int module_parse_arg(Module *this, char key, char const *optarg)
{
	return this->class_def->parse_arg(this, key, optarg);
}

PlayerEngine *module_get_engine(Module *this, Temperament *temperament)
{
	return this->class_def->get_engine(this, temperament);
}

void module_free(Module *this)
{
	this->class_def->free(this);
}

//--- Modules helper functions

int modules_cat_options(List *list, struct option *dst, size_t length)
{
	ListNode *node;
	LIST_FOREACH(list, node) {
		Module *module = (Module *) node->item;
		getopt_cat_options(module_get_options(module), dst, length);
	}
	return 0;
}

void modules_usage(List *list)
{
	ListNode *node;
	LIST_FOREACH(list, node) {
		Module *module = (Module *) node->item;
		printf("\n%s module options:\n", module->class_def->name);
		module_usage(module);
	}
}

int modules_parse_arg(List *list, char key, char const *optarg)
{
	ListNode *node;
	LIST_FOREACH(list, node) {
		Module *module = (Module *) node->item;
		int ret = module_parse_arg(module, key, optarg);
		if (ret != 0) return ret;
	}
	return 0;
}

PlayerEngine *modules_get_engine(List *list, Temperament *temperament)
{
	ListNode *node;
	LIST_FOREACH(list, node) {
		Module *module = (Module *) node->item;
		PlayerEngine *engine = module_get_engine(module, temperament);
		if (engine != NULL) {
			return engine;
		}
	}
	return NULL;
}

void modules_free(List *list)
{
	list_free(list, (ListFreeCB) module_free);
}

//--- Getopt helper functions

int getopt_get_short_options(struct option *src, char *dst, size_t capacity)
{
	size_t size = 0;

	for (int i = 0; src[i].name != NULL; i++) {
		if (size + 3 > capacity) {
			return -1;
		} 
		dst[size++] = src[i].val;
		if (src[i].has_arg == 1) {
			dst[size++] = ':';
		}
	}
	dst[size++] = '\0';
	return 0;
}

int getopt_cat_options(struct option const *src, struct option *dst, size_t length)
{
	int n = 0;
	while (dst[n].name != NULL) {
		n++;
	}
 
	for (int i = 0; src[i].name != NULL && n < length - 1; i++) {
		memcpy(&dst[n], &src[i], sizeof(struct option));
		n++;
	}

	dst[n].name = NULL;
	dst[n].has_arg = 0;
	dst[n].flag = NULL;
	dst[n].val = 0;

	return 0;
}

