/* wave2c, a WAV file to GBA C source converter.
 * Copyright (c) 2003 by Mathieu Brethes.
 *
 * Contact : thieumsweb@free.fr
 * Website : http://thieumsweb.free.fr/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __WAV_DATA__
#define __WAV_DATA__

#include <strings.h>
#include <stdio.h>

typedef struct {
	int sampleRate;
	int numChannels;
	int bitsPerSample;

	int dataLength;
} wavSound;

/* Loads a wave header in memory, and checks for its validity. */
/* returns NULL on error, a malloced() wavSound* otherwise.    */
wavSound * loadWaveHeader(FILE * fp);

/* Loads the actual wave data into the data structure. */
void saveWave(FILE * fpI, wavSound *s, FILE * fpO, char * name);
void saveWave_(FILE * fpI, wavSound *s, FILE * fpO, char * name, int MaxSamples);

#endif

