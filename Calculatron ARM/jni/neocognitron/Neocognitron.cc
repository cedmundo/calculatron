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

void nc_init(Neocognitron *neoc, const char *fnameC) {
	int32_t l;

	MemoryRegister *reg = global_register();
	LOGDN("Initalizing neocognitron...");

	neoc->Ng = ((int32_t) neoc->Ag) * 2 + 1;

	if (((neoc->N0 + neoc->Nc[0]) % 2) == 0) {
		neoc->Ng = ((int32_t) neoc->Ag) * 2 + 1;
	} else {
		neoc->Ng = ((int32_t) (neoc->Ag + 0.5)) * 2;
	}

	neoc->Na = get1Di(neoc->L);
	neoc->Nasini = get1Di(neoc->L);

	for (l = 0; l < neoc->L; l++) {
		if ((neoc->Nc[l] + neoc->Ns[l]) % 2 == 0) {
			neoc->Na[l] = ((int32_t) neoc->As[l]) * 2 + 1;
			neoc->Nasini[l] = ((int32_t) neoc->Asini[l]) * 2 + 1;
		} else {
			neoc->Na[l] = ((int32_t) (neoc->As[l] + 0.5)) * 2;
			neoc->Nasini[l] = ((int32_t) (neoc->Asini[l] + 0.5)) * 2;
		}
	}

	neoc->Nd = get1Di(neoc->L);
	for (l = 0; l < neoc->L; l++) {
		if (((neoc->mc[l] == 2) && (neoc->Ns[l] % 2 == 1))
				|| ((neoc->mc[l] == 1)
						&& ((neoc->Ns[l] + neoc->Nc[l + 1]) % 2 == 0))) {
			neoc->Nd[l] = ((int32_t) neoc->Ac[l]) * 2 + 1;
		} else {
			neoc->Nd[l] = ((int32_t) (neoc->Ac[l] + 0.5)) * 2;
		}
	}

	neoc->Nl = get1Di(neoc->L);
	for (l = 0; l < neoc->L; l++)
		neoc->Nl[l] = ((int32_t) neoc->Al[l]) * 2 + 1;

	neoc->Mas = get1Di(neoc->L);
	neoc->cg = get2D(neoc->Ng, neoc->Ng);
	make_cg(neoc->cg, neoc->Ng, neoc->Ag, neoc->Ago, neoc->gamcg);

	neoc->cs = getp(neoc->L);
	for (l = 0; l < neoc->L; l++) {
		neoc->cs[l] = get2D(neoc->Na[l], neoc->Na[l]);
		make_c(neoc->cs[l], neoc->Na[l], neoc->As[l], neoc->gamS[l],
				neoc->coS[l]);
	}

	neoc->ac = getp(neoc->L);
	for (l = 0; l < neoc->L - 1; l++) {
		neoc->ac[l] = get2D(neoc->Nd[l], neoc->Nd[l]);
		make_ac(neoc->ac[l], neoc->Nd[l], neoc->Ac[l], neoc->Aco[l],
				neoc->gamC[l], neoc->gamCo[l], neoc->gamCp[l], neoc->coC[l],
				neoc->pnC[l]);
	}

	neoc->as = (double *****) getp(neoc->L);
	neoc->bs = (double **) getp(neoc->L);
	neoc->u0 = geta3D(1, neoc->N0, neoc->N0, 1);
	neoc->us = (double ****) getp(neoc->L);
	neoc->uc = (double ****) getp(neoc->L + 1);
	neoc->uc[0] = geta3D(neoc->Kc[0], neoc->Nc[0], neoc->Nc[0], neoc->Kc[0]);
	neoc->NsMAX = 0;
	neoc->NcMAX = neoc->Nc[neoc->L];
	for (l = 0; l < neoc->L; l++) {
		if (neoc->Ns[l] > neoc->NsMAX)
			neoc->NsMAX = neoc->Ns[l];
		if (neoc->Nc[l] > neoc->NcMAX)
			neoc->NcMAX = neoc->Nc[l];
	}

	neoc->TTmax = neoc->TL;
	if (neoc->TT > neoc->TTmax)
		neoc->TTmax = neoc->TT;

	// Read connections.
	rwcon(NULL, fnameC, neoc->as, neoc->bs, &neoc->cat, neoc->Na, neoc->Kc,
			neoc->Ks, neoc->Mas, neoc->L, 0);

	neoc->KsMAX = 0;
	for (l = 0; l < neoc->L; l++) {
		if (neoc->Ks[l] > neoc->KsMAX)
			neoc->KsMAX = neoc->Ks[l];
	}

	for (l = 0; l < neoc->L; l++) {
		neoc->us[l] = geta3D(neoc->Ks[l], neoc->Ns[l], neoc->Ns[l],
				neoc->Ks[l]);
		neoc->uc[l + 1] = geta3D(neoc->Kc[l + 1], neoc->Nc[l + 1],
				neoc->Nc[l + 1], neoc->Kc[l + 1]);
	}

	LOGDN("Neocognitron loaded, MemoryRegister is in [%p]. Reference capacity: %d, used: %d",
			reg, reg->capacity, reg->used);
}

Neocognitron *get_nc_instance() {
	static Neocognitron *nc_static = new Neocognitron();
	return nc_static;
}

void nc_destroy_instance() {
	free_register();
	free(global_register());
}

int32_t nc_categorize(Neocognitron *neoc, const IplImage *segment) {
	int32_t recCat, l;
	recCat = 0;

	// Convert segment IplImage to U0 Input layer.
	pat(neoc->u0[0], 65, segment);
	onoff(neoc->uc[0], neoc->u0[0], neoc->cg, neoc->N0, neoc->Nc[0], neoc->Ng);

	for (l = 0; l < neoc->L; l++) {
		Sres(neoc->us[l], neoc->uc[l], neoc->as[l], neoc->cs[l], neoc->bs[l],
				neoc->Nc[l], neoc->Kc[l], neoc->Ns[l], neoc->Ks[l], neoc->Na[l],
				neoc->NcMAX, neoc->NsMAX, neoc->thetar[l]);
		if (l != neoc->L - 1) {
			Cres(neoc->uc[l + 1], neoc->us[l], neoc->ac[l], neoc->Ns[l],
					neoc->Nc[l + 1], neoc->mc[l], neoc->Kc[l + 1], neoc->Nd[l],
					neoc->NsMAX);
		} else {
			recCat = CresL(neoc->uc[l + 1], neoc->us[l], neoc->cat, neoc->Ns[l],
					neoc->Ks[l], neoc->Kc[l + 1]);
		}
	}

	return recCat;
}

