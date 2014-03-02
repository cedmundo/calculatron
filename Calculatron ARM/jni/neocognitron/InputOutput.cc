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

void rwcon(FILE *fpC, const char fnameC[], double *****as, double **bs,
		int32_t **cat0, int32_t *Na, int32_t *Kc, int32_t *Ks, int32_t *Mas,
		int32_t L, int32_t learn) {
	double **ma;
	int32_t l, isize, Na2, ks, kc, m, Lread;

	if (learn > 0) {
	} else if (learn != -1) {
		if (learn == 0) {
			Lread = L;
		} else {
			Lread = -learn - 1;
		}

		fpC = fopen(fnameC, "rb");
		if (fpC == NULL) {
			LOGDN("File open error fpC: %s\n", fnameC);
			exit(1);
		}

		isize = fread(Kc, sizeof(int32_t), L + 1, fpC);
		if (isize != L + 1)
			goto error;

		isize = fread(Ks, sizeof(int32_t), L, fpC);
		if (isize != L)
			goto error;

		isize = fread(Mas, sizeof(int32_t), L, fpC);
		if (isize != L)
			goto error;

		for (l = 0; l < Lread; l++) {
			as[l] = geta4D1(Ks[l]);
			add4D2(as[l], Ks[l], Kc[l], Ks[l]);
			Na2 = Na[l] * Na[l];
			for (ks = 0; ks < Ks[l]; ks++) {
				for (kc = 0; kc < Kc[l]; kc++) {
					as[l][ks][kc] = NULL;
				}
			}
			ma = get2D(Na[l] * Mas[l], Na[l]);
			for (m = 0; m < Mas[l]; m++) {
				isize = fread(&ks, sizeof(int32_t), 1, fpC);
				if (isize != 1)
					goto error;
				isize = fread(&kc, sizeof(int32_t), 1, fpC);
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
		if (learn == 0) {
			cat0[0] = get1Di(Ks[L - 1]);
			isize = fread(cat0[0], sizeof(int32_t), Ks[L - 1], fpC);
			if (isize != Ks[L - 1])
				goto error;
		}
	}

	LOGDN("Scale of the network:");
	for (l = 0; l < L; l++) {
		LOGDN("Stage %d: Ks = %d, Mas = %d", l + 1, Ks[l],
				Mas[l]);
	}

	fclose(fpC);
	return;

	error: LOGDN("I/O error in connections file, isize=%d\n", isize);
	fclose(fpC);
	return;
}

void pat(double **u, int32_t N, const IplImage *image) {
	int32_t i, j;
	int32_t withStep = image->widthStep;
	const char *image_data = image->imageData;
	double pix;

	for (i = 0; i < N; i++)
		for (j = 0; j < N; j++) {
			pix = (double) image_data[i * withStep + j];
			if (pix != 0)
				u[i][j] = (pix < 0 ? -pix : pix) / 255.0;
			else
				u[i][j] = 0;
		}

}
