/*
 *  ETA/OS - File system utility functions
 *  Copyright (C) 2016   Michel Megens <dev@bietje.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @addtogroup fops
 * @{
 */

#include <etaos/kernel.h>
#include <etaos/string.h>
#include <etaos/types.h>
#include <etaos/mem.h>

#include <etaos/fs/util.h>

/**
 * @brief Free the result of #fs_split_path
 * @param split String array allocated by #fs_split_path.
 */
void fs_free_path_split(char **split)
{
	int idx = 0;

	for(; split[idx]; idx++)
		kfree(split[idx]);

	kfree(split[idx]);
	kfree(split);
}

static char **path_split(char *path, const char _delim)
{
	char *tmp = path, *token;
	size_t count = 0UL, idx;
	char *last = NULL;
	char **result = NULL;
	char delim[] = { _delim, 0 };
	char *strtok_last = NULL;

	while(*tmp) {
		if(*tmp == _delim) {
			count ++;
			last = tmp;
		}

		tmp++;
	}

	/* Add space for trailing tokens */
	count += last < (path + strlen(path) - 1);

	/* Space for NULL terminator */
	count ++;

	result = kzalloc(sizeof(char*) * count);

	if(!result)
		return NULL;

	idx = 0UL;
	token = strtok_r(path, delim, &strtok_last);

	while(token) {
		*(result + idx++) = strdup(token);
		token = strtok_r(0, delim, &strtok_last);
	}

	*(result + idx) = NULL;
	return result;
}

/**
 * @brief Split a path into an array.
 * @param path0 Path to split.
 * @return A two dimensional array with the result.
 *
 * This functions splits a path into an array of strings based on the
 * \p / character.
 */
char **fs_split_path(const char *path0)
{
	char *path;
	char **result = NULL;

	path = strdup(path0);
	result = path_split(path, '/');
	kfree(path);

	return result;
}

/** @} */

