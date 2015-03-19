wave2c, a WAV file to GBA C source converter
============================================

Translates audio binaries into AVR memory data.

Credits
-------
 
Author:    Ino Schlaucher <ino@blushingboy.org>

Created:   2008-07-28

Copyright: 2008 Ino Schlaucher [blushingboy.org](http://blushingboy.org)

License:   GPLv3 (upgrading previous version)

Based on an original piece of code by Mathieu Brethes.

Copyright (c) 2003 by Mathieu Brethes <thieumsweb@free.fr> [thieumsweb.free.fr](http://thieumsweb.free.fr/) 

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

How-To
------

1. Use Audacity to create the file with the right settings: 8KHz, 8bit, mono. Do it with WAV export.
2. Audacity will always leave a tail, making the file suck big time. Therefore trim it with: `sox fifiu.wav fifiu-cut.wav trim 0 10000s`
3. Compile wav2c with usual `make`
4. Export it to text with: `./wav2c fifiu-cut.wav fifiu.h sounddata`

Links
-----

[Audacity](http://audacity.sourceforge.net/): Free Audio Editor and Recorder

[SoX: Sound Exchange](http://sox.sourceforge.net/)
