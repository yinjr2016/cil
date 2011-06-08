/* 
 * Copyright (C) 2011 Tresys Technology, LLC
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdlib.h>
#include <stdio.h>

#include <sepol/policydb/policydb.h>
#include <sepol/policydb/symtab.h>

#include "cil.h"
#include "cil_mem.h"
#include "cil_tree.h"
#include "cil_symtab.h"
#include "cil_build_ast.h"

void cil_list_init(struct cil_list **list)
{
	struct cil_list *new_list = cil_malloc(sizeof(struct cil_list));
	new_list->head = NULL;

	*list = new_list;
}

void cil_list_destroy(struct cil_list **list, uint8_t destroy_data)
{
	struct cil_list_item *item = (*list)->head;
	struct cil_list_item *next = NULL;
	while (item != NULL)
	{
		if (item->flavor == CIL_LIST) 
			cil_list_destroy((struct cil_list**)&(item->data), destroy_data);
		next = item->next;
		cil_list_item_destroy(&item, destroy_data);
		item = next;
	}
	*list = NULL;	
}

void cil_list_item_init(struct cil_list_item **item)
{
	struct cil_list_item *new_item = cil_malloc(sizeof(struct cil_list_item));
	new_item->next = NULL;
	new_item->flavor = 0;
	new_item->data = NULL;

	*item = new_item;
}

void cil_list_item_destroy(struct cil_list_item **item, uint8_t destroy_data)
{
	if (destroy_data) 
		cil_destroy_data(&(*item)->data, (*item)->flavor);
	free(*item);
	*item = NULL;
}


int cil_list_get_tail(struct cil_list *list, struct cil_list_item *tail)
{
	if (list == NULL)
		return SEPOL_ERR;

	struct cil_list_item *curr = list->head;
	while (curr->next != NULL)
		curr = curr->next;

	tail = curr;
	return SEPOL_OK;
}

int cil_list_append_item(struct cil_list *list, struct cil_list_item *item)
{
	if (list == NULL || item == NULL)
		return SEPOL_ERR;

	if (list->head == NULL) {
		list->head = item;
		return SEPOL_OK;
	}

	struct cil_list_item *curr_item = list->head;

	while (curr_item->next != NULL) 
		curr_item = curr_item->next;
	
	curr_item->next = item;

	return SEPOL_OK;
}

int cil_list_prepend_item(struct cil_list *list, struct cil_list_item *item) 
{
	if (list == NULL || item == NULL)
		return SEPOL_ERR;

	if (item->next != NULL) {
		printf("Error: List item to prepend has next\n");
		return SEPOL_ERR;
	}

	struct cil_list_item *old_head = list->head;
	struct cil_list_item *new_head = item;

	list->head = new_head;
	new_head->next = old_head;

	return SEPOL_OK;
}

void cil_print_list_lists(struct cil_list *list_list)
{
	struct cil_list_item *list_item;
	struct cil_list_item *sub_list_item;
	
	list_item = list_list->head;
	while (list_item != NULL) {
		sub_list_item = ((struct cil_list*)list_item->data)->head;
		printf("(");
		while (sub_list_item != NULL) {
			printf(" %p:%s ", sub_list_item->data, ((struct cil_symtab_datum*)sub_list_item->data)->name);
			sub_list_item = sub_list_item->next;
		}
		printf(")\n");
		list_item = list_item->next;
	}
}


