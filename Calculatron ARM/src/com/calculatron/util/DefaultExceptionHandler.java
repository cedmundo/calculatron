package com.calculatron.util;

import java.lang.Thread.UncaughtExceptionHandler;

import android.util.Log;

import com.calculatron.camera.CameraHolder;

public class DefaultExceptionHandler implements UncaughtExceptionHandler {

	private CameraHolder holder;

	public DefaultExceptionHandler(CameraHolder holder) {
		this.holder = holder;
	}

	@Override
	public void uncaughtException(Thread arg0, Throwable arg1) {
		try {
			holder.closeCamera();
		} catch (Exception ex) {
			Log.e(DefaultExceptionHandler.class.getCanonicalName(),
					ex.getMessage(), ex);

			Log.e(DefaultExceptionHandler.class.getCanonicalName(),
					"Derived from exception", arg1);
		}
	}

}
