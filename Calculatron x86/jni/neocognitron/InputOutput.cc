/*
 * License:
 * This license is derived from the original wrote by K. Fukushima (See README.TXT)
 *
 * You are free:
 *  To copy, distribute, display, and perform the work (this computer program),
 *  To make derivative works,
 * Under the following conditions:
 *  Attribution:
 *      You must give the original author credit.
 *      When you publish any of your works related to, or based on,
 *      this original work or derivative works,
 *      you must cite the reference papers listed above.
 *  Non-commercial:
 *      You may not use this work, or derivative works, for commercial purposes,
 *      unless you get permission from the original author of Neocognitron (K. Fukushima)
 *      or Calculatron (Carlos Edmundo Martínez Mendoza).
 *  Share Alike:
 *      If you alter, transform, or build upon this work,
 *      you may distribute the resulting work
 *      only under a license identical to this one.
 * For any reuse or distribution, you must make clear to others
 * the license terms of this work.
 *
 *	File: InputOutput.cc
 *  Created on: 30/09/2013
 *      Authors: K. Fukushima, Carlos Edmundo Martínez Mendoza
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "opencv/cv.h"
#include "MemoryManage.h"
#include "InputOutput.h"

void rwcon(	// read/write connections.  Also get memores as and bs, in the read mode
		FILE *fpC,		// (in) file pointer
		const char fnameC[],	// (in) file name
		double *****as,	// (i/o) exc. connect. to S-cells: as[L][Ks][Kc][Na][Na]
		double **bs,		// (i/o) inh. connect. to S-cells: bs[L][Ks]
		int32_t **cat0,	// (i/o) category of the cell-plane UsL: cat0[1][KsL]=cat[Ks]
		int32_t *Na,		// (in) size of connect. as
		int32_t *Kc,		// (i/o) no. of C-cell-planes: Kc[L+1]
		int32_t *Ks,		// (i/o) no. of S-cell-planes: Ks[L]
		int32_t *Mas,		// (i/o) no. of active as[L][Ks][Kc]: Mas[L]
		int32_t L,			// number of stages
		int32_t learn// switch:  if (learn>0) write, else read.  If (learn<0) up to (-learn-1)
		) {
	double **ma;
	int32_t l, isize, Na2, ks, kc, m, Lread;

	if (learn > 0) {							// write
		// Do nothing.
	} else if (learn != -1) {	// if (learn==0, or <=-2), read
		if (learn == 0) {		// if (learn==0), read all stages
			Lread = L;
		} else {				// if (learn<=-2), read up to (-lean-1)th stage
			Lread = -learn - 1;
		}

		fpC = fopen(fnameC, "rb");
		if (fpC == NULL) {
			LOGDN("File open error fpC: %s\n", fnameC);
			exit(1);
		}

		isize = fread(Kc, sizeof(int), L + 1, fpC);
		if (isize != L + 1)
			goto error;

		isize = fread(Ks, sizeof(int), L, fpC);
		if (isize != L)
			goto error;

		isize = fread(Mas, sizeof(int), L, fpC);
		if (isize != L)
			goto error;

		for (l = 0; l < Lread; l++) {
			as[l] = geta4D1(Ks[l]);
			add4D2(as[l], Ks[l], Kc[l], Ks[l]);	// get Kc[l]*Ks[l] elements to pointer array
			Na2 = Na[l] * Na[l];
			for (ks = 0; ks < Ks[l]; ks++) {
				for (kc = 0; kc < Kc[l]; kc++) {
					as[l][ks][kc] = NULL;
				}
			}
			ma = get2D(Na[l] * Mas[l], Na[l]);		// get Mas arrays [Na][Na]
			for (m = 0; m < Mas[l]; m++) {
				isize = fread(&ks, sizeof(int), 1, fpC);
				if (isize != 1)
					goto error;
				isize = fread(&kc, sizeof(int), 1, fpC);
				if (isize != 1)
					goto error;
				as[l][ks][kc] = ma;
				isize = fread(ma[0], sizeof(double), Na2, fpC);
				if (isize != Na2)
					goto error;
				ma += Na[l];
			}

			bs[l] = get1D(Ks[l]);
			isize = fread(bs[l], sizeof(double), Ks[l], fpC);
			if (isize != Ks[l])
				goto error;
		}
		if (learn == 0) {			// Namely, Lread == L
			cat0[0] = get1Di(Ks[L - 1]);
			isize = fread(cat0[0], sizeof(int), Ks[L - 1], fpC);
			if (isize != Ks[L - 1])
				goto error;
		}
	}

	LOGDN("Scale of the network:\n");
	for (l = 0; l < L; l++) {
		LOGDN("Stage %d:        Ks = %d,      Mas = %d\n", l + 1, Ks[l],
				Mas[l]);
	}LOGDN("\n");
	fclose(fpC);
	return;

	error: LOGDN("i/o error in fpC:       isize=%d\n", isize);
	fclose(fpC);
	return;
}

void pat(				// get a pattern (from ETL1 file or memory)
		double **u,		// (out) generated pattern: u[N][N]
		int32_t N,			// size of the layer:  N*N = 64*64 or 65*65
		const IplImage *image) {
	int32_t i, j;
	int32_t withStep = image->widthStep;
	const char *image_data = image->imageData;
	double pix;

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++) {
			pix = (double) image_data[i * withStep + j];
			if (pix != 0)
				u[i][j] = 255.0 / (pix < 0 ? -pix : pix);
			else
				u[i][j] = 0;
		}

}
