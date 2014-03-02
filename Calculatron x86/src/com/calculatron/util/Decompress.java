package com.calculatron.util;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

public class Decompress {
	private AssetManager assetManager;
	private String zip;
	private String loc;

	public Decompress(Context context, String zipFile, String location) {
		this.assetManager = context.getAssets();
		zip = zipFile;
		loc = location;

		dirChecker("");
	}

	public void unzip() throws IOException {
		InputStream fin = assetManager.open(zip);
		ZipInputStream zin = new ZipInputStream(fin);
		ZipEntry ze = null;
		while ((ze = zin.getNextEntry()) != null) {

			if (ze.isDirectory()) {
				dirChecker(ze.getName());
			} else {
				Log.d(Decompress.class.getCanonicalName(), "Decompresing: "
						+ ze.getName());
				FileOutputStream fout = new FileOutputStream(loc + ze.getName());

				int readLen = -1;
				byte[] buff = new byte[2048];

				// Loop until End of File and write the contents to the output
				// stream
				while ((readLen = zin.read(buff)) != -1) {
					fout.write(buff, 0, readLen);
				}

				zin.closeEntry();
				fout.close();
			}

		}
		zin.close();

	}

	private void dirChecker(String dir) {
		File f = new File(loc + dir);

		if (!f.isDirectory()) {
			f.mkdirs();
		}
	}
}