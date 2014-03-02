package com.calculatron;

import java.io.File;
import java.util.Locale;

import com.calculatron.extraction.PendingExtraction;
import com.calculatron.util.Decompress;

import android.os.AsyncTask;
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
				getString(R.string.loading), true);

		Log.d(StartActivty.class.getCanonicalName(), "Working directory: "
				+ getFilesDir().getAbsolutePath());

		PrepareUse pu = new PrepareUse(this);
		pu.execute(new Void[] { null });
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

	private class PrepareUse extends AsyncTask<Void, Void, Integer> {

		public StartActivty base;

		public PrepareUse(StartActivty startActivty) {
			this.base = startActivty;
		}

		@Override
		protected Integer doInBackground(Void... params) {
			try {
				Log.d(StartActivty.class.getCanonicalName(), "Checking files");

				File databin = new File(getFilesDir(), "data.bin");
				if (!databin.exists()) {
					Log.d(StartActivty.class.getCanonicalName(),
							"'data.bin' Does not exists, uncompressing from assets.");
					Decompress dec = new Decompress(base, "data.zip", base
							.getFilesDir().getAbsoluteFile() + "/");
					dec.unzip();

					Log.d(StartActivty.class.getCanonicalName(), "Unzipped!");
				}

				Log.d(StartActivty.class.getCanonicalName(),
						"Instancing neocognitron...");
				PendingExtraction.instaceNeocognitron(String.format(Locale.US,
						"%s/data.bin", getFilesDir().getAbsolutePath()));
			} catch (Exception ex) {
				return -1;
			}
			return 0;
		}

		@Override
		protected void onPostExecute(Integer result) {
			if (result == 0) {
				Intent intent = new Intent(base, CalculatorActivity.class);
				startActivity(intent);
				finish();
			} else
				Toast.makeText(base, base.getString(R.string.corrupt),
						Toast.LENGTH_LONG).show();
		}
	}
}
