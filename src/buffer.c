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
#include <buffer.h>
#include <pthread.h>
#include <util.h>

/*
 * Buffer.
 */
struct buffer
{
	unsigned *data; /* Data.                        */
	unsigned size;  /* Max size (in elements).      */
	unsigned first; /* First element in the buffer. */
	unsigned last;  /* Last element in the buffer.  */
};

/*
 * Creates a buffer.
 */
struct buffer *buffer_create(unsigned size)
{
	struct buffer *buf;
	
	/* Sanity check. */
	assert(size > 0);

	buf = smalloc(size*sizeof(struct buffer));
	
	/* Initialize buffer. */
	buf->size = size;
	buf->data = smalloc(size*sizeof(unsigned));
	buf->first = 0;
	buf->last = 0;

	return (buf);
}

/*
 * Destroys a buffer.
 */
void buffer_destroy(struct buffer *buf)
{
	/* Sanity check. */
	assert(buf != NULL);
	
	/* House keeping. */
	free(buf->data);
	free(buf);
}

/*
 * Puts an item in a buffer.
 */
void buffer_put(struct buffer *buf, unsigned item)
{
	/* Sanity check. */
	assert(buf != NULL);

	/* Expand buffer. */
	if (buf->last == buf->size)
	{
		buf->data = srealloc(buf->data, 2*buf->size*sizeof(unsigned));
		buf->size *= 2;
	}

	buf->data[buf->last++] = item;
}

/*
 * Gets an item from a buffer.
 */
unsigned buffer_get(struct buffer *buf)
{
	unsigned item;
	
	/* Sanity check. */
	assert(buf != NULL);
	
	item = buf->data[buf->first++];

	return (item);
}
