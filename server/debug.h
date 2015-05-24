/* This is a light File Management System, Supports simple file transmission.
 *   			Copyright (C) 2015  Yang Zhang
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *************************************************************************/
#ifndef _DEBUG_H
#define _DEBUG_H
#include <stdio.h>

/* For used to debug program */
#ifdef __DEBUG__
#define debug(fmt, ...) \
	fprintf(stderr, "[" __FILE__ ",%d] " fmt "\n", __LINE__, ##__VA_ARGS__);
#else
#define debug(fmt, ...)
#endif

#endif
