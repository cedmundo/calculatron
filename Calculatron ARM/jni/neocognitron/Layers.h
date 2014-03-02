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
 *	File: Layers.h
 *  Created on: 30/09/2013
 *     Authors: K. Fukushima, Carlos Edmundo Martínez Mendoza
 */

#ifndef LAYERS_H_
#define LAYERS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void Sres(				// response of S-layer
		double ***us,		// (out) response of S-cells: us[Ks][Ns][Ns]
		double ***uc,		// (in) response of C-cells: uc[Kc][Nc][Nc]
		double ****as,	// (in) exc. connections to S-cells: as[Ks][Kc][Na][Na]
		double **cs,		// (in) exc. connections to V-cells: cs[Na][Na]
		double *bs,		// (in) inh. connections to S-cells: bs[Ks]
		int32_t Nc,			// size of the C-layer: (nc*2) or (nc*2+1)
		int32_t Kc,			// number of cell-planes in C-layer
		int32_t Ns,			// size of the S-layer: (ns*2) or (ns*2+1)
		int32_t Ks,			// numberof cell-planes in S-layer
		int32_t Na,			// size of the connections (as & cs): (na*2) or (na*2+1)
		int32_t NcMAX,		// max of Nc (and Ng) among all stages
		int32_t NsMAX,		// max of Ns among all stages
		double theta		// threshold of an S-cell
		);
void Cres(				// response of C-layer
		double ***uc,		// (out) response of C-cells: uc[K][Nc][Nc]
		double ***us,		// (in) response of S-cells: us[K][Ns][Ns]
		double **ac,		// (in) exc. connections to C-cells: ac[Nd][Nd]
		int32_t Ns,			// size of the S-layer: (ns*2) or (ns*2+1)
		int32_t Nc,			// size of the C-layer: (nc*2) or (nc*2+1)
		int32_t mc,			// ratio of thinning-out
		int32_t K,			// number of cell-planes in S- and C-layers (K=Ks=Kc)
		int32_t Nd,			// size of the connections (ac): (nd*2) or (nd*2+1)
		int32_t NsMAX		// max of Ns among all stages
		);
int32_t CresL(				// response of C-layer without lateral inhibition
		double ***uc,		// (out) response of C-cells: uc[Kc][1][1]
		double ***us,		// (in) response of S-cells: us[K][Ns][Ns]
		int32_t *cat,		// (in) category of the S-plane: cat[Ks]
		int32_t Ns,			// size of the S-layer: (ns*2) or (ns*2+1)
		int32_t Ks,			// number of cell-planes in S-layers (Ks)
		int32_t Kc			// number of cell-planes in C-layers (Kc)
		);
void onoff(				// response of on- & off-center cell layers
		double ***uc,// (out) response of on- & off-center cells: uc[2][Nc][Nc]
		double **us,		// (in) response of receptor cells: us[Ns][Ns]
		double **cg,		// (in) connections for on-center: cg[Ng][Ng]
		int32_t Ns,			// size of the receptor layer: (ns*2) or (ns*2+1)
		int32_t Nc,	// size of the on- & off-center cell layer: (nc*2) or (nc*2+1)
		int32_t Ng			// size of the connections (ag): (ng*2) or (ng*2+1)
		);

#ifdef __cplusplus
}
#endif

#endif /* LAYERS_H_ */
