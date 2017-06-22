#
#   ETA/OS - EEPROM library
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

__name__ = "eeprom"

class EEPROM(object):
	name = ""

	def __init__(self, name):
		self.name = name
	
	def read(self, addr, num):
		return read(self.name, addr, num)

	def write(self, addr, data, num):
		write(self.name, addr, data, num)

def read(name, addr, num):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t addr, num, list, name;
	uint8_t rawaddr;
	size_t rawnum;
	void *dst;
	const char *cname;

	if (NATIVE_GET_NUM_ARGS() != 3) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	name = NATIVE_GET_LOCAL(0);
	addr = NATIVE_GET_LOCAL(1);
	num = NATIVE_GET_LOCAL(2);

	if(OBJ_GET_TYPE(addr) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(num) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(name) != OBJ_TYPE_STR) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	rawaddr = ((pPmInt_t)addr)->val & 0xFF;
	rawnum  = ((pPmInt_t)num)->val;
	cname = (const char *)((pPmString_t)name)->val;
	dst = kzalloc(rawnum);

	pm_ee_read(cname, rawaddr, dst, rawnum);
	list = list_create_from_ptr(dst, rawnum);
	kfree(dst);

	NATIVE_SET_TOS(list);
	return retval;
	"""
	pass

def write(name, addr, data, num):
	"""__NATIVE__
	PmReturn_t retval = PM_RET_OK;
	pPmObj_t addr, data, len, name;
	uint8_t rawaddr;
	size_t rawsize;
	const char *cname;
	void *dst;

	if (NATIVE_GET_NUM_ARGS() != 4) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	name = NATIVE_GET_LOCAL(0);
	addr = NATIVE_GET_LOCAL(1);
	data = NATIVE_GET_LOCAL(2);
	len = NATIVE_GET_LOCAL(3);

	if(OBJ_GET_TYPE(addr) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(data) != OBJ_TYPE_LST ||
		OBJ_GET_TYPE(len) != OBJ_TYPE_INT ||
		OBJ_GET_TYPE(name) != OBJ_TYPE_STR) {
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}

	rawaddr = ((pPmInt_t)addr)->val & 0xFF;
	rawsize = ((pPmInt_t)len)->val;
	cname = (const char*)((pPmString_t)name)->val;
	dst = kzalloc(rawsize);

	list_to_byte_array(data, dst, rawsize);
	pm_ee_write(cname, rawaddr, dst, rawsize);

	kfree(dst);
	return retval;
	"""
	pass

