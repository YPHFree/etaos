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

## @addtogroup python-tmp35
# @{

## @package SRAM
#  @brief Provides PyMite's SRAM drivers

__name__ = "sram"

class SRAM(object):
	name = ""

	## Create a new SRAM object.
	# @param name Name of the SRAM device driver.
	def __init__(self, name):
		self.name = name

	## Read from \p this SRAM chip.
	# @param addr Address to start reading.
	# @param num Number of bytes to read.
	# @return An array list of the read bytes.
	def read(self, addr, num):
		return read(self.name, addr, num)

	## Write to \p this SRAM chip.
	# @param addr Address to start writing.
	# @param data List of bytes to write.
	# @param length Length of \p data.
	# @return None
	def write(self, addr, data, length):
		write(self.name, addr, data, length)

	## Write to \p this SRAM chip.
	# @param addr Address to start writing.
	# @param string List of bytes to write.
	# @return None
	def write_string(self, addr, string):
		write_string(self.name, addr, string, len(string))

	## Read from \p this SRAM chip.
	# @param addr Address to start reading.
	# @param length Number of bytes to read.
	# @return String object.
	def read_string(self, addr, length):
		return read_string(self.name, addr, length)

## @}

def write(name, addr, data, num):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t addr, data, num, name;
	const char *cname;
	size_t cnum;
	uint16_t caddr;
	void *cdata;

	if (NATIVE_GET_NUM_ARGS() != 4) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	name = NATIVE_GET_LOCAL(0);
	addr = NATIVE_GET_LOCAL(1);
	data = NATIVE_GET_LOCAL(2);
	num = NATIVE_GET_LOCAL(3);

	if(OBJ_GET_TYPE(name) != OBJ_TYPE_STR ||
		OBJ_GET_TYPE(data) != OBJ_TYPE_LST ||
		OBJ_GET_TYPE(addr) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(num) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cname = (const char*)((pPmString_t)name)->val;
	caddr = ((pPmInt_t)addr)->val;
	cnum = ((pPmInt_t)num)->val;
	cdata = kzalloc(cnum);

	list_to_byte_array(data, cdata, cnum);

	pm_sram_write(cname, caddr, cdata, cnum);
	kfree(cdata);

	return retval;
	"""
	pass

def read(name, addr, num):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t addr, data, num, name;
	const char *cname;
	size_t cnum;
	uint16_t caddr;
	void *cdata;

	if (NATIVE_GET_NUM_ARGS() != 3) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	name = NATIVE_GET_LOCAL(0);
	addr = NATIVE_GET_LOCAL(1);
	num = NATIVE_GET_LOCAL(2);

	if(OBJ_GET_TYPE(name) != OBJ_TYPE_STR ||
		OBJ_GET_TYPE(addr) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(num) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cname = (const char*)((pPmString_t)name)->val;
	caddr = ((pPmInt_t)addr)->val;
	cnum = ((pPmInt_t)num)->val;
	cdata = kzalloc(cnum);

	pm_sram_read(cname, caddr, cdata, cnum);
	data = list_create_from_ptr(cdata, cnum);
	kfree(cdata);

	NATIVE_SET_TOS(data);
	return retval;
	"""
	pass

def write_string(name, addr, string, length):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t addr, data, num, name;
	const char *cname;
	size_t cnum;
	uint16_t caddr;
	void *cdata;

	if (NATIVE_GET_NUM_ARGS() != 4) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	name = NATIVE_GET_LOCAL(0);
	addr = NATIVE_GET_LOCAL(1);
	data = NATIVE_GET_LOCAL(2);
	num = NATIVE_GET_LOCAL(3);

	if(OBJ_GET_TYPE(name) != OBJ_TYPE_STR ||
		OBJ_GET_TYPE(data) != OBJ_TYPE_STR ||
		OBJ_GET_TYPE(addr) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(num) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cname = (const char*)((pPmString_t)name)->val;
	caddr = ((pPmInt_t)addr)->val;
	cnum = ((pPmInt_t)num)->val + 1;
	cdata = (void*)((pPmString_t)data)->val;

	pm_sram_write(cname, caddr, cdata, cnum);
	return retval;
	"""
	pass

def read_string(name, addr, length):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t name, addr, num, text;
	const char *cname;
	size_t cnum;
	uint16_t caddr;
	char *cdata, *cpy;

	if (NATIVE_GET_NUM_ARGS() != 3) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	name = NATIVE_GET_LOCAL(0);
	addr = NATIVE_GET_LOCAL(1);
	num = NATIVE_GET_LOCAL(2);

	if(OBJ_GET_TYPE(name) != OBJ_TYPE_STR ||
		OBJ_GET_TYPE(addr) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(num) != OBJ_TYPE_INT) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	cname = (const char*)((pPmString_t)name)->val;
	caddr = ((pPmInt_t)addr)->val;
	cnum = ((pPmInt_t)num)->val + 1;
	cpy = cdata = kzalloc(cnum);

	pm_sram_read(cname, caddr, cdata, cnum);

	string_new((uint8_t const **)&cdata, &text);
	kfree(cpy);

	NATIVE_SET_TOS(text);
	return retval;
	"""
	pass

