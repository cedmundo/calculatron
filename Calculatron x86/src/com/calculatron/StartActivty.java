package com.calculatron;

import java.io.File;
import java.io.IOException;

import com.calculatron.util.Decompress;

import android.os.Bundle;
import android.app.Activity;
import android.app.ProgressDialog;
import android.content.Intent;
import android.util.Log;
import android.view.Menu;
import android.widget.Toast;

public class StartActivty extends Activity {

	private ProgressDialog dialog;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_start);

		dialog = ProgressDialog.show(StartActivty.this, "Loading",
				"Calculatron is preparing for its first run ...", true);
		
		Log.d(StartActivty.class.getCanonicalName(), "Working directory: "
				+ getFilesDir().getAbsolutePath());

		try {
			//Uncomment to reset bin data.
			//new File(getFilesDir(), "data.bin").delete();
			
			checkFiles();
		} catch (IOException ex) {
			Log.e("During file decompression",
					StartActivty.class.getCanonicalName(), ex);
			Toast.makeText(this,
					"Init files cannot be installed: " + ex.getMessage(),
					Toast.LENGTH_LONG).show();
		}		
	}
	
	@Override
	protected void onDestroy() {
		dialog.dismiss();
		super.onDestroy();
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		return true;
	}

	private void checkFiles() throws IOException {

		Log.d(StartActivty.class.getCanonicalName(), "Checking files");

		File databin = new File(getFilesDir(), "data.bin");
		if (!databin.exists()) {
			Log.d(StartActivty.class.getCanonicalName(),
					"'data.bin' Does not exists, uncompressing from assets.");
			Decompress dec = new Decompress(this, "data.zip", getFilesDir()
					.getAbsolutePath() + "/");
			dec.unzip();

			Log.d(StartActivty.class.getCanonicalName(), "Unzipped!");
		}

		Intent intent = new Intent(this, CalculatorActivity.class);
		startActivity(intent);
		finish();
	}
}
