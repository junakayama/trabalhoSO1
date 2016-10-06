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
#include <semaphore.h>
#include <stdio.h>
/*
 * Buffer.
 */

sem_t cheio_A, cheio_B, vazio_A, vazio_B;
pthread_mutex_t mutex_A, mutex_B;

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
	sem_init(&cheio_A,0,0);
	sem_init(&cheio_B,0,0);
	sem_init(&vazio_A,0,size);
	sem_init(&vazio_B,0,size);

	pthread_mutex_init(&mutex_A,NULL);
	pthread_mutex_init(&mutex_B,NULL);

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

	sem_destroy(&cheio_A);
	sem_destroy(&cheio_B);
	sem_destroy(&vazio_A);
	sem_destroy(&vazio_B);

	pthread_mutex_destroy(&mutex_A);
	pthread_mutex_destroy(&mutex_B);

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
	sem_wait(&vazio_A);
	/* Sanity check. */
	assert(buf != NULL);

	/* Expand buffer. */
	//if (buf->last == buf->size)
	//{
		
		//buf->data = srealloc(buf->data, 2*buf->size*sizeof(unsigned));
		//buf->size *= 2;
	//}

	pthread_mutex_lock(&mutex_A);
	buf->last = ((buf->last)+1) % buf->size;
	buf->data[buf->last] = item;
	pthread_mutex_unlock(&mutex_A);
	sem_post(&cheio_A);
	
}

/*
 * Gets an item from a buffer.
 */
unsigned buffer_get(struct buffer *buf)
{
	sem_wait(&cheio_A);
	unsigned item;

	/* Sanity check. */
	assert(buf != NULL);
	pthread_mutex_lock(&mutex_B);
	buf->first = ((buf->first)+1) % buf->size;
	item = buf->data[buf->first];

	pthread_mutex_unlock(&mutex_B);
	sem_post(&vazio_A);
	return (item);

}
