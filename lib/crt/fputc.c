/*
 *  ETA/OS - STDIO fputc
 *  Copyright (C) 2014   Michel Megens <dev@bietje.net>
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

/** @file fputc.c */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/bitops.h>

/**
 * @addtogroup libcio
 * @{
 */

/**
 * @brief Write one character to a stream.
 * @param c Character to write.
 * @param stream Stream to write \p c to.
 * @return An error code.
 */
int fputc(int c, struct file * stream)
{
	if(!test_bit(STREAM_WRITE_FLAG, &stream->flags))
		return -EOF;

	if(test_bit(STREAM_RW_BUFFER_FLAG, &stream->flags)) {
		if(stream->index < stream->length) {
			stream->buff[stream->index] = c;
			stream->index++;
			return c;
		}
	}

	return stream->put(c, stream);
}

/** @} */

