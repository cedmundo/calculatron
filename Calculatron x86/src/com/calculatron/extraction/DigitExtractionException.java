package com.calculatron.extraction;

public class DigitExtractionException extends Exception {

	public static final int ORIENTATION_NOT_COMPATIBLE = 0x9001;

	private static final long serialVersionUID = -6654238887558465496L;
	private int code;

	public DigitExtractionException(int code) {
		super(String.format("Extraction exception code [%d]", code));
		this.code = code;
	}

	public int getCode() {
		return code;
	}
}
