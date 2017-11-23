ETA/OS README
=============

About
-----

ETA/OS is a small scale, embedded kernel and/or operating system. It provides
basic hardware abstraction allowing an application programmer to build somewhat
portable embedded applications. Some of its features:

  * Memory management
  * Scheduler (semi-SMP ready)
  * Interrupt management
  * Timer management
  * Driver core
  * Several device and sensor drivers

License
-------

ETA/OS is licenced as GNU Lesser General Public Licence version 3 or newer. 
The COPYING file should contian more information about the licence, or
if it's not included, look at http://www.gnu.org/licences/.

    ETA/OS
    Copyright (C) 2014, 2015, 2016, 2017  ETA/OS developers <etaos@googlegroups.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

Compiling ETA/OS
----------------

In order to build ETA/OS you need a working cross compile toolchain and a
host toolchain to build the DevOps applications. The entire setup process
is described [here](http://etaos.bietje.net/docs/env-setup.html). The
build scripts has a builtin help function. Run `make help ARCH=<arch>` to
see all available targets.

Testing
-------

ETA/OS comes with a set of unit tests, which can be found in the `usr/tests`
directory. These tests can be ran manually, ofcourse, or the running process
can be automated for most tests using a tool called Calypso. Calypso can be
download as a Ruby gem:

`user@pc# gem install calypso`

Running all available unit tests can be done with the following command:

`user@pc$ calypso -yfc usr/calypso.yaml`

For test configurations, please check the Calypso help: `calypso -h` command.

Contributing
------------

To see how you can help improving ETA/OS, read the contribution guide. You can
find it in the CONTRIBUTING.md file in the repository or on the
[web](http://etaos.bietje.net/contrib.html).

Trouble shooting
----------------

* Solving the avrdude timeout for the Arduino Mega / ATmega2560:

For some reason, avrdude is sometimes unable to properly communicate
with the ATmega2560. This solution can be tried when you receive the
following error:

	avrdude: stk500v2_ReceiveMessage(): timeout

If you run into this error, there are 3 'scripts' provided to work around
it:

`user@pc$ scripts/avrdude_timeout_fix [DEVICE_FILE]`

Now you can upload your binaries to the ATmega.

If you would like a tty-fix script and an upload in one, then you should give
either calypso (for unit tests) or `scripts/avr_upload` a try.

`user@pc$ scripts/avr_upload -h`

Please note that these are not 'run once and be good forever' scripts. They need
to be ran before _every_ upload.

