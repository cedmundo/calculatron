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
 *	File: NeoDebug.h
 *  Created on: 03/10/2013
 *     Authors: K. Fukushima, Carlos Edmundo Martínez Mendoza
 */

#ifndef NEODEBUG_H_
#define NEODEBUG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "android/log.h"

#ifdef NDEBUG
	#define NLTAG "libCalculatron.armeabi.neocognitron"
	#define LOGDN(...)
#else
	#define NLTAG "libCalculatron.armeabi.neocognitron:"
	#define LOGDN(...) __android_log_print(ANDROID_LOG_DEBUG, NLTAG, __VA_ARGS__)
#endif



#ifdef __cplusplus
}
#endif

#endif /* NEODEBUG_H_ */
