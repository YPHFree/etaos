/*
 *  ETA/OS - LibC ioctl
 *  Copyright (C) 2014   Michel Megens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <etaos/kernel.h>
#include <etaos/error.h>
#include <etaos/bitops.h>
#include <etaos/stdio.h>

int ioctl(FILE stream, unsigned long reg, void *buf)
{
	int rc;

	if(!buf && test_bit(STREAM_READ_FLAG, &stream->flags))
		rc = stream->ioctl(stream, reg, buf);
	else if(buf && (stream->flags & (__SRD | __SWR)) == (__SRD | __SWR))
		rc = stream->ioctl(stream, reg, buf);
	else
		rc = -EINVAL;

	return rc;
}

