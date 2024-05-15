#include "Noise.h"

double Noise::smoothNoise(double zoom, double x1, double y1, double z1) {
	//ȡС������
	double fractX = x1 - (int)x1;
	double fractY = y1 - (int)y1;
	double fractZ = z1 - (int)z1;

	//wrapֵ����
	double x2 = x1 - 1; if (x2 < 0) x2 = (round(noiseHeight / zoom)) - 1;
	double y2 = y1 - 1; if (y2 < 0) y2 = (round(noiseWidth / zoom)) - 1;
	double z2 = z1 - 1; if (z2 < 0) z2 = (round(noiseDepth / zoom)) - 1;

	//��ֵƽ������
	double value = 0.0;
	value += fractX * fractY * fractZ * noiseMat[(int)x1][(int)y1][(int)z1];
	value += (1.0 - fractX) * fractY * fractZ * noiseMat[(int)x2][(int)y1][(int)z1];
	value += fractX * (1.0 - fractY) * fractZ * noiseMat[(int)x1][(int)y2][(int)z1];
	value += (1.0 - fractX) * (1.0 - fractY) * fractZ * noiseMat[(int)x2][(int)y2][(int)z1];

	value += fractX * fractY * (1.0 - fractZ) * noiseMat[(int)x1][(int)y1][(int)z2];
	value += (1.0 - fractX) * fractY * (1.0 - fractZ) * noiseMat[(int)x2][(int)y1][(int)z2];
	value += fractX * (1.0 - fractY) * (1.0 - fractZ) * noiseMat[(int)x1][(int)y2][(int)z2];
	value += (1.0 - fractX) * (1.0 - fractY) * (1.0 - fractZ) * noiseMat[(int)x2][(int)y2][(int)z2];

	return value;
}

double Noise::turbulence(double x, double y, double z, double maxZoom) {
	double sum = 0.0, zoom = maxZoom;

	sum = (sin((1.0 / 512.0) * (8 * PI) * (x + z - 4 * y)) + 1) * 8.0;

	while (zoom >= 0.9) {
		sum = sum + smoothNoise(zoom, x / zoom, y / zoom, z / zoom) * zoom;
		zoom = zoom / 2.0;
	}

	sum = 128.0 * sum / maxZoom;
	return sum;
}

void Noise::fillDataArray(GLubyte data[]) {
	double maxZoom = 32.0;
	for (int i = 0; i < noiseHeight; i++) {
		for (int j = 0; j < noiseWidth; j++) {
			for (int k = 0; k < noiseDepth; k++) {
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 0] = (GLubyte)turbulence(i, j, k, maxZoom);
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 1] = (GLubyte)turbulence(i, j, k, maxZoom);
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 2] = (GLubyte)turbulence(i, j, k, maxZoom);
				data[i * (noiseWidth * noiseHeight * 4) + j * (noiseHeight * 4) + k * 4 + 3] = (GLubyte)255;
			}
		}
	}
}

void  Noise::generateNoise() {
	for (int x = 0; x < noiseHeight; x++) {
		for (int y = 0; y < noiseWidth; y++) {
			for (int z = 0; z < noiseDepth; z++) {
				noiseMat[x][y][z] = (double)rand() / (RAND_MAX + 1.0);
			}
		}
	}
}