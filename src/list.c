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

#include "list.h"

#include <stdlib.h>

#include "common.h"

List *list_new(void) {
	List *this = malloc(sizeof(List));
	if (this == NULL) {
		return NULL;
	}

	list_init(this);
	return this;
}

void list_free(List *this, ListFreeCB free_item) {
	list_clear(this, free_item);
	free(this);
}

void list_init(List *this) {
	this->first = NULL;
	this->last = NULL;
}

void list_clear(List *this, ListFreeCB free_item) {
	ListNode *node = this->first;
	while (node != NULL) {
		ListNode *next = node->next;
		if (free_item != NULL) {
			free_item(node->item);
		}
		free(node);
		node = next;
	}	
	this->first = NULL;
	this->last = NULL;
}

int list_append(List *this, ListItem *item) {
	ListNode *node = malloc(sizeof(ListNode));
	if (node == NULL) {
		return -1;
	}

	node->item = item;
	node->next = NULL;
	
	if (this->first == NULL) {
		this->first = node;
	}
	if (this->last != NULL) {
		this->last->next = node;
	}
	this->last = node;
	return 0;
}

int list_remove(List *this, ListItem *item) {
	ListNode *node = this->first;
	ListNode *prev = NULL;

	while (node != NULL) {
		if (node->item == item) {
			if (prev != NULL) prev->next = node->next;
			if (this->first == node) this->first = node->next;
			if (this->last == node) this->last = prev;
			return 1;
		}
		prev = node;
		node = node->next;
	}
	return 0;
}

int list_count(List *this) {
	int count = 0;

	ListNode *node = this->first;
	while (node != NULL) {
		count++;
		node = node->next;
	}
	return count;
}

ListItem *list_find(List *this, ListFindCB cmp, void *needle) {
	ListNode *node = this->first;
	while (node != NULL) {
		if (cmp(node->item, needle) == 0) {
			return node->item;
		}
		node = node->next;
	}
	return NULL;
}

ListItem *list_get(List *this, int n) {
	ListNode *node;
	int i;
	for (node = this->first, i = 0; node != NULL; node = node->next, i++) {
		if (i == n) {
			return node->item;
		}
	}
	return NULL;
}

void list_exec(List *this, ListExecCB exec) {
	ListNode *node;
	for (node = this->first; node != NULL; node = node->next) {
		exec(node->item);
	}	
}

