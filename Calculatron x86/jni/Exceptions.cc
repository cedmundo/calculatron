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
 * File: Exceptions.cc
 * Creation: 24-08-13 23:30.
 * Description:
 *	Implementation of exceptions.h: Library can throw runtime exceptions
 *	during extraction.
 *
 * Author:
 *	Carlos Edmundo Martínez MEndoza.
 */
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include "Exceptions.h"

jint throwNoClassDefError(JNIEnv *env, const char *message) {
	jclass exClass;
	const char *className = "java/lang/NoClassDefFoundError";

	exClass = env->FindClass(className);

	if (exClass == NULL) {
		return throwNoClassDefError(env, className);
	}

	return env->ThrowNew(exClass, message);
}

jint throwNoSuchMethodError(JNIEnv *env, const char *className,
		const char *methodName, const char *signature) {
	jclass exClass;
	const char *exClassName = "java/lang/NoSuchMethodError";
	char *msgBuf;
	jint retCode;
	size_t nMallocSize;

	exClass = env->FindClass(exClassName);
	if (exClass == NULL) {
		return throwNoClassDefError(env, exClassName);
	}

	nMallocSize = strlen(className) + strlen(methodName) + strlen(signature)
			+ 8;

	msgBuf = (char *) malloc(nMallocSize);

	if (msgBuf == NULL) {
		return throwOutOfMemoryError(env,
				"throwNoSuchMethodError: allocating msgBuf");
	}

	memset(msgBuf, 0, nMallocSize);

	strcpy(msgBuf, className);
	strcat(msgBuf, ".");
	strcat(msgBuf, methodName);
	strcat(msgBuf, ".");
	strcat(msgBuf, signature);

	retCode = env->ThrowNew(exClass, msgBuf);
	free(msgBuf);
	return retCode;
}

jint throwNullPointerException(JNIEnv *env, const char *message) {
	jclass exClass;
	const char *className = "java/lang/NullPointerException";

	exClass = env->FindClass(className);
	if (exClass == NULL) {
		return throwNoClassDefError(env, className);
	}

	return env->ThrowNew(exClass, message);
}

jint throwNoSuchFieldError(JNIEnv *env, const char *message) {
	jclass exClass;
	const char *className = "java/lang/NoSuchFieldError";

	exClass = env->FindClass(className);
	if (exClass == NULL) {
		return throwNoClassDefError(env, className);
	}

	return env->ThrowNew(exClass, message);
}

jint throwOutOfMemoryError(JNIEnv *env, const char *message) {
	jclass exClass;
	const char *className = "java/lang/OutOfMemoryError";

	exClass = env->FindClass(className);
	if (exClass == NULL) {
		return throwNoClassDefError(env, className);
	}

	return env->ThrowNew(exClass, message);
}

jint throwIOException(JNIEnv *env, const char *message) {
	jclass exClass;
	const char *className = "java/io/IOException";

	exClass = env->FindClass(className);
	if (exClass == NULL) {
		return throwNoClassDefError(env, className);
	}

	return env->ThrowNew(exClass, message);
}

jint throwExtractionException(JNIEnv *env, int code) {
	jclass exClass;
	jint jcode = (jint) code;
	jobject exObject;
	jmethodID exConstructor;

	const char *className = "com/calculatron/extraction/DigitExtractionException";

	exClass = env->FindClass(className);
	if (exClass == NULL || env->ExceptionCheck() == JNI_TRUE) {
		env->ExceptionClear();

		return throwNoClassDefError(env, className);
	}

	exConstructor = env->GetMethodID(exClass, "<init>", "(I)V");
	if (exConstructor == NULL || env->ExceptionCheck() == JNI_TRUE) {
		env->ExceptionClear();

		return throwNoSuchMethodError(env,
				"com/calculatron/extraction/DigitExtractionException", "<init>",
				"(I)V");
	}

	exObject = env->NewObject(exClass, exConstructor, jcode);

	return env->Throw((jthrowable) exObject);
}
