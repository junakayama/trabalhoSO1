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

#ifndef BUFFER_H_
#define BUFFER_H_

	/*
	 * Opaque pointer to a circular buffer.
	 */
	typedef struct buffer * buffer_t;

	/* Forward definitions. */
	extern void buffer_destroy(buffer_t);
	extern unsigned buffer_get(buffer_t);
	extern buffer_t buffer_create(unsigned);
	extern void buffer_put(buffer_t, unsigned);

#endif /* BUFFER_H_ */
