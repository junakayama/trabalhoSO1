/*
 * Copyright(C) 2014-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
 * 
 * This file is part of compress.
 * 
 * compress is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * compress is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with compress. If not, see <http://www.gnu.org/licenses/>.
 */

#include <assert.h>
#include <dictionary.h>
#include <util.h>
#include <stdlib.h>

/*
 * Resets a dictionary.
 */
void dictionary_reset(struct dictionary *dict)
{
	dict->nentries = 1;
	
	for (int i = 0; i < dict->max_entries; i++)
	{
		dict->entries[i].parent = -1;
		dict->entries[i].child = -1;
		dict->entries[i].next = -1;
	}
}

/*
 * Creates a dictionary.
 */
struct dictionary *dictionary_create(int max_entries)
{
	struct dictionary *dict;
	
	/* Sanity check. */
	assert(max_entries > 0);
	
	dict = smalloc(sizeof(struct dictionary));
	
	/* Initialize dictionary. */
	dict->max_entries = (max_entries + 1);
	dict->nentries = 1;
	dict->entries = smalloc((max_entries + 1)*sizeof(struct entry));
	for (int i = 0; i < (max_entries + 1); i++)
	{
		dict->entries[i].parent = -1;
		dict->entries[i].child = -1;
		dict->entries[i].next = -1;
	}
	
	return (dict);
}

/*
 * Destroys a dictionary.
 */
void dictionary_destroy(struct dictionary *dict)
{
	/* Sanity check. */
	assert(dict != NULL);
	
	free(dict->entries);
	free(dict);
}
 
/*
 * Adds a character to a dictionary entry.
 */
int dictionary_add(struct dictionary *dict, int i, char ch, code_t code)
{
	int j;
	
	/* Sanity check. */
	assert(dict != NULL);
	assert(i >= 0);
	
	/* Dictionary overflow. */
	if (dict->nentries >= dict->max_entries)
	{
		warning("dictionary overflow");
		return (-1);
	}
	
	j = dict->nentries++;
    
	/* Add entry to dictionary. */
	dict->entries[j].parent = i;
	dict->entries[j].child = -1;
	dict->entries[j].next = dict->entries[i].child;
	dict->entries[i].child = j;
	dict->entries[j].ch = ch;
	dict->entries[j].code = code;
			
	return (j);
}

/*
 * Searches for a character in a dictionary entry.
 */
int dictionary_find(struct dictionary *dict, int i, char ch)
{
	for (int j = dict->entries[i].child; j >= 0; j = dict->entries[j].next)
	{
		if (ch == dict->entries[j].ch)
			return (j);
	}

	return (-1);
}
