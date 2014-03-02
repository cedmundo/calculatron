/*
 *  * License:
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
 * File: exceptions.c
 * Creation: 24-08-13 23:30.
 * Description:
 *	Header file for exceptions: Library can throw runtime exceptions
 *	during extraction.
 *
 * Author:
 *	Carlos Edmundo Martínez Mendoza.
 */

/*
 * Throws a Java NoClassDefError in JVM.
 *	If the request class does not exists, this error is thrown.
 *
 * Paramteres: message - Message to display.
 * Returns: the same as (* env)->throwNew(...);
 */
jint throwNoClassDefError(JNIEnv *env, const char *message);

/*
 * Throws a Java NoSuchMethodError,
 *	if a requested method or constructor does not exists, this error is thrown.
 *
 * Parameters: className - Name of the class wherever the method does not exists.
 *			  methodName - Name of the inexistent method.
 *			  signature - Signature of inexistent method.
 *
 * Returns: the same as (* env)->throwNew(...);
 */
jint throwNoSuchMethodError(JNIEnv *env, const char *className, const char *methodName,
		const char *signature);

/*
 * Throws a Java NoSuchFieldError in JVM.
 *	If the request class does not exists, this error is thrown.
 *
 * Paramteres: message - Message to display.
 * Returns: the same as (* env)->throwNew(...);
 */
jint throwNoSuchFieldError(JNIEnv *env, const char *message);

/*
 * Throws a JavaOutOfMemoryError in JVM.
 *	If the request class does not exists, this error is thrown.
 *
 * Paramteres: message - Message to display.
 * Returns: the same as (* env)->throwNew(...);
 */jint throwOutOfMemoryError(JNIEnv *env, const char *message);

/*
 * Throws a Java IOException in JVM.
 *	If the request class does not exists, this error is thrown.
 *
 * Paramteres: message - Message to display.
 * Returns: the same as (* env)->throwNew(...);
 */
jint throwIOException(JNIEnv *env, const char *message);

/*
 * Throws a Java ExtractionException in JVM.
 *	If the request class does not exists, this error is thrown.
 *
 * Paramteres: code - Internal code of exception.
 * Returns: the same as (* env)->Throw(...);
 */
jint throwExtractionException(JNIEnv *env, int code);

/*
 * Throws a Java NullPointerException in JVM.
 *	If the request class does not exists, this error is thrown.
 *
 * Paramteres: message - Message to display.
 * Returns: the same as (* env)->throwNew(...);
 */
jint throwNullPointerException(JNIEnv *env, const char *message);

/*
 * Register of exceptions:
 */
#define EXCODE_NOT_SUPPORTED_YET 0x001
#define EXCODE_NOT_BITMAP_INFO 0x101
#define EXCODE_BITMAP_FORMAT 0x102
#define EXCODE_CANNOT_LOCK_PIXELS 0x103

