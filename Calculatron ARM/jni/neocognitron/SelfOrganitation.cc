/*
 *License:
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
 *	File: SelfOrganitation.cc
 *  Created on: 30/09/2013
 *     Authors: K. Fukushima, Carlos Edmundo Martínez Mendoza
 */

#include <stdio.h>
#include <math.h>
#include <stdint.h>

#include "NeoDebug.h"
#include "MemoryManage.h"
#include "Layers.h"
#include "SelfOrganitation.h"

int32_t Sorg(				// self-organization of S-layer
		double ****as,	// (out) exc. connections to S-cells: as[Ks][Kc][Na][Na]
		double *bs,		// (out) inh. connections to S-cells: bs[Ks], new Ks
		double ***uc,		// (in) response of C-cells: uc[Kc][Nc][Nc]
		double **cs,		// (in) spatial weight: cs[Na][Na]
		double **asini,	// (in) exc. initial connections to S-cells: as[Na][Na]
		double **cl,// (in) connect. for lat.inh. from us to usini: cl[Nl][Nl]
		double ***us,	// (work) response of S-cells: us[Ks][Ns][Ns], old Ks
		int32_t Nc,			// size of the C-layer:  (nc*2) or (nc*2+1)
		int32_t Kc,			// number of cell-planes in C-layer
		int32_t Ns,			// size of the S-layer:  (ns*2) or (ns*2+1)
		int32_t Ks,			// number of cell-planes in S-layer, old value
		int32_t Na,		// size of connections (as & cs):  (na*2) or (na*2+1)
		int32_t Nasini,	// size of connect. asini:  (nasini*2) or (nasini*2+1)
		int32_t Nl,	// size of connection cl (lat. inh. from us to usini):  Nl=nl*2+1
		double D,			// radius of competition area
		double qs,			// parameter determining the speed of learning
		double theta,		// threshold of an S-cell
		int32_t Ksmax,		// maximum possible Ks
		int32_t KsMAX,		// max of KsMAX among all stages
		int32_t NcMAX,		// max of Nc (and Ng) among all stages
		int32_t NsMAX,		// max of Ns among all stages
		double thrini		// threshold for cutting usini
		) {
	static int32_t init = 1;	// flag indicationg initial entry
	static double **usini;// response of seed-selecting plane: usini[NsMAX][NsMAX]
	static double **work;		// for wmax[][] & us[Ks][][]: work[NsMAX][NsMAX]
	static int32_t **kwmax;	// kwmax[Ns][Ns].  Also used as a print-buffer when fpT!=NULL
	static int32_t *iseed;		// iseed[Ks], old Ks
	static int32_t *jseed;		// jseed[Ks], old Ks
	int32_t ks, iseed0, jseed0;

	if (init != 0) {
		usini = get2D(NsMAX, NsMAX);
		work = get2D(NsMAX, NsMAX);
		kwmax = get2Di(NsMAX, NsMAX);
		iseed = get1Di(KsMAX);
		jseed = get1Di(KsMAX);
		init = 0;
	}

	if (Ks > 0) {
		Sres(us, uc, as, cs, bs, Nc, Kc, Ns, Ks, Na, NcMAX, NsMAX, theta);
		// Sres, using old connections
		seed(iseed, jseed, us, work, kwmax, Ns, Ks, NsMAX, D);		// work=wmax

		for (ks = 0; ks < Ks; ks++) {
			if (iseed[ks] >= 0) {
				delta(as, bs, uc, cs, Na, Nc, Kc, Ns, ks, iseed[ks], jseed[ks],
						KsMAX, qs);
				// reinforce connect. for old cell-planes
				// memory for as[] may be increased by delta()
			}
		}

		Sres(us, uc, as, cs, bs, Nc, Kc, Ns, Ks, Na, NcMAX, NsMAX, theta);
		// Sres, using renewed connections
	}
	Sresini(usini, uc, asini, Nc, Kc, Ns, Nasini, NcMAX, thrini);
	for (ks = 0; ks < Ks; ks++) {
		lat(usini, us[ks], cl, Ns, Nl);			// lat.inh. from us to usini
	}
	for (ks = Ks; ks < Ksmax; ks++) {
		seedini(&iseed0, &jseed0, usini, Ns);// location of seed = max.-output cell
		if (iseed0 == -1)
			break;
		add4D2(as, ks + 1, Kc, Ksmax);	// add Kc elements to pointer array **as
		delta(as, bs, uc, cs, Na, Nc, Kc, Ns, ks, iseed0, jseed0, KsMAX, qs);
		// reinforce connect. for new cell-planes
		// memory for as[] may be increased by delta()
		// memory for bs[] must have been prepared beforehand
		Sres(&work, uc, &as[ks], cs, &bs[ks], Nc, Kc, Ns, 1, Na, NcMAX, NsMAX,
				theta);
		// Sres, for the new cell-plane
		lat(usini, work, cl, Ns, Nl);		// lat.inh. from new us to usini
	}
	return ks;
}

void delta(				// strengthen connections
		double ****as,	// (out) exc. connections to S-cells: as[Ks][Kc][Na][Na]
		double *bs,		// (out) inh. connections to S-cells: bs[Ks]
		double ***uc,		// (in) response of C-cells: uc[Kc][Nc][Nc]
		double **cs,		// (in) spatial weight: cs[Na][Na]
		int32_t Na,		// size of the connections (as & cs): (na*2) or (na*2+1)
		int32_t Nc,			// size of the C-layer: (nc*2) or (nc*2+1)
		int32_t Kc,			// numberof cell-planes in C-layer
		int32_t Ns,			// size of the S-layer: (ns*2) or (ns*2+1)
		int32_t kseed,		// cell-plane of the seed cell
		int32_t iseed,		// x-coordinate of the seed cell
		int32_t jseed,		// y-coordinate of the seed cell
		int32_t KsMAX,	// max of KsMAX among all stages, because KcMAX <= KsMAX
		double qs			// parameter determining the speed of learning
		) {
	static int32_t init = 1;
	static int32_t *ucmax;		// ucmax[KsMAX], because KcMAX <= KsMAX
	int32_t i, j, ic, jc, kc, icmin, icmax, jcmin, jcmax, iamin, jamin, icmin0,
			jcmin0;
	double **ma;
	int32_t m_inc;
	double uu, b2, cc, aa;
	int32_t ns2 = Ns - 1;						// sn=ns2*0.5
	int32_t nc2 = Nc - 1;						// cn=nc2*0.5
	int32_t na2 = Na - 1;						// an=na2*0.5
	int32_t na0 = (nc2 - ns2 + na2) / 2;		// na0=cn-sn+an
	int32_t na1 = (nc2 - ns2 - na2) / 2;		// na1=cn-sn-an

	if (init != 0) {
		ucmax = get1Di(KsMAX);			// ucmax[KsMAX], because KcMAX <= KsMAX
	}

	icmax = iseed + na0;				// = (iseed - sn) + an + cn;
	if (icmax > nc2)
		icmax = nc2;
	icmin = iseed + na1;				// = (iseed - sn) - an + cn;
	icmin0 = icmin;
	if (icmin < 0)
		icmin = 0;
	iamin = icmin - icmin0;
	jcmax = jseed + na0;				// = (jseed - sn) + an + cn;
	if (jcmax > nc2)
		jcmax = nc2;
	jcmin = jseed + na1;				// = (jseed - sn) - an + cn;
	jcmin0 = jcmin;
	if (jcmin < 0)
		jcmin = 0;
	jamin = jcmin - jcmin0;

	m_inc = 0;
	for (kc = 0; kc < Kc; kc++) {
		for (ic = icmin, i = iamin; ic <= icmax; ic++, i++) {
			for (jc = jcmin, j = jamin; jc <= jcmax; jc++, j++) {
				if (uc[kc][ic][jc] > 0.0) {	// if at least one positive cell exists
					ucmax[kc] = 1;
					if (as[kseed][kc] == NULL) {
						m_inc++;
					}
					goto skip;
				}
			}
		}
		ucmax[kc] = 0;
		skip: ;
	}
	ma = get2D(Na * m_inc, Na);			// get m_inc arrays [Na][Na]

	for (kc = 0; kc < Kc; kc++) {
		if (ucmax[kc] > 0) {
			if (as[kseed][kc] == NULL) {
				as[kseed][kc] = ma;
				ma += Na;
				for (i = 0; i < Na; i++) {
					for (j = 0; j < Na; j++) {
						as[kseed][kc][i][j] = 0.0;
					}
				}
			}
			for (ic = icmin, i = iamin; ic <= icmax; ic++, i++) {
				for (jc = jcmin, j = jamin; jc <= jcmax; jc++, j++) {
					if ((uu = uc[kc][ic][jc]) > 0.0) {
						if ((cc = cs[i][j]) > 0.0) {
							as[kseed][kc][i][j] += qs * cc * uu;
						}
					}
				}
			}
		}
	}
	b2 = 0.0;
	for (kc = 0; kc < Kc; kc++) {
		if (as[kseed][kc] != NULL) {
			for (i = 0; i < Na; i++) {
				for (j = 0; j < Na; j++) {
					if ((aa = as[kseed][kc][i][j]) > 0.0) {
						if ((cc = cs[i][j]) > 0.0) {
							b2 += aa * aa / cc;
						}
					}
				}
			}
		}
	}
	bs[kseed] = sqrt(b2);
}

void seed(				// select seed cells
		int32_t *iseed,	// (out) x-coordinate of the k-th cell-plane: iseed[Ks]
		int32_t *jseed,	// (out) y-coordinate of the k-th cell-plane: jseed[Ks]
		double ***us,		// (in) response of S-cells: us[Ks][Ns][Ns]
		double **wmax,		// (work) working area: wmax[Ns][Ns]
		int32_t **kwmax,	// (work) working area: kwmax[Ns][Ns]
		int32_t Ns,			// size of the S-layer
		int32_t Ks,			// number of cell-planes in S-layer
		int32_t NsMAX,		// max of Ns among all stages
		double D			// radius of competition area
		) {
	static int32_t init = 1;	// flag indicationg initial entry
	static int32_t *nmax;// (work) upper bound of the competition: nmax[NsMAX]
	static int32_t *nmin;	// (work) lower bound of the competition: nmin[Ns]
	double d2 = D * D;		// D**2 in the scale of this layer
	int32_t id = (int) D;
	int32_t k, i, j, ii, jj, iii, jjj, i2, r2, ks;
	double wm;

	if (init != 0) {
		nmax = get1Di(Ns);
		nmin = get1Di(Ns);
		init = 0;
	}
	for (i = 0; i < Ns; i++) {
		nmin[i] = i - id;
		if (nmin[i] < 0)
			nmin[i] = 0;
		nmax[i] = i + id;
		if (nmax[i] >= Ns)
			nmax[i] = Ns - 1;
	}

	for (k = 0; k < Ks; k++) {		// reset iseed & jseed
		iseed[k] = -1;
		jseed[k] = -1;
	}
	for (i = 0; i < Ns; i++) {		// competition among k at each location
		for (j = 0; j < Ns; j++) {
			wmax[i][j] = 0.0;
			kwmax[i][j] = -1;
			for (k = 0; k < Ks; k++) {
				if (us[k][i][j] > wmax[i][j]) {
					wmax[i][j] = us[k][i][j];
					kwmax[i][j] = k;
				}
			}
		}
	}
	for (i = 0; i < Ns; i++) {		// find a winner in each hypercolumn
		for (j = 0; j < Ns; j++) {
			if ((wm = wmax[i][j]) > 0.0) {
				for (ii = nmin[i]; ii <= nmax[i]; ii++) {
					iii = ii - i;
					i2 = iii * iii;
					for (jj = nmin[j]; jj <= nmax[j]; jj++) {
						jjj = jj - j;
						r2 = i2 + jjj * jjj;
						if (r2 <= d2 && r2 != 0.0) {
							if (wmax[ii][jj] > wm) {		// defeated
								kwmax[i][j] = -1;
								goto skip;
							} else if (wmax[ii][jj] == wm) {	// tie
								wmax[ii][jj] = wm * 0.9999;
							}
						}
					}
				}
			}
			skip: ;
		}
	}
	for (i = 0; i < Ns; i++) {		// find a winner for each cell-plane
		for (j = 0; j < Ns; j++) {
			if ((ks = kwmax[i][j]) >= 0) {
				if ((iseed[ks] < 0) ||	// first appearance of a winner in ks
						(wmax[i][j] > wmax[iseed[ks]][jseed[ks]])) {
					iseed[ks] = i;		// replace the seed
					jseed[ks] = j;
				}
			}
		}
	}
}

void Sresini(			// response of seed-selecting plane
		double **usini,	// (out) response of seed-selecting plane: usini[Ns][Ns]
		double ***uc,		// (in) response of C-cells: uc[Kc][Nc][Nc]
		double **asini,	// (in) exc. initial connections to S-cells: as[Na][Na]
		int32_t Nc,			// size of the C-layer: (nc*2) or (nc*2+1)
		int32_t Kc,			// number of cell-planes in C-layer
		int32_t Ns,			// size of the S-layer: (ns*2) or (ns*2+1)
		int32_t Na,	// size of the connections (asini, as & cs): (na*2) or (na*2+1)
		int32_t NcMAX,		// max of Nc (and Ng) among all stages
		double thrini		// threshold for cutting usini
		) {
	static int32_t init = 1;	// flag indicationg initial entry
	static int32_t *nsmax;		// (work) upper bound of the loop: nsmax[NcMAX]
	static int32_t *nsmin;		// (work) lower bound of the loop: nsmin[NcMAX]
	static int32_t *namax;	// (work) upper bound of the conection: namax[NcMAX]
	int32_t ns2 = Ns - 1;
	int32_t nc2 = Nc - 1;
	int32_t na2 = Na - 1;
	int32_t na0 = (na2 - nc2 + ns2) / 2;// na0=an-cn+sn; sn=ns2*0.5, cn=nc2*0.5, an=na2*0.5
	int32_t is, js, ic, jc, kc, ia, ja, ic1;
	double uu, usimax, usicut;

	if (init != 0) {
		nsmax = get1Di(NcMAX);
		nsmin = get1Di(NcMAX);
		namax = get1Di(NcMAX);
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
	for (is = 0; is < Ns; is++) {
		for (js = 0; js < Ns; js++) {
			usini[is][js] = 0.0;
		}
	}
	for (kc = 0; kc < Kc; kc++) {
		for (ic = 0; ic < Nc; ic++) {
			for (jc = 0; jc < Nc; jc++) {
				if ((uu = uc[kc][ic][jc]) > 0.0) {
					for (is = nsmin[ic], ia = namax[ic]; is <= nsmax[ic];
							is++, ia--) {
						for (js = nsmin[jc], ja = namax[jc]; js <= nsmax[jc];
								js++, ja--) {
							usini[is][js] += asini[ia][ja] * uu;
						}
					}
				}
			}
		}
	}
	usimax = 0.0;
	for (is = 0; is < Ns; is++) {
		for (js = 0; js < Ns; js++) {
			if (usini[is][js] > usimax)
				usimax = usini[is][js];
		}
	}
	usicut = usimax * thrini;
	for (is = 0; is < Ns; is++) {
		for (js = 0; js < Ns; js++) {
			usini[is][js] -= usicut;
			if (usini[is][js] < 0.0)
				usini[is][js] = 0.0;
		}
	}
}

void seedini(			// select seed cells
		int32_t *iseed0,// (out) x-coordinate of the k-th cell-plane: iseed0[1]
		int32_t *jseed0,// (out) y-coordinate of the k-th cell-plane: jseed0[1]
		double **usini,	// (in) response of S-cells: usini[Ns][Ns]
		int32_t Ns			// size of the S-layer
		) {
	int32_t i, j;
	double umax;

	umax = 0.0;
	iseed0[0] = -1;
	jseed0[0] = -1;
	for (i = 0; i < Ns; i++) {		// find maximum output
		for (j = 0; j < Ns; j++) {
			if (usini[i][j] > umax) {
				umax = usini[i][j];
				iseed0[0] = i;
				jseed0[0] = j;
			}
		}
	}
}

void lat(				// lateral inhibition from uin to uout
		double **ut,		// (i/o) cell-plane to be inhibited: ut[N][N]
		double **uf,		// (in) inhibiting cell-plane: uf[N][N]
		double **cl,	// (in) connections for lateral inhibition: cl[Nl][Nl]
		int32_t N,			// size of the layer
		int32_t Nl			// size of connection cl:  Nl=nl*2+1
		)
		//  Nl=nl*2+1,  nl=Al
		{
	int32_t i, j, ii, jj, iii, jjj;
	double uu;
	int32_t nl = Nl / 2;		// Nl=nl*2+1

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			if ((uu = ut[i][j]) > 0.0) {
				for (ii = -nl; ii <= nl; ii++) {
					iii = i + ii;
					if (iii >= 0 && iii < N) {
						for (jj = -nl; jj <= nl; jj++) {
							jjj = j + jj;
							if (jjj >= 0 && jjj < N) {
								uu -= cl[ii + nl][jj + nl] * uf[iii][jjj];
							}
						}
					}
				}
				if (uu < 0.0)
					uu = 0.0;
				ut[i][j] = uu;
			}
		}
	}
}

int32_t SorgL(// self-organization of S-layer (new cell-plane if winner is not from correct category)
		double ****as,	// (out) exc. connections to S-cells: as[Ks][Kc][Na][Na]
		double *bs,		// (out) inh. connections to S-cells: bs[Ks], new Ks
		int32_t *cat,		// (out) category of the S-plane: cat[Ks]
		int32_t Cat,		// (in) category of the training pattern
		double ***uc,		// (in) response of C-cells: uc[Kc][Nc][Nc]
		double **cs,		// (in) spatial weight: cs[Na][Na]
		double **asini,	// (in) exc. initial connections to S-cells: as[Na][Na]
		double ***us,	// (work) response of S-cells: us[Ks][Ns][Ns], old Ks
		int32_t Nc,			// size of the C-layer:  (nc*2) or (nc*2+1)
		int32_t Kc,			// number of cell-planes in C-layer
		int32_t Ns,			// size of the S-layer:  (ns*2) or (ns*2+1)
		int32_t Ks,			// number of cell-planes in S-layer, old value
		int32_t Na,		// size of the connections (as & cs): (na*2) or (na*2+1)
		int32_t Nasini,	// size of the connect. asini:  (nasini*2) or (nasini*2+1)
		double qs,			// parameter determining the speed of learning
		double theta,		// threshold of an S-cell
		int32_t Ksmax,		// maximum possible Ks
		int32_t KsMAX,	// max of KsMAX among all stages, because KcMAX <= KsMAX
		int32_t NcMAX,		// max of Nc (and Ng) among all stages
		int32_t NsMAX,		// max of Ns among all stages
		FILE *fpT	// (in) file pointer for debugging.  If ==NULL, no debugging
		) {
	static int32_t init = 1;	// flag indicationg initial entry
	static double **usini;// (work) response of seed-selecting plane: usini[Ns][Ns]
	int32_t ks, iseed0, jseed0, kseed0, k, i, j;
	double usMax;		// max value of us[k][i][j]
	int32_t kwin;		// k that gives usMax

	if (init != 0) {
		usini = get2D(Ns, Ns);
		init = 0;
	}
	kseed0 = -1;
	if (Ks > 0) {
		ks = Ks;
		Sres(us, uc, as, cs, bs, Nc, Kc, Ns, Ks, Na, NcMAX, NsMAX, theta);
		// Sres, using old connections
		usMax = 0.0;
		kwin = -1;
		iseed0 = -1;
		jseed0 = -1;
		for (k = 0; k < Ks; k++) {
			for (i = 0; i < Ns; i++) {
				for (j = 0; j < Ns; j++) {
					if (us[k][i][j] > usMax) {
						usMax = us[k][i][j];
						kwin = k;
						iseed0 = i;
						jseed0 = j;
					}
				}
			}
		}
		if ((kwin >= 0) && (cat[kwin] == Cat)) {
			kseed0 = kwin;						// else kseed0 = -1 (unchanged)
		}

//TEST
		if (fpT != NULL) {
			if ((kwin >= 0) && (cat[kwin] != Cat)) {
			LOGDN(
						"        Erroneous response:   recCat=%d   (Input Cat=%d)\n",
						cat[kwin], Cat);
				fprintf(fpT,
						"        Erroneous response:   recCat=%d   (Input Cat=%d)\n",
						cat[kwin], Cat);
			}
		}
//TEST

	}

	if (kseed0 < 0) {
		ks = Ks + 1;
		Sresini(usini, uc, asini, Nc, Kc, Ns, Nasini, NcMAX, 0.0);
		seedini(&iseed0, &jseed0, usini, Ns);// location of seed = max.-output cell
		if (iseed0 == -1) {
			return Ks;
		}

		cat[Ks] = Cat;
		add4D2(as, Ks + 1, Kc, Ksmax);	// add Kc elements to pointer array **as
		kseed0 = Ks;						// cell-plane to be reinforced

//TEST
		if (fpT != NULL) {
		LOGDN("        New cell-plane created (next)\n");
			fprintf(fpT, "        New cell-plane created (next)\n");
		}
//TEST

	}

	delta(as, bs, uc, cs, Na, Nc, Kc, Ns, kseed0, iseed0, jseed0, KsMAX, qs);
	// if (0 <= kseed0 < Ks): reinforce connect. for old cell-planes
	// if (kseed0 = Ks): reinforce connect. for new cell-planes
	//                   memory for bs[] must be prepared beforehand
	// memory for as[] may be increased by delta()

	return ks;
}

