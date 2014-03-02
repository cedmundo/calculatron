/*
 * License:
 * This license is derived from the original wrote by K. Fukushima (See NC_README.TXT)
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
 * File: Calculatron.armeabi.cc
 * Author: Carlos Edmundo Martínez Mendoza.
 */

// Java Native Interface
#include <jni.h>

// Standar C libraries.
#include <stdio.h>
#include <stdlib.h>

// Android includes.
#include <android/bitmap.h>
#include <android/log.h>

// Impl. Header (In this case, LOGD MACRO)
#include "Calculatron.armeabi.h"
#include "Exceptions.h"

// Impl. Header.
#include "com_calculatron_extraction_PendingExtraction.h"

// OpenCV includes.
#include "opencv/cv.h"
#include "opencv/highgui.h"

// Neocognitron
#include "neocognitron/Neocognitron.h"
#include "neocognitron/MemoryManage.h"

//
//
JNIEXPORT void JNICALL Java_com_calculatron_extraction_PendingExtraction_extract(
		JNIEnv *env, jobject jo_pending_extraction) {

	jclass jc_pending_extraction; // For perform calls on PendingExtraction instance.
	jfieldID jfid_file_path;	// Field ID: filePath.

	jmethodID jmid_report_start; // MethodID: reportDigitExtraction(byte b)
	jmethodID jmid_report_end; // MethodID: reportDigitExtraction(byte b)
	jmethodID jmid_report_update; // MethodID: reportDigitExtraction(byte b)

	jstring js_file_path;		// String: Where original picture is located.

	const char* jch_file_path; // To call.

	IplImage* ia;
	IplImage* ib; // To save memory, will process only between this two images.
	IplImage* i_character_source;

	CvMemStorage* storage = cvCreateMemStorage(0);

	char *result_string; // Result string.
	int16_t current_char_count = 0; // Count of internal character.

	result_string = new char[45]; // Max 45 character per line.

	// Initializing
	jc_pending_extraction = env->GetObjectClass(jo_pending_extraction);

	jfid_file_path = env->GetFieldID(jc_pending_extraction, "filePath",
			"Ljava/lang/String;");
	if (env->ExceptionCheck() == JNI_TRUE || jfid_file_path == NULL) {
		env->ExceptionClear();

		throwNoSuchFieldError(env,
				"Field PendingExtraction#filePath does not exists");
		return;
	}

	js_file_path = (jstring) env->GetObjectField(jo_pending_extraction,
			jfid_file_path);
	if (js_file_path == NULL) {
		throwNullPointerException(env, "File path is null");
		return;
	}

	jch_file_path = env->GetStringUTFChars(js_file_path, 0);

	/*
	 * Methods to callback application.
	 * */
	jmid_report_start = env->GetMethodID(jc_pending_extraction, "reportStart",
			"()V");
	if (env->ExceptionCheck() == JNI_TRUE || jmid_report_start == NULL) {
		env->ExceptionClear();
		throwNoSuchMethodError(env, "PendingExtraction", "reportStart", "()V");
		return;
	}

	jmid_report_update = env->GetMethodID(jc_pending_extraction, "reportUpdate",
			"(B)V");
	if (env->ExceptionCheck() == JNI_TRUE || jmid_report_update == NULL) {
		env->ExceptionClear();
		throwNoSuchMethodError(env, "PendingExtraction", "reportUpdate",
				"(B)V");
		return;
	}

	jmid_report_end = env->GetMethodID(jc_pending_extraction, "reportEnd",
			"(Ljava/lang/String;)V");
	if (env->ExceptionCheck() == JNI_TRUE || jmid_report_end == NULL) {
		env->ExceptionClear();
		throwNoSuchMethodError(env, "PendingExtraction", "reportEnd",
				"(Ljava/lang/String;)V");
		return;
	}

	// =======================
	// Let's begin processing.
	// =======================
	// Load and create images.

	// Report we are going to start.
	env->CallVoidMethod(jo_pending_extraction, jmid_report_start);

	ia = cvLoadImage(jch_file_path, 0); // Cached image.

	int32_t iwidth = ia->width;
	int32_t iheight = ia->height;

	ib = cvCreateImage(cvSize(iwidth, iheight), IPL_DEPTH_8U, 1); // Where to put first data.
	i_character_source = cvCreateImage(cvSize(iwidth, iheight), IPL_DEPTH_8U,
			1); // Where to put first data.

	//threshold image

	CvScalar avg;
	CvScalar avgStd;
	cvAvgSdv(ia, &avg, &avgStd, NULL);
	LOGD("Avg: %f\nStd: %f\n", avg.val[0], avgStd.val[0]);

	/*cvThreshold(ia, ib, (int) avg.val[0] - 7 * (int) (avgStd.val[0] / 8), 255,
	 CV_THRESH_BINARY_INV | CV_THRESH_OTSU);*/

	/*cvThreshold(ia, ib, (int) avg.val[0] - 7 * (int) (avgStd.val[0] / 8), 255,
	 CV_THRESH_BINARY_INV);*/

	// double c = (2.5 * avgStd.val[0] / 10) - (avg.val[0] / 100); - Noche.
	double c = (2.5 * avgStd.val[0] / 10) - (avg.val[0] / 100);
	LOGD("C = %lf", c);

	cvAdaptiveThreshold(ia, ib, 255, CV_ADAPTIVE_THRESH_MEAN_C,
			CV_THRESH_BINARY, 17, c);
	cvNot(ib, ib);
	cvErode(ib, ib);
	cvDilate(ib, ib, NULL, 2);

	// Copy image to character sources before canny pic.
	cvCopy(ib, i_character_source);

	// Canny picture for detect contourns.
	cvCanny(ib, ib, 10, 30, 3);

	double aspectRatio;
	CvSeq* contour = 0;
	CvSeq* contourLow = 0;
	CvRect char_bounds, container_center;
	CvPoint pt1, pt2;
	int32_t digit_category;

	//Search countours in preprocesed image
	cvFindContours(ib, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL,
			CV_CHAIN_APPROX_SIMPLE, cvPoint(0, 0));
	//Optimize contours, reduce points
	contourLow = cvApproxPoly(contour, sizeof(CvContour), storage,
			CV_POLY_APPROX_DP, 1, 1);

	IplImage *image_segment = cvCreateImage(cvSize(32, 32), IPL_DEPTH_8U, 1);
	IplImage *neo_character = cvCreateImage(cvSize(64, 64), IPL_DEPTH_8U, 1);

	container_center = cvRect(16, 16, 32, 32);

#ifndef NDEBUG
	char name[40];
	int cc = 0;
#endif

	for (; contourLow != 0; contourLow = contourLow->h_next) {
		CvScalar color = CV_RGB( 255, 255, 255 ); //CV_RGB( rand()&200, rand()&200, rand()&200 );
		//We can draw the contour of object
		//cvDrawContours( imagen_color, contourLow, color, color, -1, 0, 8, cvPoint(0,0) );
		//Or detect bounding rect of contour
		char_bounds = cvBoundingRect(contourLow, 0);

		//LOGD("%dx%d [%d, %d]", rect.width, rect.height, rect.x, rect.y);
		if (char_bounds.height > 30 && char_bounds.width > 5
				&& char_bounds.height < (i_character_source->height - 15)) {

			aspectRatio = ((double) char_bounds.height)
					/ ((double) char_bounds.width);

			if (aspectRatio <= 1.2) {

				cvSetImageROI(i_character_source, char_bounds);
				cvResize(i_character_source, image_segment);
				cvResetImageROI(i_character_source);

				cvErode(image_segment, image_segment, NULL, 2);

				cvSet(neo_character, cvScalar(0, 0, 0, 0));
				cvSetImageROI(neo_character, container_center);
				cvCopy(image_segment, neo_character);
				cvResetImageROI(neo_character);

				cvTranspose(neo_character, neo_character);
				cvFlip(neo_character, neo_character, 1);

				// Apply new filters.
				//cvErode(neo_character, neo_character, NULL, 1);

				//
				// Categorize image.
				//
				digit_category = nc_categorize(get_nc_instance(),
						neo_character);

				if (digit_category != -1) {
					result_string[current_char_count++] = 48 + digit_category;
					env->CallVoidMethod(jo_pending_extraction,
							jmid_report_update, (char *) 48 + digit_category);

					LOGD("%dx%d %lf [%d, %d]: Category %d", char_bounds.width, char_bounds.height, aspectRatio, char_bounds.x, char_bounds.y, digit_category);
				}

#ifndef NDEBUG
				sprintf(name, "/mnt/sdcard/pending.0.%d.jpg", cc++);
				cvSaveImage(name, neo_character);
#endif
				pt1.x = char_bounds.x;
				pt2.x = (char_bounds.x + char_bounds.width);
				pt1.y = char_bounds.y;
				pt2.y = (char_bounds.y + char_bounds.height);

				cvRectangle(i_character_source, pt1, pt2, color, 1, 8, 0);
			}
		}

	}

	// Put data into Class string.
	result_string[current_char_count++] = 0;

	// Save image changes if debug is enabled.
	cvSaveImage(jch_file_path, i_character_source);

	// Clean up.
	cvReleaseImage(&image_segment);
	cvReleaseImage(&ib);
	cvReleaseImage(&ia);
	cvReleaseImage(&i_character_source);
	cvReleaseMemStorage(&storage);

	env->ReleaseStringUTFChars(js_file_path, jch_file_path);

	jstring final_java_string = env->NewStringUTF(result_string);
	free(result_string);

	// Report we are going to start.
	env->CallVoidMethod(jo_pending_extraction, jmid_report_end,
			final_java_string);
}

JNIEXPORT void JNICALL Java_com_calculatron_extraction_PendingExtraction_instaceNeocognitron(
		JNIEnv *env, jclass jc_pending_extraction, jstring js_neo_connections) {

	const char* jch_neo_connections; // File direction.
	Neocognitron *neocognitron; // Current instance, if clear, recently static created.

	LOGD("Creating Neocognitron instance");

	neocognitron = get_nc_instance();
	jch_neo_connections = env->GetStringUTFChars(js_neo_connections, 0);

	if (neocognitron != NULL)
		nc_init(neocognitron, jch_neo_connections);

	env->ReleaseStringUTFChars(js_neo_connections, jch_neo_connections);
}

JNIEXPORT void JNICALL Java_com_calculatron_extraction_PendingExtraction_destroyNeocognitron(
		JNIEnv *env, jclass jc_pending_extraction) {
	LOGD("Destroying Neocognitron instance");
	nc_destroy_instance();
}

