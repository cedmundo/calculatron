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
 *	File: SelfOrganitation.h
 *  Created on: 30/09/2013
 *     Authors: K. Fukushima, Carlos Edmundo Martínez Mendoza
 */

#ifndef SELFORGANITATION_H_
#define SELFORGANITATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

int32_t  Sorg(				// self-organization of S-layer
		double ****as,	// (out) exc. connections to S-cells: as[Ks][Kc][Na][Na]
		double *bs,		// (out) inh. connections to S-cells: bs[Ks], new Ks
		double ***uc,		// (in) response of C-cells: uc[Kc][Nc][Nc]
		double **cs,		// (in) spatial weight: cs[Na][Na]
		double **asini,	// (in) exc. initial connections to S-cells: as[Na][Na]
		double **cl,// (in) connect. for lat.inh. from us to usini: cl[Nl][Nl]
		double ***us,	// (work) response of S-cells: us[Ks][Ns][Ns], old Ks
		int32_t  Nc,			// size of the C-layer:  (nc*2) or (nc*2+1)
		int32_t  Kc,			// number of cell-planes in C-layer
		int32_t  Ns,			// size of the S-layer:  (ns*2) or (ns*2+1)
		int32_t  Ks,			// number of cell-planes in S-layer, old value
		int32_t  Na,			// size of connections (as & cs):  (na*2) or (na*2+1)
		int32_t  Nasini,		// size of connect. asini:  (nasini*2) or (nasini*2+1)
		int32_t  Nl,	// size of connection cl (lat. inh. from us to usini):  Nl=nl*2+1
		double D,			// radius of competition area
		double qs,			// parameter determining the speed of learning
		double theta,		// threshold of an S-cell
		int32_t  Ksmax,		// maximum possible Ks
		int32_t  KsMAX,		// max of KsMAX among all stages
		int32_t  NcMAX,		// max of Nc (and Ng) among all stages
		int32_t  NsMAX,		// max of Ns among all stages
		double thrini		// threshold for cutting usini
		);
void delta(				// strengthen connections
		double ****as,	// (out) exc. connections to S-cells: as[Ks][Kc][Na][Na]
		double *bs,		// (out) inh. connections to S-cells: bs[Ks]
		double ***uc,		// (in) response of C-cells: uc[Kc][Nc][Nc]
		double **cs,		// (in) spatial weight: cs[Na][Na]
		int32_t  Na,			// size of the connections (as & cs): (na*2) or (na*2+1)
		int32_t  Nc,			// size of the C-layer: (nc*2) or (nc*2+1)
		int32_t  Kc,			// numberof cell-planes in C-layer
		int32_t  Ns,			// size of the S-layer: (ns*2) or (ns*2+1)
		int32_t  kseed,		// cell-plane of the seed cell
		int32_t  iseed,		// x-coordinate of the seed cell
		int32_t  jseed,		// y-coordinate of the seed cell
		int32_t  KsMAX,		// max of KsMAX among all stages, because KcMAX <= KsMAX
		double qs			// parameter determining the speed of learning
		);
void seed(				// select seed cells
		int32_t  *iseed,		// (out) x-coordinate of the k-th cell-plane: iseed[Ks]
		int32_t  *jseed,		// (out) y-coordinate of the k-th cell-plane: jseed[Ks]
		double ***us,		// (in) response of S-cells: us[Ks][Ns][Ns]
		double **wmax,		// (work) working area: wmax[Ns][Ns]
		int32_t  **kwmax,	// (work) working area: kwmax[Ns][Ns]
		int32_t  Ns,			// size of the S-layer
		int32_t  Ks,			// number of cell-planes in S-layer
		int32_t  NsMAX,		// max of Ns among all stages
		double D			// radius of competition area
		);
void Sresini(			// response of seed-selecting plane
		double **usini,	// (out) response of seed-selecting plane: usini[Ns][Ns]
		double ***uc,		// (in) response of C-cells: uc[Kc][Nc][Nc]
		double **asini,	// (in) exc. initial connections to S-cells: as[Na][Na]
		int32_t  Nc,			// size of the C-layer: (nc*2) or (nc*2+1)
		int32_t  Kc,			// number of cell-planes in C-layer
		int32_t  Ns,			// size of the S-layer: (ns*2) or (ns*2+1)
		int32_t  Na,	// size of the connections (asini, as & cs): (na*2) or (na*2+1)
		int32_t  NcMAX,		// max of Nc (and Ng) among all stages
		double thrini		// threshold for cutting usini
		);
void seedini(			// select seed cells
		int32_t  *iseed0,	// (out) x-coordinate of the k-th cell-plane: iseed0[1]
		int32_t  *jseed0,	// (out) y-coordinate of the k-th cell-plane: jseed0[1]
		double **usini,	// (in) response of S-cells: usini[Ns][Ns]
		int32_t  Ns			// size of the S-layer
		);
void lat(				// lateral inhibition from uin to uout
		double **ut,		// (i/o) cell-plane to be inhibited: ut[N][N]
		double **uf,		// (in) inhibiting cell-plane: uf[N][N]
		double **cl,	// (in) connections for lateral inhibition: cl[Nl][Nl]
		int32_t  N,			// size of the layer
		int32_t  Nl			// size of connection cl:  Nl=nl*2+1
		);
int32_t  SorgL(// self-organization of S-layer (new cell-plane if winner is not from correct category)
		double ****as,	// (out) exc. connections to S-cells: as[Ks][Kc][Na][Na]
		double *bs,		// (out) inh. connections to S-cells: bs[Ks], new Ks
		int32_t  *cat,		// (out) category of the S-plane: cat[Ks]
		int32_t  Cat,		// (in) category of the training pattern
		double ***uc,		// (in) response of C-cells: uc[Kc][Nc][Nc]
		double **cs,		// (in) spatial weight: cs[Na][Na]
		double **asini,	// (in) exc. initial connections to S-cells: as[Na][Na]
		double ***us,	// (work) response of S-cells: us[Ks][Ns][Ns], old Ks
		int32_t  Nc,			// size of the C-layer:  (nc*2) or (nc*2+1)
		int32_t  Kc,			// number of cell-planes in C-layer
		int32_t  Ns,			// size of the S-layer:  (ns*2) or (ns*2+1)
		int32_t  Ks,			// number of cell-planes in S-layer, old value
		int32_t  Na,			// size of the connections (as & cs): (na*2) or (na*2+1)
		int32_t  Nasini,	// size of the connect. asini:  (nasini*2) or (nasini*2+1)
		double qs,			// parameter determining the speed of learning
		double theta,		// threshold of an S-cell
		int32_t  Ksmax,		// maximum possible Ks
		int32_t  KsMAX,		// max of KsMAX among all stages, because KcMAX <= KsMAX
		int32_t  NcMAX,		// max of Nc (and Ng) among all stages
		int32_t  NsMAX,		// max of Ns among all stages
		FILE *fpT// (in) file point32_t er for debugging.  If ==NULL, no debugging
		);

#ifdef __cplusplus
}
#endif

#endif /* SELFORGANITATION_H_ */
