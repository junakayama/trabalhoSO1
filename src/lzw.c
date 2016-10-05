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

#include <buffer.h>
#include <dictionary.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <util.h>
#include <pthread.h>
#include <semaphore.h>

/* 
 * Parameters.
 */
#define RADIX 256 /* Radix of input data. */
#define WIDTH  12 /* Width of code word.  */

/*============================================================================*
 *                           Bit Buffer Reader/Writer                         *
 *============================================================================*/

/*
 * Writes data to a file.
 */

pthread_t thread_A;
pthread_t thread_B;
pthread_mutex_t cheio, vazio;

static void lzw_writebits(buffer_t in, FILE *out)
{
	int bits;   /* Working bits. */
	unsigned n; /* Current bit.  */
	int buf;    /* Buffer.       */
	
	n = 0;
	buf = 0;
	
	/*
	 * Read data from input buffer
	 * and write to output file.
	 */
	while ((bits = buffer_get(in)) != EOF)
	{	
		buf  = buf << WIDTH;
		buf |= bits & ((1 << WIDTH) - 1);
		n += WIDTH;
				
		/* Flush bytes. */
		while (n >= 8)
		{
			fputc((buf >> (n - 8)) & 0xff, out);
			n -= 8;
		}
	}
	
	if (n > 0)
		fputc((buf << (8 - n)) & 0xff, out);
}

/*============================================================================*
 *                           Bit Buffer Reader/Writer                         *
 *============================================================================*/

/*
 * Reads data from a file.
 */
static void lzw_readbits(FILE *in, buffer_t out)
{
	int bits;   /* Working bits. */
	unsigned n; /* Current bit.  */
	int buf;    /* Buffer.       */
	
	n = 0;
	buf = 0;
	
	/*
	 * Read data from input file
	 * and write to output buffer.
	 */
	while ((bits = fgetc(in)) != EOF)
	{	
		buf = buf << 8;
		buf |= bits & 0xff;
		n += 8;
				
		/* Flush bytes. */
		while (n >= WIDTH)
		{
			buffer_put(out, (buf >> (n - WIDTH)) & ((1 << WIDTH) - 1));
			n -= WIDTH;
		}
	}
			
	buffer_put(out, EOF);
}

/*============================================================================*
 *                                Readbyte                                    *
 *============================================================================*/

/*
 * Reads data from a file.
 */
static void lzw_readbytes(FILE *infile, buffer_t outbuf)
{
	int ch;

	/* Read data from file to the buffer. */
	while ((ch = fgetc(infile)) != EOF)
		buffer_put(outbuf, ch & 0xff);
	
	buffer_put(outbuf, EOF);
}

/*============================================================================*
 *                                writebytes                                  *
 *============================================================================*/

/*
 * Writes data to a file.
 */
static void lzw_writebytes(buffer_t inbuf, FILE *outfile)
{
	// pthread_mutex_lock(&cheio);
	int ch;
	
	/* Read data from file to the buffer. */
	while ((ch = buffer_get(inbuf)) != EOF)
		fputc(ch, outfile);
	// pthread_mutex_unlock(&vazio);
}

/*============================================================================*
 *                                   LZW                                      *
 *============================================================================*/

/*
 * Initializes dictionary.
 */
static code_t lzw_init(dictionary_t dict, int radix)
{
	for (int i = 0; i < radix; i++)
		dictionary_add(dict, 0, i, i);
	
	return (radix);
}

/*
 * Compress data.
 */
static void lzw_compress(buffer_t in, buffer_t out)
{	
	// pthread_mutex_lock(&vazio);
	unsigned ch;       /* Working character. */
	int i, ni;         /* Working entries.   */
	code_t code;       /* Current code.      */
	dictionary_t dict; /* Dictionary.        */
	
	dict = dictionary_create(1 << WIDTH);
	
	i = 0;
	code = lzw_init(dict, RADIX);

	/* Compress data. */
	ch = buffer_get(in);
	while (ch != EOF)
	{	
		ni = dictionary_find(dict, i, (char)ch);
		
		/* Find longest prefix. */
		if (ni >= 0)
		{			
			ch = buffer_get(in);
			i = ni;
		
			/* Next character. */
			if (ch != EOF)
				continue;
		}
		
		buffer_put(out, dict->entries[i].code);
		
		if (code == ((1 << WIDTH) - 1))
		{	
			i = 0;
			dictionary_reset(dict);
			code = lzw_init(dict, RADIX);
			buffer_put(out, RADIX);
			continue;
		}
		
		dictionary_add(dict, i, ch, ++code);
		i = 0;
	}
	
	buffer_put(out, EOF);

	dictionary_destroy(dict);
	// pthread_mutex_unlock(&cheio);
}

/*
 * Builds a string.
 */
static char *buildstr(char *base, char ch)
{
	char *s, *p;
	
	p = s = smalloc((strlen(base) + 2)*sizeof(char));
	
	while (*base != '\0')
		*p++ = *base++;
	
	*p++ = ch;
	*p = '\0';
	
	return (s);
}

/*
 * Decompress data.
 */
static void lzw_decompress(buffer_t in, buffer_t out)
{
	// pthread_mutex_lock(&vazio);
	char *s, *p;   /* Working string. */
	unsigned code; /* Working code.   */
	unsigned i;    /* Loop index.     */
	char **st;     /* String table.   */
	
	st = smalloc(((1 << WIDTH) + 2)*sizeof(char *));
	
	/* Initializes the symbol table. */
	for (i = 0; i < RADIX; i++)
		st[i] = buildstr("", i);
	
	st[i++] =  buildstr("", ' ');
	
	code = buffer_get(in);
	
	/* Broken file. */
	if (code >= i)
		error("broken file");
	
	s = st[code];
	
	/* Decompress data. */
	while (1)
	{
		/* Output current string. */
		for (p = s; *p != '\0'; p++)
			buffer_put(out, (unsigned)(*p & 0xff));
		
		code = buffer_get(in);
		
		/* End of input. */
		if (code == EOF)
			break;
		
		/* Reset symbol table. */
		if (code == RADIX)
		{			
			while (i > 0)
				free(st[--i]);
			
			/* Initializes the symbol table. */
			for (i = 0; i < RADIX; i++)
				st[i] = buildstr("", i);
			
			st[i++] =  buildstr("", ' ');
			
			code = buffer_get(in);
	
			/* Broken file. */
			if (code >= i)
				error("broken file");
			
			s = st[code];
			
			continue;
		}
		
		/* Broken file. */
		if (code > i)
			error("broken file");
		
		p = st[code];
		
		if (i == code)
			p = buildstr(s, s[0]);
		
		st[i++] = buildstr(s, p[0]);
		
		s = p;
	}
	
	buffer_put(out, EOF);
	
	/* House keeping. */
	while (i > 0)
		free(st[--i]);
	free(st);
	// pthread_mutex_unlock(&cheio);
}

/*
 * Compress/Decompress a file using the LZW algorithm. 
 */

 struct argsCompress {
 	buffer_t inbuf;  /* Input buffer.  */
	buffer_t outbuf; /* Output buffer. */
 } argsCompress;

 struct argsWrite {
	buffer_t outbuf;
 	FILE *output;
 } argsWrite;

void lzw(FILE *input, FILE *output, int compress)
{

 	buffer_t inbuf = buffer_create(1024);  /* Input buffer.  */
	buffer_t outbuf = buffer_create(1024);; /* Output buffer. */
	argsCompress.inbuf = inbuf;
	argsCompress.outbuf = outbuf;
	argsWrite.outbuf = outbuf;
	argsWrite.output = output;
	// pthread_mutex_init(&cheio, NULL);
	// pthread_mutex_init(&vazio, NULL);

	/* Compress mode. */
	if (compress)
	{
		lzw_readbytes(input, inbuf);
		pthread_create(&thread_A, NULL, (void *(*)(void *)) lzw_compress, &argsCompress);
		pthread_create(&thread_B, NULL, (void *(*)(void *)) lzw_writebits, &argsWrite);
	}
	
	/* Decompress mode. */
	else
	{		
		lzw_readbits(input, inbuf);
		pthread_create(&thread_A, NULL, (void *(*)(void *)) lzw_decompress, &argsCompress);
		pthread_create(&thread_B, NULL, (void *(*)(void *)) lzw_writebytes, &argsWrite);
	}
	
	pthread_join(thread_A, NULL);
	pthread_join(thread_B, NULL);
	// pthread_mutex_destroy(&cheio);
	// pthread_mutex_destroy(&vazio);
	buffer_destroy(outbuf);
	buffer_destroy(inbuf);
}
