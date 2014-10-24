/*
 *  ETA/OS - sysctl
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
#include <etaos/types.h>
#include <etaos/error.h>
#include <etaos/stdio.h>
#include <etaos/tick.h>
#include <etaos/stddef.h>

/**
 * @ingroup kernel
 * @brief sysctl - change system wide settings.
 * @param ctl Setting you want to change.
 * @param ... ctl arguments.
 * @see sys_ctl_t
 */
int sysctl(sys_ctl_t ctl, ...)
{
	int err;
	void *stream;
	va_list va;

	va_start(va, ctl);
	switch(ctl) {
	case SYS_SET_STDOUT:
		stream = (FILE) va_arg(va, size_t);
		stdout = stream;
		err = -EOK;
		break;

	case SYS_SET_STDERR:
		stream = (FILE) va_arg(va, size_t);
		stderr = stream;
		err = -EOK;
		break;

	case SYS_SET_STDIN:
		stream = (FILE) va_arg(va, size_t);
		stdin = stream;
		err = -EOK;
		break;

	case SYS_SET_SYSCLK:
		stream = (void*)va_arg(va, size_t);
		sys_clk = stream;
		err = -EOK;
		break;

	default:
		err = -EINVAL;
		break;
	}
	va_end(va);

	return err;
}

