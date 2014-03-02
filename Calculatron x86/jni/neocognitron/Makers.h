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
 *	File: Makers.h
 *  Created on: 30/09/2013
 *     Authors: K. Fukushima Carlos Edmundo Martínez Mendoza
 */

#ifndef MAKERS_H_
#define MAKERS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void make_cg(			// make connections for on-center
		double **cg,		// (out) Connections: cg[Ng][Ng]
		int32_t Ng,			// size of connection cg: (ng*2) or (ng*2+1)
		double Ag,			// radius of the off-periphery (connect. area)
		double Ago,		// radius of the on-center component
		double gamcg		// spatial decay at x=Ag
		);
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
		);
void make_c(			// make connections (spatial weight)
		double **c,		// (out) Connections: c[Na][Na]
		int32_t Na,			// size of the connection A: (na*2) or (na*2+1)
		double A,			// radius of the connectable area
		double gamma,		// spatial decay at x=A
		double co			// value of c[][] at the center
		);

#ifdef __cplusplus
}
#endif

#endif /* MAKERS_H_ */
