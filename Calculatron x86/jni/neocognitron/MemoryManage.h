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
 *	File: MemoryManage.h
 *  Created on: 30/09/2013
 *     Authors: K. Fukushima, Carlos Edmundo Martínez Mendoza
 */

#ifndef MEMORYMANAGE_H_
#define MEMORYMANAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef void * std_pointer;
typedef struct _memory_register {
	std_pointer *allocations;

	uint64_t used;
	uint64_t request_block;
	uint64_t capacity;
} MemoryRegister;

MemoryRegister *generate_memory_register(int quantity);
MemoryRegister *global_register();
MemoryRegister *static_register();

void free_register();
void free_static();

std_pointer reg_static(std_pointer s);
std_pointer reg_global(std_pointer a);

double ***getp(			// memory for 1-D pointer array **u[L]
		int32_t L);
double *get1D(			// get memory for 1-D array m[N]
		int32_t N);
int32_t *get1Di(			// get memory for 1-D int32_t array m[N]
		int32_t N);
int32_t *get1Di_static(			// get memory for 1-D int32_t array m[N]
		int32_t N);

double **get2D(			// get memory for 2-D array u[N1][N2]
		int32_t N1, int32_t N2);

double **get2D_static(			// get memory for 2-D array u[N1][N2]
		int32_t N1, int32_t N2);

int32_t **get2Di(			// get memory for 2-D int32_t array m[N1][N2]
		int32_t N1, int32_t N2);

unsigned char **get2Dc(	// get memory for 2-D unsigned char array ch[N1][N2]
		int32_t N1, int32_t N2);
double ***geta3D(		// get addable memory for 3-D array u[K][N1][N2]
		int32_t K,			// initial array size
		int32_t N1, int32_t N2, int32_t Kmax// maximum possible array size: u[Kmax][N1][N2]
		);
void add3D(				// add memory to 3-D array u[K][N1][N2]
		double ***u,	// (in) u = (double ***)malloc(sizeof(double **) * Kmax)
		int32_t Knew,		// new array size
		int32_t N1, int32_t N2, int32_t Kmax	// maximum possiboe array size
		);
double ****geta4D1(		// get addable memory for 1-D pointer array ***a[]
		int32_t Kmax		// maximum possible pointer array size: ***a[Kmax]
		);
void add4D2(			// add memory to 2-D pointer array **a[K][Kc]
		double ****a,	// (in) a = (double ****)malloc(sizeof(double *) * Kmax)
		int32_t Knew,		// new pointer array size: **a[Knew][Kc]
		int32_t Kc, int32_t Kmax// maximum possible array size: **a[Kmax][Kc]
		);

#ifdef __cplusplus
}
#endif

#endif /* MEMORYMANAGE_H_ */
