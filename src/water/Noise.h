#pragma once
#include <GLFW/glfw3.h>
#include <cmath>

#define PI 3.1415926535

const int noiseWidth = 256;
const int noiseHeight = 256;
const int noiseDepth = 256;

class Noise
{
	private:
		int mWidth{ noiseWidth };
		int mHeight{ noiseHeight };
		int mDeep{ noiseDepth };
		double noiseMat[noiseHeight][noiseWidth][noiseDepth];
	private:
		double smoothNoise(double zoom, double x1, double y1, double z1);
		double turbulence(double x, double y, double z, double maxZoom);

	public:
		int getWidth() { return mWidth; };
		int getHeight() { return mHeight; };
		int getDeep() { return mDeep; };
		void fillDataArray(GLubyte data[]);
		void generateNoise();
};

