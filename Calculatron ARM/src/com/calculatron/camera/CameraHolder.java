package com.calculatron.camera;

import java.util.Date;

import android.content.Context;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.PictureCallback;

public class CameraHolder {

	public static final long MILLISECONDS_BETWEEN_PHOTOS = 1500;

	private Camera currentCamera;
	private CameraPreview currentPreview;
	private Context context;

	private boolean open;

	private long lastDispatchedTouch;
	private PictureCallback callback;

	public CameraHolder(Context context, PictureCallback callback) {
		this.context = context;
		this.callback = callback;
		this.lastDispatchedTouch = new Date().getTime();
		this.open = false;
	}

	public CameraPreview getCurrentPreview() {
		return currentPreview;
	}

	public void setCurrentPreview(CameraPreview currentPreview) {
		this.currentPreview = currentPreview;
	}

	public void setCurrentCamera(Camera currentCamera) {
		this.currentCamera = currentCamera;
	}

	public Camera getCurrentCamera() {
		return currentCamera;
	}

	public Camera openCamera() {
		Camera c = Camera.open(CameraInfo.CAMERA_FACING_BACK);
		setCurrentCamera(c);

		currentPreview = new CameraPreview(context, currentCamera);
		open = true;
		return c;
	}

	public void closeCamera() {
		if (currentCamera != null) {
			currentCamera.stopPreview();
			currentCamera.setPreviewCallback(null);
			currentCamera.release();
			currentCamera = null;
			currentPreview = null;
			open = false;
		}
	}

	public boolean isOpen() {
		return open;
	}

	public void pause() throws Exception {
		currentPreview.setEnabled(false);

		currentCamera.stopPreview();
		currentCamera.unlock();
	}

	public void resume() throws Exception {
		currentPreview.setEnabled(true);

		currentCamera.reconnect();
		currentCamera.startPreview();
	}

	public void toggleFlashMode() {
		Parameters p = currentCamera.getParameters();

		if (currentCamera.getParameters().getFlashMode() != Parameters.FLASH_MODE_TORCH) {
			p.setFlashMode(Parameters.FLASH_MODE_ON);
		} else {
			p.setFlashMode(Parameters.FLASH_MODE_OFF);
		}

		currentCamera.setParameters(p);
	}

	public void takePhoto() {
		long time = new Date().getTime();
		if (time >= lastDispatchedTouch + MILLISECONDS_BETWEEN_PHOTOS) {
			currentCamera.takePicture(null, null, callback);
			lastDispatchedTouch = new Date().getTime();
		}
	}
}
