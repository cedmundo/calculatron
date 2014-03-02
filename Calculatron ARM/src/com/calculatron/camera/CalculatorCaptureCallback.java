package com.calculatron.camera;

import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Locale;

import com.calculatron.controllers.CalculatorController;
import com.calculatron.extraction.PendingExtraction;
import static com.calculatron.util.ImageTransformation.transform;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.hardware.Camera;
import android.hardware.Camera.PictureCallback;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class CalculatorCaptureCallback implements PictureCallback {

	private int orientation;
	private Handler controller;

	public CalculatorCaptureCallback(Handler controller) {
		this.controller = controller;
		this.orientation = 0;
	}

	public int getOrientation() {
		return orientation;
	}

	public void setOrientation(int orientation) {
		this.orientation = orientation;
	}

	public Handler getController() {
		return controller;
	}

	public void setController(Handler controller) {
		this.controller = controller;
	}

	@Override
	public void onPictureTaken(byte[] data, Camera camera) {
		final byte[] cameraData = data;

		Thread procThread = new Thread(new Runnable() {

			@Override
			public void run() {
				PendingExtraction taskedExtraction;

				Bitmap original = BitmapFactory.decodeByteArray(cameraData, 0,
						cameraData.length);

				try {
					Bitmap transformed = transform(original, orientation);
					taskedExtraction = pendingBitmap(transformed);
					taskedExtraction.extract();
				} catch (Exception ex) {
					Message m = new Message();
					m.what = CalculatorController.MESSAGE_REPORT_EXCEPTION;
					m.obj = ex;
					controller.sendMessage(m);
				}
			}
		});

		procThread.start();
	}

	public PendingExtraction pendingBitmap(Bitmap bitmap) {
		String filePath = String.format(Locale.US, "%s/pending.%d.jpeg",
				Environment.getExternalStorageDirectory().getPath(), 0);

		FileOutputStream outputStream = null;
		Log.i(CalculatorCaptureCallback.class.getCanonicalName(),
				String.format(Locale.US, "Tasked image %s", filePath));
		try {
			outputStream = new FileOutputStream(filePath);
			bitmap.compress(Bitmap.CompressFormat.JPEG, 100, outputStream);
		} catch (IOException ex) {
			Message m = new Message();
			m.what = CalculatorController.MESSAGE_REPORT_EXCEPTION;
			m.obj = ex;
			controller.sendMessage(m);
		} finally {
			try {
				if (outputStream != null)
					outputStream.close();
			} catch (IOException ex) {
				Message m = new Message();
				m.what = CalculatorController.MESSAGE_REPORT_EXCEPTION;
				m.obj = ex;
				controller.sendMessage(m);
			}
		}

		PendingExtraction result = new PendingExtraction(filePath, controller);
		return result;
	}
}
