#
#   ETA/OS - SRAM library
#   Copyright (C) 2017  Michel Megens <dev@michelmegens.net>
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published
#    by the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

## @addtogroup python-sram
# @{

## @package SRAM
#  @brief Provides PyMite's SRAM drivers

import device
from device import Device

__name__ = "sram"

## SRAM class
class SRAM(Device):
	## Create a new SRAM object.
	# @param name Device name.
	def __init__(self, name):
		Device.__init__(self, name)
		self.size = -1

	## Read a string from SRAM.
	# @param addr Address to read from.
	# @param length Number of characters to read.
	# @return String read from SRAM.
	def read_string(self, addr, length):
		rv = self.read(length, addr)
		return rv

	## Write a string to SRAM.
	# @param addr Address to write to.
	# @param string String to write.
	# @return Error code.
	def write_string(self, addr, string):
		rv = self.write(string, addr)
		return rv

	## Read a float from SRAM.
	# @param addr Address to read a float from.
	# @return The float read from SRAM.
	def read_float(self, addr):
		rv = self.read_numeral(addr, True)
		return rv

	## Write a float to SRAM.
	# @param addr Address to write to.
	# @param num Float to write.
	# @return An error code.
	def write_float(self, addr, num):
		rv = self.write_numeral(num, addr)
		return rv
## @}
