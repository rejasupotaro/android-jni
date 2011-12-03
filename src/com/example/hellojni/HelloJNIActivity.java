package com.example.hellojni;

import android.app.Activity;
import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

public class HelloJNIActivity extends Activity {
	private Bitmap bmp;
	private ImageView imageView;
	private TextView javaTimeText;
	private TextView nativeTimeText;
	
	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);

		Resources resource = getResources();

		bmp = BitmapFactory.decodeResource(resource, R.drawable.madomagi);
		imageView = (ImageView) findViewById(R.id.ImageView);

		imageView.setImageBitmap(bmp);

		Button javaButton = (Button) findViewById(R.id.JavaButton);
		javaButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				javaImageProcessing();			
			}
		});

		Button nativeButton = (Button) findViewById(R.id.NativeButton);
		nativeButton.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				nativeImageProcessing();
			}
		});
		
		javaTimeText = (TextView) findViewById(R.id.JavaTimeText);
		nativeTimeText = (TextView) findViewById(R.id.NativeTimeText);
	}

	private void javaImageProcessing() { 
		int width = bmp.getWidth();
		int height = bmp.getHeight();

		int[] pixels = new int[width * height];
		bmp.getPixels(pixels, 0, width, 0, 0, width, height);
		
		//int[] blendPixels = createShadowPixels(width, height);
		
		long startTime = System.currentTimeMillis();
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int pixel = pixels[x + y * width];
				//int blendPixel = blendPixels[x + y * width];
				//pixel = saturationTransform(pixel);
				
				int red = Color.red(pixel);
				int green = Color.green(pixel);
				int blue = Color.blue(pixel);
				/*
				int blendRed = Color.red(blendPixel);
				int blendGreen = Color.green(blendPixel);
				int blendBlue = Color.blue(blendPixel);
				
				red = burn(red, blendRed);
				green = burn(green, blendGreen);
				blue = burn(blue, blendBlue);
				
				red = multiple(red, blendRed);
				green = multiple(green, blendGreen);
				blue = multiple(blue, blendBlue);
				*/
				
				red = burn(red, red);
				green = burn(green, green);
				blue = burn(blue, blue);
				
				red = multiple(red, red);
				green = multiple(green, green);
				blue = multiple(blue, blue);
				
				pixels[x + y * width] = Color.rgb(red, green, blue);
			}
		}
		javaTimeText.setText((System.currentTimeMillis() - startTime) + "ミリ秒");

		Bitmap javaBmp = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
		javaBmp.setPixels(pixels, 0, width, 0, 0, width, height);
		imageView.setImageBitmap(javaBmp);
	}
	
	private int[] createShadowPixels(int width, int height) {
		int centerX = width / 2;
		int centerY = height / 2;
		
		int[] pixels = new int[width * height];
		
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				int dist = (int) Math.sqrt(Math.pow((x - centerX), 2) + Math.pow((y - centerY), 2));
				dist /= 5;
				int color = dist > 255 ? 255 : dist;
				pixels[x + y * width] = ~((color << 24) | (color << 16) | (color << 8) | color);
			}
		}
		return pixels;
	}
	
	private int saturationTransform(int pixel) {
		int red = Color.red(pixel);
		int green = Color.green(pixel);
		int blue = Color.blue(pixel);
		
		int ave = (red + green + blue) / 3;
		
		red -= (red - ave) / 2;
		green -= (green - ave) / 2;
		blue -= (blue - ave) / 2;
		
		pixel = Color.rgb(red, green, blue);
		
		return pixel;
	}
	
	private int multiple(int pa, int pb) {
		return pa * pb / 255;
	}
	
	private int burn(int pa, int pb) {
		if (pb == 0) return pa;
		int p = 255 - ((255 - pa) * 255 / pb);
		return p < 0 ? 0 : p;
	}
	
	private int gammaTransform(int x, double gamma) {
		return (int) (255 * Math.pow((x / 255.0), (1.0 / gamma)));
	}

	private void nativeImageProcessing() {
		int width = bmp.getWidth();
		int height = bmp.getHeight();
		
		int[] pixels = new int[width * height];
		bmp.getPixels(pixels, 0, width, 0, 0, width, height);
	
		long startTime = System.currentTimeMillis();
		//Log.d("**********", "result:" + imageProcessing(pixels, bmp.getWidth(), bmp.getHeight()));
		imageFilter(bmp);
		nativeTimeText.setText((System.currentTimeMillis() - startTime) + "ミリ秒");
		
		Bitmap nativeBmp = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
		nativeBmp.setPixels(pixels, 0, width, 0, 0, width, height);
		imageView.setImageBitmap(nativeBmp);
		
	}

	private native int imageProcessing(int[] pixels, int width, int height);
	private native void imageFilter(Bitmap bitmap);

	static {
		System.loadLibrary("ImageProcessing");
	}
}