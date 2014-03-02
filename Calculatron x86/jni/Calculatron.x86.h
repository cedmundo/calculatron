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
 *	File: Calculatron.x86.h
 *  Created on: 21/09/2013
 *      Author: Carlos Edmundo Martínez Mendoza
 */

#ifndef CALCULATRON_ARMEABI_H_
#define CALCULATRON_ARMEABI_H_

#include "android/log.h"

#ifdef NDEBUG
#define LTAG "libCalculatron.x86"
#define LOGD(...)
#else
#define LTAG "libCalculatron.x86:"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LTAG, __VA_ARGS__)
#endif

#endif /* CALCULATRON_ARMEABI_H_ */
