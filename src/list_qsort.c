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

#include "list_qsort.h"

#include <stddef.h>

static void swap(ListNode *a, ListNode *b)
{
	ListItem *a_item = a->item;
	a->item = b->item;
	b->item = a_item;
}

static ListNode *partition(ListNode *head, ListNode *tail, ListCmpCB cmp)
{
	ListNode *pivot = head;
	ListNode *previous = head;
	ListNode *current = head;

	while (current != tail->next) {
		if (cmp(current->item, pivot->item) < 0) {
			swap(current, previous->next);
			previous = previous->next;
		}
		current = current->next;
	}

	swap(pivot, previous);

	return previous;
}

static void qsort_helper(ListNode *head, ListNode *tail, ListCmpCB cmp)
{
	if (head == NULL || head == tail) {
		return;
	}
	ListNode *pivot = partition(head, tail, cmp);

	qsort_helper(head, pivot, cmp);
	qsort_helper(pivot->next, tail, cmp);
}

void list_qsort(List *list, ListCmpCB cmp)
{
	ListNode *tail = list->last;
	qsort_helper(list->first, tail, cmp);
}
