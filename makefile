#
# Copyright(C) 2014-2016 Pedro H. Penna <pedrohenriquepenna@gmail.com>
# 
# This file is part of LZW.
# 
# LZW is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
# 
# LZW is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with LZW. If not, see <http://www.gnu.org/licenses/>.
#

# Directories.
BINDIR = bin
INCDIR = include
SRCDIR = src

# Toolchain.
CC = gcc
CFLAGS  = -std=c99 -pedantic
CFLAGS += -Wall -Werror
CFLAGS += -I $(INCDIR) -pthread -O3

# Executable.
EXEC=lzw

# Build everything.
all: 
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) $(SRCDIR)/*.c -o $(BINDIR)/$(EXEC)

# Cleans compilation files.
clean:
	@rm -rf $(BINDIR)/$(EXEC)
