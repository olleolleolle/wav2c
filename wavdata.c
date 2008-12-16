/***************************************************************
 * Name:      wave2c, a WAV file to GBA C source converter.
 * Purpose:   translate audio binaries into AVR memory data
 * Author:    Ino Schlaucher (ino@blushingboy.org)
 * Created:   2008-07-28
 * Copyright: Ino Schlaucher (http://blushingboy.org)
 * License:   GPLv3 (upgrading previous version)
 **************************************************************
 *
 * Based on an original piece of code by Mathieu Brethes.
 *
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

#include "wavdata.h"
#include <stdlib.h>

/* Loads a wave header in memory, and checks for its validity. */
/* returns NULL on error, a malloced() wavSound* otherwise.    */
wavSound * loadWaveHeader(FILE * fp) {
	char c[5];
	int nbRead;
	int chunkSize;
	int subChunk1Size;
	int subChunk2Size;
	short int audFormat;
	short int nbChannels;
	int sampleRate;
	int byteRate;
	short int blockAlign;
	short int bitsPerSample;
	wavSound *w;

	c[4] = 0;

	nbRead=fread(c, sizeof(char), 4, fp);

	// EOF ?
	if (nbRead < 4) return NULL;

	// Not a RIFF ?
	if (strcmp(c, "RIFF") != 0) {
		printf("Not a RIFF: %s\n", c);
		return NULL;
	}

	nbRead=fread(&chunkSize, sizeof(int), 1, fp);

	// EOF ?
	if (nbRead < 1) return NULL;

	nbRead=fread(c, sizeof(char), 4, fp);

	// EOF ?
	if (nbRead < 4) return NULL;

	// Not a WAVE riff ?
	if (strcmp(c, "WAVE") != 0) {
		printf("Not a WAVE: %s\n", c);
		return NULL;
	}

	nbRead=fread(c, sizeof(char), 4, fp);

	// EOF ?
	if (nbRead < 4) return NULL;

	// Not a "fmt " subchunk ?
	if (strcmp(c, "fmt ") != 0) {
		printf("No fmt subchunk: %s\n", c);
		return NULL;
	}

	// read size of chunk.
	nbRead=fread(&subChunk1Size, sizeof(int), 1, fp);
	if (nbRead < 1) return NULL;

	// is it a PCM ?
	if (subChunk1Size != 16) {
		printf("Not PCM fmt chunk size: %x\n", subChunk1Size);
		return NULL;
	}

	nbRead=fread(&audFormat, sizeof(short int), 1, fp);
	if (nbRead < 1) return NULL;

	// is it PCM ?
	if (audFormat != 1) {
		printf("No PCM format (1): %x\n", audFormat);
		return NULL;
	}

	nbRead=fread(&nbChannels, sizeof(short int), 1, fp);
	if (nbRead < 1) return NULL;

	// is it mono or stereo ?
	if (nbChannels > 2 || nbChannels < 1) {
		printf("Number of channels invalid: %x\n", nbChannels);
		return NULL;
	}

	nbRead=fread(&sampleRate, sizeof(int), 1, fp);
	if (nbRead < 1) return NULL;

	nbRead=fread(&byteRate, sizeof(int), 1, fp);
	if (nbRead < 1) return NULL;

	nbRead=fread(&blockAlign, sizeof(short int), 1, fp);
	if (nbRead < 1) return NULL;

	nbRead=fread(&bitsPerSample, sizeof(short int), 1, fp);
	if (nbRead < 1) return NULL;

	nbRead=fread(c, sizeof(char), 4, fp);

	// EOF ?
	if (nbRead < 4) return NULL;

	// Not a data section ?
	if (strcmp(c, "data") != 0) {
		printf("Not a data subchunk: %s\n", c);
		return NULL;
	}

	nbRead=fread(&subChunk2Size, sizeof(int), 1, fp);
	if (nbRead < 1) return NULL;

	// Now we can generate the structure...

	w = (wavSound *) malloc(sizeof(wavSound));
	// out of memory ?
	if (w == NULL) {
		printf("Out of memory, sorry\n");
		return w;
	}

	w->sampleRate = sampleRate;
	w->numChannels = nbChannels;
	w->bitsPerSample = bitsPerSample;
	w->dataLength = subChunk2Size;

	return w;
}

/* Loads the actual wave data into the data structure. */
void saveWave(FILE * fpI, wavSound *s, FILE * fpO, char * name) {
    saveWave_(fpI, s, fpO, name, -1);
}
void saveWave_(FILE * fpI, wavSound *s, FILE * fpO, char * name, int MaxSamples) {
	long filepos;
	int i;
	int realLength, auxLength;
	unsigned char stuff8;

	filepos = ftell(fpI);

	/* Print general information) */
	fprintf(fpO, "// %s sound made by wav2c\n\n", name);
	fprintf(fpO, "// const int %s_sampleRate = %d;\n", name, s->sampleRate);

    /* Chose the amount of samples to send to the text file
     * limiting the size to the file
     */
    realLength = (s->dataLength / s->numChannels / s->bitsPerSample * 8);
    if (MaxSamples != -1 && realLength > MaxSamples)
        realLength = MaxSamples;

	fprintf(fpO, "const int %s_length = %d;\n\n", name, realLength);

	/* Is it a stereo file ? */
	if (s->numChannels == 2) {
		fprintf(fpO, "const signed char %s_dataL[]= {", name);
		/* 8-bit ? convert 0-255 to -128-127 */
		if (s->bitsPerSample == 8) {
			for (i = 0 ; i < realLength ; i++) {
				fread(&stuff8, sizeof(unsigned char), 1, fpI);
				fprintf(fpO, "%d, ", -128 + stuff8);
				// read right output and forget about it
				fread(&stuff8, sizeof(unsigned char), 1, fpI);
				if ((i % 20) == 0) fprintf(fpO, "\n");
			}
			// reset file position;
			fseek(fpI, filepos, SEEK_SET);
			fprintf(fpO, "};\n\n");
			fprintf(fpO, "const signed char %s_dataR[]={", name);
			for (i = 0 ; i < realLength ; i++) {
				// read left output and forget about it
				fread(&stuff8, sizeof(unsigned char), 1, fpI);
				fread(&stuff8, sizeof(unsigned char), 1, fpI);
				fprintf(fpO, "%d, ", -128 + stuff8);
				if ((i % 20) == 0) fprintf(fpO, "\n");
			}
			fprintf(fpO, "};\n");
		}
		/* 16-bit ? convert signed 16 to signed 8 */
		else {
			for (i = 0 ; i < realLength ; i++) {
				// We take only MSB of wave data...
				fread(&stuff8, sizeof(char), 1, fpI);
				fread(&stuff8, sizeof(char), 1, fpI);
				fprintf(fpO, "%d, ", (signed char)stuff8);
				// read right output and forget about it
				fread(&stuff8, sizeof(char), 1, fpI);
				fread(&stuff8, sizeof(char), 1, fpI);
				if ((i % 20) == 0) fprintf(fpO, "\n");
			}
			// reset file position;
			fseek(fpI, filepos, SEEK_SET);
			fprintf(fpO, "};\n\n");
			fprintf(fpO, "const signed char %s_dataR[]={", name);
			for (i = 0 ; i < realLength ; i++) {
				// read left output and forget about it
				fread(&stuff8, sizeof(char), 1, fpI);
				fread(&stuff8, sizeof(char), 1, fpI);
				fread(&stuff8, sizeof(char), 1, fpI);
				fread(&stuff8, sizeof(char), 1, fpI);
				fprintf(fpO, "%d, ", (signed char)stuff8);
				if ((i % 20) == 0) fprintf(fpO, "\n");
			}
			fprintf(fpO, "};\n");
		}
	}
	/* Monaural file */
	/** PATCHED FOR ARDUINO **/
	else {
		fprintf(fpO, "const signed char %s_data[] PROGMEM ={", name);
		if (s->bitsPerSample == 8) {
			for (i = 0 ; i < realLength ; i++) {
				fread(&stuff8, sizeof(unsigned char), 1, fpI);
				fprintf(fpO, "%d, ", stuff8);
				if ((i % 20) == 0) fprintf(fpO, "\n");
			}
			fprintf(fpO, "};\n");
		} else {
			for (i = 0 ; i < realLength ; i++) {
				fread(&stuff8, sizeof(char), 1, fpI);
				fread(&stuff8, sizeof(char), 1, fpI);
				fprintf(fpO, "%d, ", (signed char)stuff8);
				if ((i % 20) == 0) fprintf(fpO, "\n");
			}
			fprintf(fpO, "};\n");
		}
	}
}




