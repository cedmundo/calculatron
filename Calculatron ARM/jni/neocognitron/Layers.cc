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
 *	File: Layers.cc
 *  Created on: 30/09/2013
 *     Authors: K. Fukushima, Carlos Edmundo Martínez Mendoza
 */

#include <stdlib.h>
#include <math.h>

#include "Layers.h"
#include "MemoryManage.h"

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
		int32_t Na,		// size of the connections (as & cs): (na*2) or (na*2+1)
		int32_t NcMAX,		// max of Nc (and Ng) among all stages
		int32_t NsMAX,		// max of Ns among all stages
		double theta		// threshold of an S-cell
		) {
	static int32_t init = 1;	// flag indicationg initial entry
	static int32_t *nsmax;		// (work) upper bound of the loop: nsmax[NcMAX]
	static int32_t *nsmin;		// (work) lower bound of the loop: nsmin[NcMAX]
	static int32_t *namax;	// (work) upper bound of the conection: namax[NcMAX]
	static double **vs2;	// (work) response**2 of V-cells: vs2[NsMAX][NsMAX]
	int32_t ns2 = Ns - 1;
	int32_t nc2 = Nc - 1;
	int32_t na2 = Na - 1;
	int32_t na0 = (na2 - nc2 + ns2) / 2;// na0=an-cn+sn; sn=ns2*0.5, cn=nc2*0.5, an=na2*0.5
	double theta1 = theta / (1 - theta);
	int32_t is, js, ks, ic, jc, kc, ia, ja, ic1;
	double uu, uu2, aas, vv, thv;

	if (init != 0) {
		nsmax = get1Di(NcMAX);
		nsmin = get1Di(NcMAX);
		namax = get1Di(NcMAX);
		vs2 = get2D(NsMAX, NsMAX);
		init = 0;
	}
	for (ic = 0; ic < Nc; ic++) {
		nsmax[ic] = ic + na0;					// = ic - cn + an + sn;
		if (nsmax[ic] > ns2)
			nsmax[ic] = ns2;
		ic1 = nc2 - ic;
		nsmin[ic1] = ns2 - nsmax[ic];
		namax[ic1] = ic1 - nsmin[ic1] + na0;// =(ic-cn) - (nsmin[ic]-sn) + an;
	}
	for (ks = 0; ks < Ks; ks++) {				// claer us & usma
		for (is = 0; is < Ns; is++) {
			for (js = 0; js < Ns; js++) {
				us[ks][is][js] = 0.0;
			}
		}
	}
	for (is = 0; is < Ns; is++) {				// claer vs2
		for (js = 0; js < Ns; js++) {
			vs2[is][js] = 0.0;
		}
	}
	for (kc = 0; kc < Kc; kc++) {	// send signals from uc to us & vs2
		for (ic = 0; ic < Nc; ic++) {
			for (jc = 0; jc < Nc; jc++) {
				if ((uu = uc[kc][ic][jc]) > 0.0) {
					uu2 = uu * uu;
					for (is = nsmin[ic], ia = namax[ic]; is <= nsmax[ic];
							is++, ia--) {
						for (js = nsmin[jc], ja = namax[jc]; js <= nsmax[jc];
								js++, ja--) {
							vs2[is][js] += cs[ia][ja] * uu2;
						}
					}
					for (ks = 0; ks < Ks; ks++) {
						if (as[ks][kc] != NULL) {// sarting adress in the memory as
							for (is = nsmin[ic], ia = namax[ic];
									is <= nsmax[ic]; is++, ia--) {
								for (js = nsmin[jc], ja = namax[jc];
										js <= nsmax[jc]; js++, ja--) {
									if ((aas = as[ks][kc][ia][ja]) > 0.0) {
										us[ks][is][js] += aas * uu;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	for (is = 0; is < Ns; is++) {
		for (js = 0; js < Ns; js++) {
			if ((vv = vs2[is][js]) > 0.0) {
				thv = theta * sqrt(vv);
				for (ks = 0; ks < Ks; ks++) {
					if ((uu = us[ks][is][js]) > 0.0) {
						uu = (1.0 + uu) / (1.0 + thv * bs[ks]);
						if (uu > 1.0) {
							us[ks][is][js] = theta1 * (uu - 1.0);
						} else {
							us[ks][is][js] = 0.0;
						}
					}
				}
			}
		}
	}
}

void Cres(				// response of C-layer
		double ***uc,		// (out) response of C-cells: uc[K][Nc][Nc]
		double ***us,		// (in) response of S-cells: us[K][Ns][Ns]
		double **ac,		// (in) exc. connections to C-cells: ac[Nd][Nd]
		int32_t Ns,			// size of the S-layer: (ns*2) or (ns*2+1)
		int32_t Nc,			// size of the C-layer: (nc*2) or (nc*2+1)
		int32_t mc,			// ratio of thinning-out
		int32_t K,		// number of cell-planes in S- and C-layers (K=Ks=Kc)
		int32_t Nd,			// size of the connections (ac): (nd*2) or (nd*2+1)
		int32_t NsMAX		// max of Ns among all stages
		) {
	static int32_t init = 1;	// flag indicationg initial entry
	static int32_t *ncmax;		// (work) upper bound of the loop: ncmax[NsMAX]
	static int32_t *ncmin;		// (work) lower bound of the loop: ncmin[NsMAX]
	static int32_t *namax;	// (work) upper bound of the conection: namax[NsMAX]
	int32_t nc2 = Nc - 1;			// cn=nc2*0.5
	int32_t ns2 = Ns - 1;			// sn=ns2*0.5
	int32_t nd2 = Nd - 1;			// dn=nd2*0.5
	int32_t nd0 = (nc2 * mc - ns2 + nd2) / 2;// =dn0=cn*mc-sn+dn=(nc2*mc-ns2+nd2)*0.5
	int32_t k, ic, jc, is, js, ia, ja, is1;
	double uu;

	if (init != 0) {
		ncmax = get1Di(NsMAX);
		ncmin = get1Di(NsMAX);
		namax = get1Di(NsMAX);
		init = 0;
	}
	for (is = 0; is < Ns; is++) {
		ncmax[is] = (is + nd0) / mc;	// =floor((is-sn+dn+cn*mc)/mc)
		if ((is + nd0) < 0)
			ncmax[is]--;	// not necessary, except for Ns>>Nc
		if (ncmax[is] > nc2)
			ncmax[is] = nc2;
		is1 = ns2 - is;
		ncmin[is1] = nc2 - ncmax[is];
		namax[is1] = is1 - ncmin[is1] * mc + nd0;
		// = (is1-sn) - (ncmin[is1]-cn)*mc + dn;
		// = is1 - ncmin[is1]*mc + cn*mc-sn+dn;
		// = is1 - ncmin[is1]*mc + dn0;
	}
	for (k = 0; k < K; k++) {
		for (ic = 0; ic < Nc; ic++) {		// claer uc
			for (jc = 0; jc < Nc; jc++) {
				uc[k][ic][jc] = 0.0;
			}
		}
		for (is = 0; is < Ns; is++) {
			for (js = 0; js < Ns; js++) {
				if ((uu = us[k][is][js]) > 0.0) {
					for (ic = ncmin[is], ia = namax[is]; ic <= ncmax[is];
							ic++, ia -= mc) {
						for (jc = ncmin[js], ja = namax[js]; jc <= ncmax[js];
								jc++, ja -= mc) {
							uc[k][ic][jc] += ac[ia][ja] * uu;
						}
					}
				}
			}
		}
		for (ic = 0; ic < Nc; ic++) {			// calculate the response of uc
			for (jc = 0; jc < Nc; jc++) {
				if ((uu = uc[k][ic][jc]) > 0.0) {
					uc[k][ic][jc] = uu / (1.0 + uu);
				} else {
					uc[k][ic][jc] = 0.0;
				}
			}
		}
	}
}

int32_t CresL(				// response of C-layer without lateral inhibition
		double ***uc,		// (out) response of C-cells: uc[Kc][1][1]
		double ***us,		// (in) response of S-cells: us[K][Ns][Ns]
		int32_t *cat,		// (in) category of the S-plane: cat[Ks]
		int32_t Ns,			// size of the S-layer: (ns*2) or (ns*2+1)
		int32_t Ks,			// number of cell-planes in S-layers (Ks)
		int32_t Kc			// number of cell-planes in C-layers (Kc)
		) {
	int32_t is, js, ks, kc, Cat;
	double Usmax;

	for (kc = 0; kc < Kc; kc++) {
		uc[kc][0][0] = 0.0;
	}

	Cat = -1;
	Usmax = 0.0;
	for (ks = 0; ks < Ks; ks++) {
		for (is = 0; is < Ns; is++) {
			for (js = 0; js < Ns; js++) {
				if (us[ks][is][js] > Usmax) {
					Usmax = us[ks][is][js];
					Cat = cat[ks];
				}
			}
		}
	}

	if (Cat >= 0) {
		uc[Cat][0][0] = Usmax;
	}
	return Cat;
}

void onoff(				// response of on- & off-center cell layers
		double ***uc,// (out) response of on- & off-center cells: uc[2][Nc][Nc]
		double **us,		// (in) response of receptor cells: us[Ns][Ns]
		double **cg,		// (in) connections for on-center: cg[Ng][Ng]
		int32_t Ns,			// size of the receptor layer: (ns*2) or (ns*2+1)
		int32_t Nc,	// size of the on- & off-center cell layer: (nc*2) or (nc*2+1)
		int32_t Ng			// size of the connections (ag): (ng*2) or (ng*2+1)
		) {
	static int32_t init = 1;	// flag indicationg initial entry
	static int32_t *ncmax;		// (work) upper bound of the loop: ncmax[Ns]
	static int32_t *ncmin;		// (work) lower bound of the loop: ncmin[Ns]
	static int32_t *namax;	// (work) upper bound of the conection: namax[Ns]
	int32_t ns2 = Ns - 1;
	int32_t nc2 = Nc - 1;
	int32_t ng2 = Ng - 1;
	int32_t ng0 = (ng2 - ns2 + nc2) / 2;// ng0=gn-sn+cn; sn=ns2*0.5, cn=nc2*0.5, gn=ng2*0.5
	int32_t ic, jc, is, js, ia, ja, is1;
	double uu;

	if (init != 0) {
		ncmax = get1Di(Ns);
		ncmin = get1Di(Ns);
		namax = get1Di(Ns);
		init = 0;
	}
	for (is = 0; is < Ns; is++) {
		ncmax[is] = is + ng0;
		if (ncmax[is] > nc2)
			ncmax[is] = nc2;
		is1 = ns2 - is;
		ncmin[is1] = nc2 - ncmax[is];
		namax[is1] = is1 - ncmin[is1] + ng0;// =(is-sn) - (ncmin[is]-cn) + gn;
	}
	for (ic = 0; ic < Nc; ic++) {
		for (jc = 0; jc < Nc; jc++) {
			uc[0][ic][jc] = 0.0;
		}
	}
	for (is = 0; is < Ns; is++) {
		for (js = 0; js < Ns; js++) {
			if ((uu = us[is][js]) > 0.0) {
				for (ic = ncmin[is], ia = namax[is]; ic <= ncmax[is];
						ic++, ia--) {
					for (jc = ncmin[js], ja = namax[js]; jc <= ncmax[js];
							jc++, ja--) {
						uc[0][ic][jc] += cg[ia][ja] * uu;
					}
				}
			}
		}
	}
	for (ic = 0; ic < Nc; ic++) {
		for (jc = 0; jc < Nc; jc++) {
			if ((uu = uc[0][ic][jc]) > 0.0001) {
				uc[1][ic][jc] = 0.0;
			} else if (uu < -0.0001) {
				uc[0][ic][jc] = 0.0;
				uc[1][ic][jc] = -uu;
			} else {
				uc[0][ic][jc] = 0.0;
				uc[1][ic][jc] = 0.0;
			}
		}
	}
}

