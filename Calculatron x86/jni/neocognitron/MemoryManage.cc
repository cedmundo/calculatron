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
 *	File: MemoryManage.cc
 *  Created on: 30/09/2013
 *     Authors: K. Fukushima, Carlos Edmundo Martínez Mendoza
 */

#include "MemoryManage.h"
#include "NeoDebug.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

double ***getp(			// memory for 1-D pointer array **u[L]
		int32_t L) {
	double ***u;

	u = (double ***) reg_global(malloc(sizeof(double **) * L));
	if (u == NULL) {
		LOGDN("Memory allocation error!\n");
		exit(1);
	}
	return u;
}

double *get1D(			// get memory for 1-D array m[N]
		int32_t N) {
	double *u;

	u = (double *) reg_global(malloc(sizeof(double) * N));
	if (u == NULL)
		LOGDN("Memory allocation error!\n");
	return u;
}

int32_t *get1Di(			// get memory for 1-D int32_t array m[N]
		int32_t N) {
	int32_t *m;

	m = (int32_t *) reg_global(malloc(sizeof(int32_t) * N));
	if (m == NULL)
		LOGDN("Memory allocation error!\n");
	return m;
}

int32_t *get1Di_static(			// get memory for 1-D int32_t array m[N]
		int32_t N) {
	int32_t *m;

	m = (int32_t *) malloc(sizeof(int32_t) * N);
	if (m == NULL)
		LOGDN("Memory allocation error!\n");
	return m;
}

double **get2D(			// get memory for 2-D array u[N1][N2]
		int32_t N1, int32_t N2) {
	int32_t i;
	double **u;

	u = (double **) reg_global(malloc(sizeof(double *) * N1));
	if (u == NULL)
		LOGDN("Memory allocation error!\n");

	u[0] = (double *) reg_global(malloc(sizeof(double) * N1 * N2));
	if (u[0] == NULL)
		LOGDN("Memory allocation error!\n");

	for (i = 1; i < N1; i++) {
		u[i] = u[i - 1] + N2;
	}

	return u;
}

double **get2D_static(			// get memory for 2-D array u[N1][N2]
		int32_t N1, int32_t N2) {
	int32_t i;
	double **u;

	u = (double **) malloc(sizeof(double *) * N1);
	if (u == NULL)
		LOGDN("Memory allocation error!\n");

	u[0] = (double *) malloc(sizeof(double) * N1 * N2);
	if (u[0] == NULL)
		LOGDN("Memory allocation error!\n");

	for (i = 1; i < N1; i++) {
		u[i] = u[i - 1] + N2;
	}

	return u;
}

int32_t **get2Di(			// get memory for 2-D int32_t array m[N1][N2]
		int32_t N1, int32_t N2) {
	int32_t i;
	int32_t **m;

	m = (int32_t **) reg_global(malloc(sizeof(int32_t *) * N1));
	if (m == NULL)
		LOGDN("Memory allocation error!\n");

	m[0] = (int32_t *) reg_global(malloc(sizeof(int32_t) * N1 * N2));
	if (m[0] == NULL)
		LOGDN("Memory allocation error!\n");

	for (i = 1; i < N1; i++) {
		m[i] = m[i - 1] + N2;
	}

	return m;
}

unsigned char **get2Dc(	// get memory for 2-D unsigned char array ch[N1][N2]
		int32_t N1, int32_t N2) {
	int32_t i;
	unsigned char **ch;

	ch = (unsigned char **) reg_global(malloc(sizeof(unsigned char *) * N1));
	if (ch == NULL)
		LOGDN("Memory allocation error!\n");

	ch[0] = (unsigned char *) reg_global(
			malloc(sizeof(unsigned char) * N1 * N2));
	if (ch[0] == NULL)
		LOGDN("Memory allocation error!\n");

	for (i = 1; i < N1; i++) {
		ch[i] = ch[i - 1] + N2;
	}

	return ch;
}

double ***geta3D(		// get addable memory for 3-D array u[K][N1][N2]
		int32_t K,			// initial array size
		int32_t N1, int32_t N2, int32_t Kmax// maximum possible array size: u[Kmax][N1][N2]
		) {
	int32_t i, j;
	double ***u;

	if (K > Kmax) {
		LOGDN("Cannot get array larger than Kmax:  K = %d", K);
		exit(1);
	}

	u = (double ***) reg_global(malloc(sizeof(double **) * Kmax));
	if (u == NULL) {
		LOGDN("Memory allocation error!\n");
		exit(1);
	}

	u[0] = (double **) reg_global(malloc(sizeof(double *) * K * N1));
	if (u[0] == NULL) {
		LOGDN("Memory allocation error!\n");
		exit(1);
	}
	for (i = 1; i < K; i++) {
		u[i] = u[i - 1] + N1;
	}
	for (i = K; i < Kmax; i++) {
		u[i] = NULL;
	}

	u[0][0] = (double *) reg_global(malloc(sizeof(double) * K * N1 * N2));
	if (u[0][0] == NULL) {
		LOGDN("Memory allocation error!\n");
		exit(1);
	}
	for (i = 1; i < K; i++) {
		u[i][0] = u[i - 1][0] + N1 * N2;
	}
	for (i = 0; i < K; i++) {
		for (j = 1; j < N1; j++) {
			u[i][j] = u[i][j - 1] + N2;
		}
	}

	return u;
}

void add3D(				// add memory to 3-D array u[K][N1][N2]
		double ***u,	// (in) u = (double ***)malloc(sizeof(double **) * Kmax)
		int32_t Knew,		// new array size
		int32_t N1, int32_t N2, int32_t Kmax	// maximum possiboe array size
		) {
	int32_t i, j;
	int32_t Kold;		// previous array size
	int32_t Kadd;		// size of the added memory

	for (Kold = 0; Kold < Kmax; Kold++) {
		if (u[Kold] == NULL)
			break;
	}
	Kadd = Knew - Kold;
	if (Kadd <= 0)
		return;

	if (Knew > Kmax) {
		LOGDN("Cannot get array larger than Kmax:  Knew = %d", Knew);
		exit(1);
	}

	u[Kold] = (double **) reg_global(malloc(sizeof(double *) * Kadd * N1));
	if (u[Kold] == NULL) {
		LOGDN("Memory allocation error!\n");
		exit(1);
	}
	for (i = Kold + 1; i < Knew; i++) {
		u[i] = u[i - 1] + N1;
	}

	u[Kold][0] = (double *) reg_global(
			malloc(sizeof(double) * Kadd * N1 * N2));
	if (u[Kold][0] == NULL) {
		LOGDN("Memory allocation error!\n");
		exit(1);
	}
	for (i = Kold + 1; i < Knew; i++) {
		u[i][0] = u[i - 1][0] + N1 * N2;
	}
	for (i = Kold; i < Knew; i++) {
		for (j = 1; j < N1; j++) {
			u[i][j] = u[i][j - 1] + N2;
		}
	}
}

double ****geta4D1(		// get addable memory for 1-D pointer array ***a[]
		int32_t Kmax		// maximum possible pointer array size: ***a[Kmax]
		) {
	int32_t i;
	double ****a;

	a = (double ****) reg_global(malloc(sizeof(double *) * Kmax));
	if (a == NULL) {
		LOGDN("Memory allocation error!\n");
		exit(1);
	}
	for (i = 0; i < Kmax; i++) {
		a[i] = NULL;
	}
	return a;
}

void add4D2(			// add memory to 2-D pointer array **a[K][Kc]
		double ****a,	// (in) a = (double ****)malloc(sizeof(double *) * Kmax)
		int32_t Knew,		// new pointer array size: **a[Knew][Kc]
		int32_t Kc, int32_t Kmax// maximum possible array size: **a[Kmax][Kc]
		) {
	int32_t i, j;
	int32_t Kold;			// previous array size
	int32_t Kadd;			// added memory size

	for (Kold = 0; Kold < Kmax; Kold++) {
		if (a[Kold] == NULL)
			break;
	}
	Kadd = Knew - Kold;
	if (Kadd <= 0)
		return;

	if (Knew > Kmax) {
		LOGDN("Cannot get array larger than Kmax:  Knew = %d", Knew);
		exit(1);
	}

	a[Kold] = (double ***) reg_global(malloc(sizeof(double *) * Kc * Kadd));
	if (a[Kold] == NULL) {
		LOGDN("Memory allocation error!\n");
		exit(1);
	}
	for (i = Kold + 1; i < Knew; i++) {
		a[i] = a[i - 1] + Kc;
	}

	for (i = Kold; i < Knew; i++) {
		for (j = 0; j < Kc; j++) {
			a[i][j] = NULL;
		}
	}
}

MemoryRegister *generate_memory_register(int quantity) {
	MemoryRegister *mem = (MemoryRegister *) malloc(sizeof(MemoryRegister *));
	mem->used = 0;
	mem->capacity = quantity;

	mem->allocations = (void **) malloc(sizeof(void *) * quantity);
	return mem;
}

MemoryRegister *global_register() {
	static MemoryRegister *_dynamic = generate_memory_register(100); // Generate a block with 150 Registers.
	return _dynamic;
}

MemoryRegister *static_register() {
	static MemoryRegister *_static = generate_memory_register(50); // Generate a block with 150 Registers.
	return _static;
}

std_pointer reg_global(std_pointer a) {
	global_register()->allocations[global_register()->used++] = a;
	return a;
}

std_pointer reg_static(std_pointer a) {
	static_register()->allocations[static_register()->used++] = a;
	return a;
}

void free_register() {
	MemoryRegister *g = global_register();
	uint64_t i;

	for (i = 0; i < g->used; i++) {
		free(g->allocations[i]);
	}

	g->used = 0;
}

void free_static() {
	MemoryRegister *g = static_register();
	uint64_t i;

	for (i = 0; i < g->used; i++) {
		free(g->allocations[i]);
	}

	g->used = 0;
}


