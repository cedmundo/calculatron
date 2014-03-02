package com.calculatron;

import android.os.Bundle;
import android.app.Activity;
import android.content.Intent;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ArrayAdapter;
import android.widget.ListView;

public class CalculatorHistoryActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_calculator_history);

		Bundle b = getIntent().getExtras();
		String argumented[] = b.getStringArray("history");

		ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
				android.R.layout.simple_list_item_1);
		for (String a : argumented) {
			if (!a.equals("IGN"))
				adapter.add(a);
		}

		if (argumented.length == 0)
			adapter.add(getString(R.string.message_no_history));

		((ListView) findViewById(R.id.calculatorhistory_listview_history))
				.setAdapter(adapter);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.calculator_history, menu);
		return true;
	}

	@Override
	public boolean onMenuItemSelected(int featureId, MenuItem item) {
		if (item.getItemId() == R.id.action_about) {
			Intent about = new Intent(this, AboutActivity.class);
			startActivity(about);
		}

		return true;
	}
}
