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

#include <stdlib.h>
#include <stdio.h>

/*
 * Prints an error message and exits.
 */
void error(const char *msg)
{
	fprintf(stderr, "Error: %s\n", msg);
	exit(-1);
}

/*
 * Prints a warning message.
 */
void warning(const char *msg)
{
	fprintf(stderr, "Warning: %s\n", msg);
}

/*
 * Safe malloc().
 */
void *smalloc(size_t size)
{
	void *p;
	
	p = malloc(size);
	if (p == NULL) {
		error("cannot smalloc()");
	}
	
	return (p);
}

/*
 * Safe realloc().
 */
void *srealloc(void *p, size_t size)
{
	void *p2;

	p2 = realloc(p, size);
	if (p2 == NULL)
		error("cannot srealloc()");

	return (p2);
}

