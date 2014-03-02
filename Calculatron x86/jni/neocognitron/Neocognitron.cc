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
 *	File: Neocognitron.cc
 *  Created on: 30/09/2013
 *     Authors: K. Fukushima, Carlos Edmundo Martínez Mendoza
 */

#include "Neocognitron.h"
#include "NeoDebug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MemoryManage.h"
#include "Layers.h"
#include "Makers.h"
#include "SelfOrganitation.h"
#include "InputOutput.h"

int32_t recognize_number(const char *fnameC, const IplImage* segment) {

	////  Parameters

	int L = 4; // number of stages

	// layer:	   U0 UG S1 C1 S2 C2 S3 C3 S4 C4
	int N0 = 65; // size of input layer U0
	int Nc[] = { 71, 37, 21, 13, 1 }; // size of Uc(or U0) cell-plane, Nc[L+1]
	int Ns[] = { 68, 38, 22, 5 }; // size of Us cell-planes,  Ns[L]
	int mc[] = { 2, 2, 2, 1 }; // Cres: step of C-cells,  mc[L]

	double Ag = 3.3; // radius of connect. ag (off-surround)
	double Ago = 1.5; // radius of on-center of ag
	double As[] = { 3.4, 3.4, 3.4, 4.9 }; // radius of connect. as, As[L]
	double Aco[] = { 3.4, 3.4, 4.4, 0.0 }; // radius of on center in connect. ac, Ac[L]
	double Ac[] = { 9.4, 7.4, Aco[2], 0.0 }; // radius of connect. ac, Ac[L]
	double D[] = { 0.0, 3.1, 3.1, 0.0 }; // radius of competition area,  D[L]
	double Asini[] = { 0.0, As[1], As[2], As[3] }; // make_c(asini): radius of connect.: Asini[L]
	double Al[] = { 0.0, D[1], D[2], 0.0 }; // radius of lat.inh. cl: Nl=((int)Al)*2+1, Al[L]

	double gamcg = 0.05; // make_cg(cg): spatial decay at x=Ag
	double gamS[] = { 0.7, 0.7, 0.7, 0.6 }; // make_c(cs): spatial decay, gammaS,  gamS[L]
	double coS[] = { 1.0, 1.0, 1.0, 1.0 }; // make_c(cs): value at the center,  coS[L]
	double gamC[] = { 0.7, 0.7, 0.7, 1.0 }; // make_ac(ac): spatial decay of exc. center at x=Ao,  gamC[L]
	double gamCo[] = { 0.12, 0.12, 0.12, 0.0 }; // make_ac(ac): spatial decay of inh. connect. at x=Ao
	double gamCp[] = { 0.8, 0.8, 0.8, 0.0 }; // make_ac(ac): spatial decay of inh. connect. at x=A
	double coC[] = { 4.0, 4.0, 4.0, 1.0 }; // make_ac(ac): value at the center,  coC[L]
	double pnC[] = { 1.0, 1.0, 1.0, 0.0 }; // make_ac(ac): ratio of negative to positive components

	double thetar[] = { 0.55, 0.51, 0.58, 0.30 }; // Sres: threshold of S-cells (recognition),  theta[L]

	int Ks[] = { 16, 80, 80, 30 }; // no. of S-cell-planes, init.val., changeable,  Ks[L]
	int Kc[] = { 2, 16, 0, 0, 10 }; // no. of C-cell-planes, init.val., changeable,  Kc[L+1]

	int TL = 400; // no. of learning pat for each stage
	int TT = 10; // no. of tests for unlearned patterns

	// memories for parameters
	int Ng; // size of on-center connect. cg[Ng][Ng]
	int *Na; // size of connect. as & cs:  Na=((int)(As+0.5))*2,  Na[L]
	int *Nd; // size of connect. ac:  Nd=((int)(Ac+0.5))*2,  Nd[L]
	int *Nasini; // size of connect. asini: Nasini=((int)(Asini+0.5))*2, Nasini[L]
	int *Nl; // size of lat.inh. cl:  Nl=((int)Al)*2+1,  Nl[L]
	int *Mas; // no. of active as[L][Ks][Kc]: Mas[L]

	// memories for fixed connections
	double **cg; // connect. of on-center: cg[Ng][Ng]
	double ***cs; // exc. connect. to V-cells: cs[L][Na][Na]
	double ***ac; // exc. connect. to C-cells: ac[L][Nd][Nd]

	// memories for responses -- fixed
	double ***u0; // response of input layer: u0[N0][N0]

	// memories for responses -- expandable
	double ****us; // response of S-cells: us[L][Ks][Ns][Ns]
	double ****uc; // response of C-cells: uc[L][Kc][Nc][Nc]
	int *cat; // category of the cell-plane in UsL: cat[KsL]

	// memories for connections -- expandable
	double *****as; // exc. connect. to S-cells: as[L][Ks][Kc][Na][Na]
	double **bs; // inh. connect. to S-cells: bs[L][Ks]

	// working
	int NsMAX; // NsMAX=max(Ns[l]) for l=0,1,..,L-1
	int KsMAX; // KsMAX=max(Ksmax[l]) for l=0,1,..,L-1
	int NcMAX; // NcMAX=max(Nc[l]) for l=0,1,..,L
	FILE *fpC = NULL; // file pointer (connections)
	int l, recCat, TTmax;

	MemoryRegister *reg = global_register();
	LOGDN("Memory register [%p]: %lld, used: %lld", reg, reg->capacity, reg->used);

	recCat = 0;
	/// calculate connection sizes N depending on Ns and Nc
	// get array and set parameters

	Ng = ((int) Ag) * 2 + 1; // size of on-center connect. cg[Ng][Ng]

	if (((N0 + Nc[0]) % 2) == 0) { // alligned
		Ng = ((int) Ag) * 2 + 1; // size of on-center connect. cg[Ng][Ng]
	} else { // staggered
		Ng = ((int) (Ag + 0.5)) * 2;
	}

	Na = get1Di(L); // size of connect. as & cs: Na[L]
	Nasini = get1Di(L); // size of init. connect. asini: Nasini[L]

	for (l = 0; l < L; l++) {
		if ((Nc[l] + Ns[l]) % 2 == 0) { // alligned (Na is odd)
			Na[l] = ((int) As[l]) * 2 + 1;
			Nasini[l] = ((int) Asini[l]) * 2 + 1;
		} else { // staggered (Na is even)
			Na[l] = ((int) (As[l] + 0.5)) * 2;
			Nasini[l] = ((int) (Asini[l] + 0.5)) * 2; // approxinate
			// Exactly, Na should be: (na-0.5)^2 + 0.5^2 = A^2;  na = 0.5 + sqrt(A^2 - 0.5^2);  Na = na * 2;
		}
	}

	Nd = get1Di(L); // size of connect. ac: Nd[L]
	for (l = 0; l < L; l++) {
		if (((mc[l] == 2) && (Ns[l] % 2 == 1))
				|| ((mc[l] == 1) && ((Ns[l] + Nc[l + 1]) % 2 == 0))) { // alligned (Nd is odd)
			Nd[l] = ((int) Ac[l]) * 2 + 1;
		} else { // staggered (Nd is even)
			Nd[l] = ((int) (Ac[l] + 0.5)) * 2; // approximate
			// Exactly, Nd should be: (nd-0.5)^2 + 0.5^2 = A^2;  nd = 0.5 + sqrt(A^2 - 0.5^2);  Nd = nd * 2;
		}
	}

	Nl = get1Di(L); // size of lat.inh. cl: Nl[L]
	for (l = 0; l < L; l++)
		Nl[l] = ((int) Al[l]) * 2 + 1;

	Mas = get1Di(L); // no. of active as[L][Ks][Kc]: Mas[L]

	////  get memory for arrays

	///  fixed connections -- get memory & calculate
	cg = get2D(Ng, Ng);
	make_cg(cg, Ng, Ag, Ago, gamcg);

	cs = getp(L);
	for (l = 0; l < L; l++) {
		cs[l] = get2D(Na[l], Na[l]);
		make_c(cs[l], Na[l], As[l], gamS[l], coS[l]);
	}

	ac = getp(L);
	for (l = 0; l < L - 1; l++) {
		ac[l] = get2D(Nd[l], Nd[l]);
		make_ac(ac[l], Nd[l], Ac[l], Aco[l], gamC[l], gamCo[l], gamCp[l],
				coC[l], pnC[l]);
	}

	as = (double *****) getp(L); // actual memories will be allocated later
	bs = (double **) getp(L); // actual memories will be allocated later

	// memories for responses -- fixed
	u0 = geta3D(1, N0, N0, 1);

	// memories for responses -- memory for arrays will be allocated later
	us = (double ****) getp(L);
	uc = (double ****) getp(L + 1); // memory(l>0) will be allocated after learning of us
	uc[0] = geta3D(Kc[0], Nc[0], Nc[0], Kc[0]);

	// calculate parameters
	NsMAX = 0;
	NcMAX = Nc[L];
	for (l = 0; l < L; l++) {
		if (Ns[l] > NsMAX)
			NsMAX = Ns[l];
		if (Nc[l] > NcMAX)
			NcMAX = Nc[l];
	}

	TTmax = TL;
	if (TT > TTmax)
		TTmax = TT;

	////  read/write connections  ====>  get Mas arrays [Na][Na] when read
	// Also get memories for as, bs and cat, when read (recognition)
	rwcon(fpC, fnameC, as, bs, &cat, Na, Kc, Ks, Mas, L, 0); // if (learn>0) write, else read

	/// get memories for responses, in case of recognition without learning
	KsMAX = 0; // recalculate KsMAX
	for (l = 0; l < L; l++) {
		if (Ks[l] > KsMAX)
			KsMAX = Ks[l];
	}

	for (l = 0; l < L; l++) {
		us[l] = geta3D(Ks[l], Ns[l], Ns[l], Ks[l]);
		uc[l + 1] = geta3D(Kc[l + 1], Nc[l + 1], Nc[l + 1], Kc[l + 1]);
	}

	// Convert segment IplImage to U0 Input layer.
	pat(u0[0], 65, segment);
	onoff(uc[0], u0[0], cg, N0, Nc[0], Ng);

	for (l = 0; l < L; l++) {
		Sres(us[l], uc[l], as[l], cs[l], bs[l], Nc[l], Kc[l], Ns[l], Ks[l],
				Na[l], NcMAX, NsMAX, thetar[l]);
		if (l != L - 1) {
			Cres(uc[l + 1], us[l], ac[l], Ns[l], Nc[l + 1], mc[l], Kc[l + 1],
					Nd[l], NsMAX);
		} else {
			recCat = CresL(uc[l + 1], us[l], cat, Ns[l], Ks[l], Kc[l + 1]);
		}
	}

	LOGDN("Used references: %lld", global_register()->used);
	free_register();

	return recCat;
}

