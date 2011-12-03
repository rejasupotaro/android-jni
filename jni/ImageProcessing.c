#include <jni.h>
#include <time.h>
#include <android/log.h>
#include <android/bitmap.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <cpu-features.h>
#include "image_processing-intrinsics.h"

#define DEBUG 0

#if DEBUG
#include <android/log.h>
#  define D(x...) __android_log_print(ANDROID_LOG_INFO,"image_processing",x)
#else
#  define D(...) do {} while (0)
#endif

#define  LOG_TAG    "imageProcessing"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

typedef struct
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
}  rgba;

int* createShadowPixels(int width, int height) {
	int centerX = width / 2;
	int centerY = height / 2;

	int pixels[width * height];

	int x, y;
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			int dist = (int) sqrt(pow((x - centerX), 2) + pow((y - centerY), 2));
			dist /= 5;
			int color = dist > 255 ? 255 : dist;
			pixels[x + y * width] = ~((color << 24) | (color << 16) | (color << 8) | color);
		}
	}

	return pixels;
}

static __inline__ int multiple(int pa, int pb) {
	return pa * pb / 255;
}

static __inline__ int burn(int pa, int pb) {
	if (pb == 0) return pa;
	int p = 255 - ((255 - pa) * 255 / pb);
	return p < 0 ? 0 : p;
}

jint Java_com_example_hellojni_HelloJNIActivity_imageProcessing(JNIEnv* env,
		jobject thiz, jintArray src, jint w, jint h)
{
	uint red, green, blue;
	uint blendRed, blendGreen, blendBlue;
	int i;
	jint* pixels = (*env)->GetIntArrayElements(env, src, 0);

	int centerX = w / 2;
	int centerY = h / 2;

	//int* blendPixels = createShadowPixels(w, h);

	for (i = 0; i < (w * h); i++) {
		//pixels[i] = saturationTransform(pixels[i]);

		red = (pixels[i] & 0x00FF0000) >> 16;
		green = (pixels[i] & 0x0000FF00) >> 8;
		blue = (pixels[i] & 0x000000FF);
		/*
		blendRed = (blendPixels[i] & 0x00FF0000) >> 16;
		blendGreen = (blendPixels[i] & 0x0000FF00) >> 8;
		blendBlue = (blendPixels[i] & 0x000000FF);

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

		pixels[i] = (0xFF000000 | (red << 16) | (green << 8) | blue);
	}

	(*env)->ReleaseIntArrayElements(env, src, pixels, 0);

	return w;
}

static void filter(AndroidBitmapInfo* info, void* pixels) {
	int x, y;
	int width = info->width;
	int height = info->height;

	burn_neon_instrinsics(info, pixels, width, height);

	for (y = 0; y < height; y++) {
		rgba* line = (rgba*) pixels;
		for (x = 0; x < width; x++) {
			rgba pixel;

			pixel.red = multiple(line[x].red, line[x].red);
			pixel.green = multiple(line[x].green, line[x].green);
			pixel.blue = multiple(line[x].blue, line[x].blue);

			pixel.alpha = line[x].alpha;

			line[x] = pixel;
		}

		pixels = (char*) pixels + info->stride;
	}
}

JNIEXPORT void JNICALL Java_com_example_hellojni_HelloJNIActivity_imageFilter(JNIEnv* env, jobject obj, jobject bitmap)
{
	AndroidBitmapInfo info;
	void* pixels;
	int ret;
	static int init;
	uint64_t features;

	if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
		LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
		return;
	}

	if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
		LOGE("Bitmap format is not RGBA_8888 !");
		return;
	}

	if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
		LOGE("AndroidBitmap_lockPxels() failed ! error=%d", ret);
	}

	if (android_getCpuFamily() != ANDROID_CPU_FAMILY_ARM) {
		LOGE("Not an ARM CPU !", -1);
		return;
	}

	features = android_getCpuFeatures();
	if ((features & ANDROID_CPU_ARM_FEATURE_ARMv7) == 0) {
		LOGE("Not an ARMv7 CPU !", -1);
		return;
	}

	if ((features & ANDROID_CPU_ARM_FEATURE_NEON) == 0) {
		LOGE("CPU doesn't support NEON !", -1);
		return;
	}

	filter(&info, pixels);

	AndroidBitmap_unlockPixels(env, bitmap);
/*
#ifdef HAVE_NEON
	if ((features & ANDROID_CPU_ARM_FEATURE_NEON) == 0) {
		LOGE("CPU doesn't support NEON !", -1);
		return;
	}

	filter(&info, pixels);

	AndroidBitmap_unlockPixels(env, bitmap);

#else
	LOGE("Program not compiled with ARMv7 support !", -1);
	return;
#endif
	return;
*/
}
