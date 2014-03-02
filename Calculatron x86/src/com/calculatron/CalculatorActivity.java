package com.calculatron;

import java.util.Locale;

import com.calculatron.camera.CalculatorCaptureCallback;
import com.calculatron.camera.CameraHolder;
import com.calculatron.controllers.CalculatorController;
import com.calculatron.extraction.PendingExtraction;
import com.calculatron.util.DisableKeyboardHack;

import android.hardware.Camera;
import android.os.Bundle;
import android.os.Message;
import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.view.ViewTreeObserver;
import static android.view.WindowManager.LayoutParams.SOFT_INPUT_STATE_ALWAYS_HIDDEN;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.Toast;

/**
 * This activity controls the two several options of application: 1. Creates a
 * camera preview with cropped frame that allows to users see what numbers will
 * be taken and processed. 2. Shows the current calculus record.
 * 
 * CalculatorActivity can receive parameter ShowCalculusRecord=Rowid to show a
 * specific record and keep working on that.
 * 
 * @author Carlos Edmundo Martínez Mendoza
 * 
 */
public class CalculatorActivity extends Activity implements OnClickListener {

	private CalculatorCaptureCallback pictureCallback;
	private CalculatorController controller;
	private CameraHolder holder;

	private EditText viewDisplay;

	public CalculatorActivity() {
		controller = new CalculatorController(this);
		pictureCallback = new CalculatorCaptureCallback(controller);
		holder = new CameraHolder(this, pictureCallback);
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_calculator);

		try {
			Configuration config = getResources().getConfiguration();
			initCameraPreview(config.orientation);
		} catch (Exception ex) {
			Log.e(CalculatorActivity.class.getCanonicalName(), ex.getMessage(),
					ex);
			Toast.makeText(this, ex.getMessage(), Toast.LENGTH_LONG).show();
		}

		// This will disable soft keyboard, but will keep caret positioning.
		viewDisplay = (EditText) findViewById(R.id.calculator_edittext_display);
		viewDisplay.setOnTouchListener(new DisableKeyboardHack(viewDisplay));

		this.getWindow().setSoftInputMode(SOFT_INPUT_STATE_ALWAYS_HIDDEN);

		PendingExtraction.NEO_CONNECTIONS_FILE = String.format(Locale.US,
				"%s/data.bin", getFilesDir().getAbsolutePath());
		
		CalculatorCaptureCallback.CACHE_LOCATION = "/mnt/sdcard";
		//CalculatorCaptureCallback.CACHE_LOCATION = getCacheDir()
		//		.getAbsolutePath();
	}

	public void onSaveInstanceState(Bundle savedInstanceState) {
		super.onSaveInstanceState(savedInstanceState);
		savedInstanceState.putSerializable("Controller", this.controller);
	}

	@Override
	protected void onRestoreInstanceState(Bundle savedInstanceState) {
		super.onRestoreInstanceState(savedInstanceState);

		CalculatorController c = (CalculatorController) savedInstanceState
				.getSerializable("Controller");
		if (c != null) {
			this.controller = c;
			this.controller.setView(this);
		}
	}

	@Override
	protected void onPause() {
		super.onPause();
		try {
			holder.pause();
		} catch (Exception ex) {
			// Ignore
		}
	}

	@Override
	protected void onResume() {
		super.onResume();
		try {
			holder.resume();
		} catch (Exception ex) {
			// Ignore.
		}
	}

	@Override
	protected void onDestroy() {
		holder.closeCamera();
		super.onDestroy();
	}

	@Override
	public void onClick(View v) {
		Message m = new Message();
		m.what = CalculatorController.MESSAGE_DISTPATCH_VIEW_EVENT;
		m.obj = v;

		controller.sendMessage(m);
	}

	@Override
	public boolean onMenuItemSelected(int featureId, MenuItem item) {
		/*
		 * switch (item.getItemId()) { case R.id.action_debug:
		 * android.content.Intent intent = new android.content.Intent();
		 * intent.setAction(android.content.Intent.ACTION_VIEW);
		 * intent.setDataAndType(android.net.Uri.fromFile(new java.io.File(
		 * Environment.getExternalStorageDirectory().getPath(),
		 * "pending.0.jpeg")), "image/jpeg"); startActivity(intent); break; case
		 * R.id.action_flash: holder.toggleFlashMode(); break; default: break; }
		 */
		if (item.getItemId() == R.id.action_about) {
			Intent about = new Intent(this, AboutActivity.class);
			startActivity(about);
		}
		return true;
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.calculator, menu);
		return true;
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);

		FrameLayout preview = (FrameLayout) findViewById(R.id.calculator_framelayout_camerapreview);
		adjustPreviewEffectiveSurface(preview, newConfig.orientation);
	}

	public void setBusyState() {
		findViewById(R.id.calculator_progress).setVisibility(View.VISIBLE);
		setTextDisplay(getString(R.string.main_calc_busy));
	}

	public void setIdleState() {
		findViewById(R.id.calculator_progress).setVisibility(View.GONE);
		Thread previewStart = new Thread(new Runnable() {
			@Override
			public void run() {
				try {
					holder.getCurrentCamera().startPreview();
				} catch (Exception ex) {
					Message m = new Message();

					m.what = CalculatorController.MESSAGE_REPORT_EXCEPTION;
					m.obj = ex;
					controller.sendMessage(m);
				}
			}
		});

		previewStart.start();
	}

	public void showMessageToUser(String message) {
		Toast.makeText(this, message, Toast.LENGTH_LONG).show();
	}

	public void setTextDisplay(String text) {
		viewDisplay.setText(text);
	}

	public int getViewCaretPosition() {
		return viewDisplay.getSelectionStart();
	}

	public void setViewCaretPosition(int index) {
		if (index <= viewDisplay.getText().length() && index > -1)
			viewDisplay.setSelection(index);
	}

	private void adjustPreviewEffectiveSurface(FrameLayout preview,
			int orientation) {
		changeCameraDisplayOrientation();

		LinearLayout controls = (LinearLayout) findViewById(R.id.calculator_linearlayout_controls);
		LayoutParams controlsParameters = controls.getLayoutParams();

		float effectiveArea = preview.getHeight() * 0.8f;
		controlsParameters.height = (int) effectiveArea;

		controls.setLayoutParams(controlsParameters);

	}

	private void initCameraPreview(int orientation) {
		// Create an instance of Camera
		holder.openCamera();

		// Create our Preview view and set it as the content of our activity.
		final FrameLayout preview = (FrameLayout) findViewById(R.id.calculator_framelayout_camerapreview);
		final int or = orientation;

		preview.addView(holder.getCurrentPreview());
		preview.getViewTreeObserver().addOnGlobalLayoutListener(
				new ViewTreeObserver.OnGlobalLayoutListener() {
					public void onGlobalLayout() {
						adjustPreviewEffectiveSurface(preview, or);
					}
				});

		preview.setOnTouchListener(new View.OnTouchListener() {
			@Override
			public boolean onTouch(View view, MotionEvent event) {
				if (event.getY() < preview.getHeight() * 0.2f) {
					holder.takePhoto();
				}
				return false;
			}
		});
	}

	private void changeCameraDisplayOrientation() {
		android.hardware.Camera.CameraInfo info = new android.hardware.Camera.CameraInfo();
		android.hardware.Camera.getCameraInfo(
				Camera.CameraInfo.CAMERA_FACING_BACK, info);
		int rotation, result, degrees;

		rotation = getWindowManager().getDefaultDisplay().getRotation();
		degrees = 0;
		result = 0;

		switch (rotation) {
		case Surface.ROTATION_0:
			degrees = 0;
			break;
		case Surface.ROTATION_90:
			degrees = 90;
			break;
		case Surface.ROTATION_180:
			degrees = 180;
			break;
		case Surface.ROTATION_270:
			degrees = 270;
			break;
		}

		result = (info.orientation - degrees + 360) % 360;

		try {
			pictureCallback.setOrientation(result);
			Camera cam = holder.getCurrentCamera();
			if (cam != null)
				cam.setDisplayOrientation(result);
		} catch (RuntimeException ex) {
			// Ignore.
		}
	}
}
