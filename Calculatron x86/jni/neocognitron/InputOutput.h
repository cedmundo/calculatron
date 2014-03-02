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
 *	File: InputOutput.h
 *  Created on: 30/09/2013
 *     Authors: K. Fukushima, Carlos Edmundo Martínez Mendoza
 */

#ifndef INPUTOUTPUT_H_
#define INPUTOUTPUT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "opencv/cv.h"
#include "NeoDebug.h"
#include <stdint.h>

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
		);
void pat(				// get a pattern (from ETL1 file or memory)
		double **u,		// (out) generated pattern: u[N][N]
		int32_t N,			// size of the layer:  N*N = 64*64 or 65*65
		const IplImage *image
		);
#ifdef __cplusplus
}
#endif

#endif /* INPUTOUTPUT_H_ */
