package com.calculatron.controllers;

import java.io.Serializable;
import java.text.DecimalFormat;
import java.text.DecimalFormatSymbols;
import java.util.ArrayList;
import java.util.Locale;

import com.calculatron.CalculatorActivity;
import com.calculatron.CalculatorHistoryActivity;
import com.calculatron.R;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;

public class CalculatorController extends Handler implements Serializable {

	private static final long serialVersionUID = -6847166322677193917L;
	public static final int MESSAGE_EXTRACTION_START = 0;
	public static final int MESSAGE_EXTRACTION_NOTIFY = 1;
	public static final int MESSAGE_EXTRACTION_END = 2;
	public static final int MESSAGE_REPORT_EXCEPTION = 3;
	public static final int MESSAGE_DISTPATCH_VIEW_EVENT = 4;

	public static final String[] OP_STRINGS = { "\u002B", "\u002D", "\u00D7",
			"\u00F7", "=" };

	public static final short NEXT_OP_SUM = 0;
	public static final short NEXT_OP_SUB = 1;
	public static final short NEXT_OP_MUL = 2;
	public static final short NEXT_OP_DIV = 3;
	public static final short NEXT_OP_IGN = 4;

	private transient CalculatorActivity view;
	private ArrayList<String[]> history;
	private short selectedOperation;
	private StringBuffer virtualDisplay;
	private boolean locked;
	private boolean hitFunc;

	public CalculatorController(CalculatorActivity view) {
		this.view = view;
		this.virtualDisplay = new StringBuffer(31); // 15 digits, 1 dot, 14
													// decimals, 1 minus sign.
		this.history = new ArrayList<String[]>();
		this.selectedOperation = NEXT_OP_SUM;
		this.locked = false;
		this.hitFunc = false;
	}

	public void setView(CalculatorActivity view) {
		this.view = view;
	}

	@Override
	public void handleMessage(Message msg) {
		switch (msg.what) {
		case MESSAGE_REPORT_EXCEPTION:
			Exception ex = (Exception) msg.obj;

			String message = view.getString(R.string.message_unexcepted_error,
					ex.getMessage());
			Log.e(CalculatorActivity.class.getCanonicalName(), message, ex);
			view.showMessageToUser(message);

			break;

		case MESSAGE_EXTRACTION_START:
			clearAndSync();
			view.setBusyState();
			locked = true;
			break;
		case MESSAGE_EXTRACTION_NOTIFY:
			Byte ch = (Byte) msg.obj;
			if (virtualDisplay.length() < 15) {
				virtualDisplay.append(new String(new byte[] { ch }));
				sync();
			}
			break;
		case MESSAGE_EXTRACTION_END:
			String recognitionData = (String) msg.obj;
			int max = 15;
			if (recognitionData.length() < 15)
				max = recognitionData.length();

			replaceVirtualDisplay(recognitionData.substring(0, max));
			sync();
			view.setIdleState();
			view.setViewCaretPosition(recognitionData.length());
			locked = false;
			break;
		case MESSAGE_DISTPATCH_VIEW_EVENT:
			View v = (View) msg.obj;
			butonPress(v.getId());
			break;
		default:
			break;
		}
	}

	public void sync() {
		view.setTextDisplay(virtualDisplay.toString());
	}

	public void syncCaret() {
		view.setViewCaretPosition(virtualDisplay.length());
	}

	public void clear() {
		virtualDisplay.delete(0, virtualDisplay.length());
	}

	public void clearAndSync() {
		clear();
		sync();
	}

	public void replaceVirtualDisplay(String text) {
		clear();
		virtualDisplay.append(text);
	}

	public void insertSyncAndMoveCaret(int pos, char digit) {
		virtualDisplay.insert(pos, digit);
		sync();
		view.setViewCaretPosition(pos + 1);
	}

	public void insertCharacter(char digit) {
		if (hitFunc) {
			this.history
					.add(new String[] { this.virtualDisplay.toString(), "=" });
			clear();
			sync();
			hitFunc = false;
		}

		int currentCaretPosition = view.getViewCaretPosition();
		int indexOfDot = virtualDisplay.indexOf(".");

		if (currentCaretPosition > this.virtualDisplay.length()
				&& currentCaretPosition == -1) {
			return;
		}

		if (digit == '.' && virtualDisplay.indexOf(".") == -1) {
			insertSyncAndMoveCaret(currentCaretPosition, '.');
			return;
		}

		int integerMax = virtualDisplay.indexOf("-") != -1 ? 16 : 15;
		int decimalMax = 15;

		try {
			if (indexOfDot != -1 && currentCaretPosition <= indexOfDot) {
				if (virtualDisplay.substring(0, indexOfDot).length() < integerMax) {
					insertSyncAndMoveCaret(currentCaretPosition, digit);
				} else {
					view.showMessageToUser(view
							.getString(R.string.message_number_too_long));
				}

			} else if (indexOfDot != -1 && currentCaretPosition > indexOfDot) {
				if (virtualDisplay.substring(indexOfDot,
						virtualDisplay.length()).length() < decimalMax) {
					insertSyncAndMoveCaret(currentCaretPosition, digit);
				} else {
					view.showMessageToUser(view
							.getString(R.string.message_decimal_too_long));
				}
			} else {
				if (virtualDisplay.length() < integerMax) {
					insertSyncAndMoveCaret(currentCaretPosition, digit);
				} else {
					view.showMessageToUser(view
							.getString(R.string.message_number_too_long));
				}
			}
		} catch (IndexOutOfBoundsException ex) {
			String message = view.getString(R.string.message_unexcepted_error,
					ex.getMessage());
			Log.e(CalculatorActivity.class.getCanonicalName(), message, ex);
			view.showMessageToUser(message);
		}
	}

	public void removeCharacter() {
		int currentCaretPosition = view.getViewCaretPosition();

		if (currentCaretPosition > 0
				&& currentCaretPosition <= virtualDisplay.length()) {
			virtualDisplay.delete(currentCaretPosition - 1,
					currentCaretPosition);
			sync();
			view.setViewCaretPosition(currentCaretPosition - 1);
		}
	}

	public void equalFunction() {
		String result = operateCurrentWithLast();
		history.add(new String[] { virtualDisplay.toString(),
				OP_STRINGS[this.selectedOperation] });
		history.add(new String[] { result, "=" });
		replaceVirtualDisplay(result);
	}

	public String operateCurrentWithLast() {
		int hSize = this.history.size();

		String lastAsString = hSize == 0 ? "0" : this.history.get(hSize - 1)[0];
		String currentAsString = virtualDisplay.toString();

		lastAsString = lastAsString.equals("") ? "0" : lastAsString;
		currentAsString = currentAsString.equals("") ? "0" : currentAsString;

		double current = 0;
		double last = 0;

		try {
			last = Double.valueOf(lastAsString);
			current = Double.valueOf(currentAsString);
		} catch (NumberFormatException ex) {
			String message = view.getString(
					R.string.message_numberformat_error, ex.getMessage());
			Log.e(CalculatorActivity.class.getCanonicalName(), message, ex);
			view.showMessageToUser(message);
		}

		switch (this.selectedOperation) {
		case NEXT_OP_SUM:
			current = last + current;
			break;

		case NEXT_OP_SUB:
			current = last - current;
			break;

		case NEXT_OP_MUL:
			current = last * current;
			break;

		case NEXT_OP_DIV:
			current = last / current;
			break;

		case NEXT_OP_IGN:
			break;

		default:
			String message = view.getString(R.string.message_unexcepted_error,
					"Unknown operation ID " + selectedOperation);
			Log.e(CalculatorActivity.class.getCanonicalName(), message);
			view.showMessageToUser(message);
			break;
		}

		DecimalFormatSymbols sym = DecimalFormatSymbols
				.getInstance(Locale.ENGLISH);
		DecimalFormat formater = new DecimalFormat(
				"###############.############", sym);

		return formater.format(current);
	}

	public void butonPress(int id) {
		if (locked) // Do not receive commands during lock.
			return;

		switch (id) {
		case R.id.calculator_button_ce:
			this.history.clear();
			clearAndSync();
			break;

		case R.id.calculator_button_c:
			clearAndSync();
			break;

		case R.id.calculator_button_e:
			removeCharacter();
			break;

		case R.id.calculator_button_h:
			Bundle b = new Bundle();

			String[] histlist = new String[this.history.size()];

			for (int i = 0; i < this.history.size(); i++) {
				String[] values = this.history.get(i);
				histlist[i] = values[1] + " " + values[0];
			}
			b.putStringArray("history", histlist);

			Intent intent = new Intent(view, CalculatorHistoryActivity.class);
			intent.putExtras(b);
			view.startActivity(intent);

			break;

		case R.id.calculator_button_one:
			insertCharacter('1');
			break;

		case R.id.calculator_button_two:
			insertCharacter('2');
			break;

		case R.id.calculator_button_three:
			insertCharacter('3');
			break;

		case R.id.calculator_button_four:
			insertCharacter('4');
			break;

		case R.id.calculator_button_five:
			insertCharacter('5');
			break;

		case R.id.calculator_button_six:
			insertCharacter('6');
			break;

		case R.id.calculator_button_seven:
			insertCharacter('7');
			break;

		case R.id.calculator_button_eight:
			insertCharacter('8');
			break;

		case R.id.calculator_button_nine:
			insertCharacter('9');
			break;

		case R.id.calculator_button_zero:
			insertCharacter('0');
			break;

		case R.id.calculator_button_dot:
			insertCharacter('.');
			break;

		case R.id.calculator_button_equalsto:
			hitFunc = true;

			short curOp = selectedOperation;
			String curInpt = virtualDisplay.toString();

			equalFunction();
			sync();
			syncCaret();

			this.selectedOperation = curOp;
			replaceVirtualDisplay(curInpt);

			break;

		case R.id.calculator_button_plus:
			hitFunc = true;
			selectedOperation = NEXT_OP_SUM;
			break;
		case R.id.calculator_button_minus:
			hitFunc = true;
			selectedOperation = NEXT_OP_SUB;
			break;
		case R.id.calculator_button_product:
			hitFunc = true;
			selectedOperation = NEXT_OP_MUL;
			break;
		case R.id.calculator_button_quotient:
			hitFunc = true;
			selectedOperation = NEXT_OP_DIV;
			break;

		default:
			view.showMessageToUser(view.getString(
					R.string.message_unexcepted_error, "Not valid button ID "
							+ id));
			break;
		}
	}
}
