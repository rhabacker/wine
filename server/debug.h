/*
 * Wine server debug and error support
 *
 * Copyright (C) 2001 Alexandre Julliard
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef __WINE_SERVER_DEBUG_H
#define __WINE_SERVER_DEBUG_H

#include <stdio.h>

void _trace(const char *file, int line, const char *func, const char *fmt, ...);
void _trace_raw(const char *fmt, ...);

#define TRACE(...) _trace(__FILE__, __LINE__, __func__, __VA_ARGS__)
#define TRACE_RAW(...) _trace_raw(__VA_ARGS__)

#ifdef __GNUC__
extern void fatal_error( const char *err, ... )  __attribute__((noreturn,format(printf,1,2)));
#else
extern void fatal_error( const char *err, ... );
#endif

#endif
