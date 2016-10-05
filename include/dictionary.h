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

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

/*============================================================================*
 *                            Private Interface                               *
 *============================================================================*/

	/*
	 * Code.
	 */
	typedef unsigned code_t;

	/*
	 * Dictionary entry.
	 */
	struct entry
	{
		char ch;     /* Character.    */
		code_t code; /* Code.         */
		int parent;  /* Parent node.  */
		int next;    /* Next sibling. */
		int child;   /* First child.  */
	};

	/*
	 * Dictionary.
	 */
	struct dictionary
	{
		int max_entries;       /* Maximum number of entries. */
		int nentries;          /* Number of entries.         */
		struct entry *entries; /* Entries.                   */
	};
	
	/*
	 * Opaque pointer to a dictionary.
	 */
	typedef struct dictionary * dictionary_t;
	
/*============================================================================*
 *                             Public Interface                               *
 *============================================================================*/
 
	/* Forward definitions. */
	extern int dictionary_add(dictionary_t, int, char, code_t);
	extern dictionary_t dictionary_create(int);
	extern void dictionary_destroy(dictionary_t);
	extern int dictionary_find(dictionary_t, int, char);
	extern void dictionary_reset(struct dictionary *);

#endif
