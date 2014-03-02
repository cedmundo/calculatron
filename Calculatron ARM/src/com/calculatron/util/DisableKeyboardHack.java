package com.calculatron.util;

import android.text.Layout;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.EditText;

public class DisableKeyboardHack implements OnTouchListener {

	private EditText editText;

	public DisableKeyboardHack(EditText editText) {
		super();
		this.editText = editText;
	}

	@Override
	public boolean onTouch(View v, MotionEvent event) {
		switch (event.getAction()) {
		case MotionEvent.ACTION_DOWN:
			Layout layout = editText.getLayout();
			float x = event.getX() + editText.getScrollX();
			int offset = layout.getOffsetForHorizontal(0, x);
			if (offset > 0)
				if (x > layout.getLineMax(0))
					editText.setSelection(offset); // touch was at end of
													// text
				else
					editText.setSelection(offset - 1);
			break;
		}
		return true;
	}

}
