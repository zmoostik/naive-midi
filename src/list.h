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

#ifndef LIST_H
#define LIST_H

struct ListItem_;
typedef struct ListItem_ ListItem;

struct ListNode_ {
	struct ListNode_ *next;
	ListItem *item;
};

typedef struct ListNode_ ListNode;

struct List_ {
	struct ListNode_ *first;
	struct ListNode_ *last;
};

typedef struct List_ List;

typedef int (*ListFindCB)(ListItem *item, void *needle);
typedef void (*ListFreeCB)(ListItem *item);
typedef int (*ListExecCB)(ListItem *item);

List *list_new(void);
void list_free(List *this, ListFreeCB free_item);
void list_init(List *this);
void list_clear(List *this, ListFreeCB free_item);
int list_append(List *this, ListItem *item);
int list_remove(List *this, ListItem *item);
int list_count(List *this);
ListItem *list_find(List *this, ListFindCB cmp, void *needle);
ListItem *list_get(List *this, int n);
void list_exec(List *this, ListExecCB exec);

#define LIST_FOREACH(list, node) \
	for (node = list->first; node != NULL; node = node->next) 

#endif

