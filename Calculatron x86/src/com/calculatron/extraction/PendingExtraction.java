package com.calculatron.extraction;

import com.calculatron.controllers.CalculatorController;

import android.os.Handler;
import android.os.Message;

public class PendingExtraction {

	public static String NEO_CONNECTIONS_FILE = null;

	private String filePath;
	private Handler handler;

	public PendingExtraction(String filePath, Handler handler) {
		this.filePath = filePath;
		this.handler = handler;
	}

	public String getFilePath() {
		return filePath;
	}

	public void reportStart() {
		handler.sendEmptyMessage(CalculatorController.MESSAGE_EXTRACTION_START);
	}

	public void reportUpdate(byte digit) {
		Message m = new Message();
		m.what = CalculatorController.MESSAGE_EXTRACTION_NOTIFY;
		m.obj = Byte.valueOf(digit); // new Character(digit);

		handler.sendMessage(m);
	}

	public void reportEnd(String value) {
		Message m = new Message();
		m.what = CalculatorController.MESSAGE_EXTRACTION_END;
		m.obj = value; // new Character(digit);

		handler.sendMessage(m);
	}
	
	public native void extract() throws DigitExtractionException;
	static {
		System.loadLibrary("Calculatron.x86");
	}
}
