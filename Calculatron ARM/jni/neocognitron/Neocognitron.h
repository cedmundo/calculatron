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
 *	File: Neocognitron.h
 *  Created on: 30/09/2013
 *     Authors: K. Fukushima, Carlos Edmundo Martínez Mendoza
 */

#ifndef NEOCOGNITRON_H_
#define NEOCOGNITRON_H_

#include "opencv/cv.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Neocognitron data structure.
 * Contains every variable that NC needs to work.
 *
 * This is stracted from main file in K. Fukushima's project.
 * When initialized, it will contains default data extracted from that project too.
 */
typedef struct _st_neocognitron {
	int32_t KsMAX;
	int32_t NsMAX;
	int32_t NcMAX;
	int32_t TTmax;

	int32_t L;
	int32_t N0;
	int32_t Nc[5];
	int32_t Ns[4];
	int32_t mc[4];

	double Ag;
	double Ago;
	double As[4];
	double Aco[4];
	double Ac[4];
	double D[4];
	double Asini[4];
	double Al[4];

	double gamcg;
	double gamS[4];
	double coS[4];
	double gamC[4];
	double gamCo[4];
	double gamCp[4];
	double coC[4];
	double pnC[4];

	double thetar[4];

	int32_t Ks[4];
	int32_t Kc[5];

	int32_t TL;
	int32_t TT;

	int32_t Ng;
	int32_t *Na;
	int32_t *Nd;
	int32_t *Nasini;
	int32_t *Nl;
	int32_t *Mas;
	double **cg;
	double ***cs;
	double ***ac;
	double ***u0;
	int32_t **mat;
	double ****us;
	double ****uc;
	int32_t *cat;
	double *****as;
	double **bs;
	_st_neocognitron() :
			KsMAX(0), NsMAX(0), NcMAX(0), TTmax(0), L(4), N0(65), Nc( { 71, 37,
					21, 13, 1 }), Ns( { 68, 38, 22, 5 }), mc( { 2, 2, 2, 1 }), Ag(
					3.3), Ago(1.5), As( { 3.4, 3.4, 3.4, 4.9 }), Aco( { 3.4,
					3.4, 4.4, 0.0 }), Ac( { 9.4, 7.4, Aco[2], 0.0 }), D( { 0.0,
					3.1, 3.1, 0.0 }), Asini( { 0.0, As[1], As[2], As[3] }), Al(
					{ 0.0, D[1], D[2], 0.0 }), gamcg(0.05), gamS( { 0.7, 0.7,
					0.7, 0.6 }), coS( { 1.0, 1.0, 1.0, 1.0 }), gamC( { 0.7, 0.7,
					0.7, 1.0 }), gamCo( { 0.12, 0.12, 0.12, 0.0 }), gamCp( {
					0.8, 0.8, 0.8, 0.0 }), coC( { 4.0, 4.0, 4.0, 1.0 }), pnC( {
					1.0, 1.0, 1.0, 0.0 }), thetar( { 0.55, 0.51, 0.58, 0.30 }), Ks(
					{ 16, 80, 80, 30 }), Kc( { 2, 16, 0, 0, 10 }), TL(400), TT(
					10), Ng(((int32_t) Ag) * 2 + 1), Na(NULL), Nd(NULL), Nasini(
					NULL), Nl(NULL), Mas(NULL), cg(NULL), cs(NULL), ac(NULL), u0(
					NULL), mat(0), us(NULL), uc(NULL), cat(NULL), as(NULL), bs(
					NULL) {
	}
} Neocognitron;

/*
 * Initalizes a empty neocognitron instance.
 * */
void nc_init(Neocognitron *nc, const char *file);

/**
 * Obtains neocognitron instance (static for all PendingExtraction objects).
 */
Neocognitron *get_nc_instance();

/**
 * Destroys data from neocognitron.
 *
 * Note: It's important to clean up all information from Neocognitron that it's
 * about 3 or 6 Mb of information (depending on arch).
 */
void nc_destroy_instance();

/**
 * Categorizes an image.
 * Each category corresponds with its number, i.e. Cat=0 is number '0'.
 * If Neocognitron can't categorize an image, it will return -1.
 *
 */
int32_t nc_categorize(Neocognitron *nc, const IplImage *segment);

#ifdef __cplusplus
}
#endif

#endif /* NEOCOGNITRON_H_ */
