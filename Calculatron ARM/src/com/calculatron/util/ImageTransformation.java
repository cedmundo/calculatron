package com.calculatron.util;

import android.graphics.Bitmap;
import static android.graphics.Bitmap.createScaledBitmap;
import static android.graphics.Bitmap.createBitmap;
import android.graphics.Canvas;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;

public class ImageTransformation {

	public static Bitmap transform(Bitmap bmpOriginal, int orientation)
			throws TransformingException {

		int finalWidth, finalHeight, rotationHeight, rotationWidth;

		Bitmap rotatedBitmap;
		Bitmap scaledBitmap;
		Bitmap bmpGrayscale;
		Bitmap effectiveBitmap;
		Matrix rotationMatrix;

		rotationMatrix = new Matrix();

		switch (orientation) {
		case 0:
			rotationHeight = bmpOriginal.getWidth();
			rotationWidth = bmpOriginal.getHeight();

			rotationMatrix.setRotate(-90, 0, 0); // Compensate angle.
			rotationMatrix.postTranslate(0, rotationHeight);

			break;
		case 90:
			rotationHeight = bmpOriginal.getHeight();
			rotationWidth = bmpOriginal.getWidth();
			break;
		case 180:
			rotationHeight = bmpOriginal.getWidth();
			rotationWidth = bmpOriginal.getHeight();

			rotationMatrix.setRotate(90); // Compensate angle.
			rotationMatrix.postTranslate(rotationWidth, 0);

			break;
		case 270:
			rotationHeight = bmpOriginal.getHeight();
			rotationWidth = bmpOriginal.getWidth();

			rotationMatrix.setRotate(180); // Compensate angle.
			rotationMatrix.postTranslate(rotationWidth, rotationHeight);
			break;
		default:
			throw new TransformingException(
					TransformingException.ORIENTATION_INCOMPATIBLE);
		}

		rotatedBitmap = Bitmap.createBitmap(rotationWidth, rotationHeight,
				bmpOriginal.getConfig());
		Canvas tmpCanvas = new Canvas(rotatedBitmap);
		tmpCanvas.drawBitmap(bmpOriginal, rotationMatrix, null);
		tmpCanvas.setBitmap(null);

		finalWidth = (int) (rotationWidth * 1.6);
		finalHeight = (int) (rotationHeight * 1.6);

		scaledBitmap = createScaledBitmap(rotatedBitmap, finalWidth,
				finalHeight, true);

		bmpGrayscale = createBitmap(finalWidth, finalHeight,
				Bitmap.Config.ARGB_8888);

		Canvas c = new Canvas(bmpGrayscale);
		Paint paint = new Paint();
		ColorMatrix cm = new ColorMatrix();
		cm.setSaturation(0);
		ColorMatrixColorFilter f = new ColorMatrixColorFilter(cm);
		paint.setColorFilter(f);
		c.drawBitmap(scaledBitmap, 0, 0, paint);

		double effectiveArea = finalWidth * 0.2;
		effectiveBitmap = createBitmap(bmpGrayscale, 0, 0, (int) effectiveArea,
				finalHeight);
		return effectiveBitmap;
	}

	public static class TransformingException extends Exception {

		/**
		 * UUID
		 */
		private static final long serialVersionUID = 355379017401519582L;
		private int type;

		public static final int PICTURE_TOO_SMALL = 0;
		public static final int OUT_OF_BOUNDS = 1;
		public static final int ORIENTATION_INCOMPATIBLE = 2;

		public TransformingException(int type) {
			this.type = type;
		}

		public int getType() {
			return type;
		}
	}
}
