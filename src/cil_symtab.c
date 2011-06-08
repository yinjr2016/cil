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
#include <sepol/errcodes.h>
#include <sepol/policydb/hashtab.h>
#include <string.h>
#include "cil_tree.h"
#include "cil_symtab.h"
#include "cil_mem.h"

void cil_symtab_datum_init(struct cil_symtab_datum *datum)
{
	datum->name = NULL;
	datum->node = NULL;
	datum->state = CIL_STATE_ENABLED;
}

void cil_symtab_datum_destroy(struct cil_symtab_datum datum)
{
	free(datum.name);
}

/* This both initializes the datum and inserts it into the symtab.
   Note that cil_symtab_datum_destroy() is the analog to the initializer portion */
int cil_symtab_insert(symtab_t *symtab, hashtab_key_t key, struct cil_symtab_datum *datum, struct cil_tree_node *node)
{
	char *newkey = cil_strdup(key);
	int rc = hashtab_insert(symtab->table, newkey, (hashtab_datum_t)datum);
	if (rc != SEPOL_OK) {
		free(newkey);
		return rc;
	}
	else {
		datum->node = node;
		datum->name = newkey;
	}
	
	return SEPOL_OK;
}

int cil_symtab_get_node(symtab_t *symtab, char *key, struct cil_tree_node **node)
{
	if (symtab == NULL || symtab->table == NULL || key == NULL || node == NULL)
		return SEPOL_ERR;

	struct cil_symtab_datum *datum = (struct cil_symtab_datum*)hashtab_search(symtab->table, (hashtab_key_t)key);
	if (datum == NULL || datum->state != CIL_STATE_ENABLED)
		return SEPOL_ENOENT;

	*node = datum->node;

	return SEPOL_OK;
}

void cil_symtab_destroy(symtab_t *symtab)
{
	if (symtab->table != NULL){
		hashtab_destroy(symtab->table);
		symtab->table = NULL;
	}
}

