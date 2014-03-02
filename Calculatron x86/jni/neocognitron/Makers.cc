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
 *	File: Makers.cc
 *  Created on: 30/09/2013
 *     Authors: K. Fukushima, Carlos Edmundo Martínez Mendoza
 */

#include <math.h>
#include <stdint.h>

#include "Makers.h"

void make_cg(			// make connections for on-center
		double **cg,		// (out) Connections: cg[Ng][Ng]
		int32_t Ng,			// size of connection cg: (ng*2) or (ng*2+1)
		double Ag,			// radius of the off-periphery (connect. area)
		double Ago,		// radius of the on-center component
		double gamcg		// spatial decay at x=Ag
		) {
	int32_t i, j;
	double x, y, y2, r2, Spos, Sneg, SSpos, SSneg, cc;
	double gn = (Ng - 1) * 0.5;
	double Ag2 = Ag * Ag;
	double Ago2 = Ago * Ago;
	double C = 1 / Ag2;
	double Co = 1 / Ago2;

	Spos = 0.0;
	Sneg = 0.0;
	for (i = 0; i < Ng; i++) {
		y = i - gn;
		y2 = y * y;
		for (j = 0; j < Ng; j++) {
			x = j - gn;
			r2 = y2 + x * x;
			if (r2 <= Ag2) {
				cc = Co * pow(gamcg, r2 / Ago2) - C * pow(gamcg, r2 / Ag2);
				cg[i][j] = cc;
				if (cc > 0.0)
					Spos += cc;
				else
					Sneg -= cc;
			} else {
				cg[i][j] = 0.0;
			}
		}
	}
	SSpos = 1.0 / Spos;
	SSneg = 1.0 / Sneg;
	for (i = 0; i < Ng; i++) {
		for (j = 0; j < Ng; j++) {
			if (cg[i][j] > 0.0) {
				cg[i][j] *= SSpos;
			} else {
				cg[i][j] *= SSneg;
			}
		}
	}
}

void make_ac(			// make center surround connections ac
		double **ac,		// (out) Connections: ac[Na][Na]
		int32_t Na,			// size of the connection ac: (na*2) or (na*2+1)
		double A,			// radius of the off-periphery (connect. area)
		double Ao,			// radius of the on-center component
		double gamma,		// spatial decay of exc. connect. at x=Ao
		double gammao,		// spatial decay of inh. connect. at x=Ao
		double gammap,		// spatial decay of inh. connect. at x=A
		double co,			// value of ac[][] at the center
		double pn			// ratio of negative to positibe components
		) {
	int32_t i, j;
	double x, y, y2, r2, Spos, Sneg, SSneg, cc, C, Co, CCo = 0, CC = 0;
	double an = (Na - 1) * 0.5;
	double A2 = A * A;
	double Ao2 = Ao * Ao;

	if (Ao2 < A2) {
		C = 1 / A2;
		Co = 1 / Ao2;
		CCo = co * Co / (Co - C);
		CC = co * C / (Co - C);
	}

	Spos = 0.0;
	Sneg = 0.0;
	for (i = 0; i < Na; i++) {
		y = i - an;
		y2 = y * y;
		for (j = 0; j < Na; j++) {
			x = j - an;
			r2 = y2 + x * x;
			if (r2 <= Ao2) {
				cc = co * pow(gamma, r2 / Ao2);
				ac[i][j] = cc;
				Spos += cc;
			} else if (r2 <= A2) {
				cc = CCo * pow(gammao, r2 / Ao2) - CC * pow(gammap, r2 / A2);
				ac[i][j] = cc;
				if (cc > 0.0)
					Spos += cc;
				else
					Sneg -= cc;
			} else {
				ac[i][j] = 0.0;
			}
		}
	}
	if (Ao2 < A2) {
		SSneg = pn * Spos / Sneg;
		for (i = 0; i < Na; i++) {
			for (j = 0; j < Na; j++) {
				if (ac[i][j] < 0.0) {
					ac[i][j] *= SSneg;
				}
			}
		}
	}
}

void make_c(			// make connections (spatial weight)
		double **c,		// (out) Connections: c[Na][Na]
		int32_t Na,			// size of the connection c: (na*2) or (na*2+1)
		double A,			// radius of the connectable area
		double gamma,		// spatial decay at x=A
		double co			// value of c[][] at the center
		) {
	int32_t i, j;
	double x, y, y2, r2;
	double an = (Na - 1) * 0.5;
	double A2 = A * A;

	for (i = 0; i < Na; i++) {
		y = i - an;
		y2 = y * y;
		for (j = 0; j < Na; j++) {
			x = j - an;
			r2 = y2 + x * x;
			if (r2 <= A2) {
				c[i][j] = co * pow(gamma, r2 / A2);
			} else {
				c[i][j] = 0.0;
			}
		}
	}
}
