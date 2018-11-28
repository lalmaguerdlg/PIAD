#pragma once
#define _USE_MATH_DEFINES
#include "Duck.h"
#include <functional>
#include <algorithm>
#include <numeric>
#include <math.h>
namespace duck
{

	std::array<std::wstring, (int)Filter::_LastEnumElement> FilterNames {
		L"Escala de gris (Promedio)",
		L"Escala de gris (Luminosity)",
		L"Escala de gris (Luminance)",
		L"Sepia",
		L"Suma",
		L"Resta",

		L"Desenfoque",
		L"Afilado",
		L"Desenfoque Gaussiano",
		L"Media ponderada",
		L"Substraccion",
		L"Sobel Horizontal",
		L"Sobel Vertical",
		L"Laplaciano",
		L"Mediana",

		L"Equalizacion simple",
		L"Equalizacion uniforme",
		L"Equalizacion exponencial",
		L"Invertir",
		L"Binario",
	};

	uchar clamp(int val, int min, int max) {
		return val <= min ? min : val >= max ? max : val;
	}

	Image::Image(uint width, uint height) : Mat{ width, height } {
		_data.resize(width * height);
	}
	// use Size instead...?
	inline void Image::resize(uint width, uint height) {
		// Use setters?
		_width = width;
		_height = height;
		_data.resize(width * height);
	}

	inline Image::DataVector& Image::dataVector() {
		return _data;
	}
	inline const Image::DataVector& Image::dataVector() const {
		return _data;
	}

	Kernel::Kernel(uint width, uint height, DataVector bytes, uint weight) :
		_weight{ !weight ? std::accumulate(bytes.begin(), bytes.end(), 0) : weight },
		Mat{ width, height, bytes } {
	}

	uint Kernel::weight() const {
		return _weight;
	}

	// Local filters...
	void convolve(const Kernel& kernel, const Image& in, Image& out) {
		// Storing variables to avoid calling functions later...
		const int imageWidth = in.width();
		const int imageHeight = in.height();
		const UCharPixelBGR* inData = (UCharPixelBGR*)in.rawBegin();
		out.resize(imageWidth, imageHeight);
		UCharPixelBGR* outData = (UCharPixelBGR*)out.rawBegin();
		const int* kernelData = (int*)kernel.rawBegin();
		const int kernelWeight = kernel.weight();
		const int minImageColumn = kernel.width() / 2;
		const int minImageRow = kernel.height() / 2;
		const int maxImageColumn = in.width() - minImageColumn;
		const int maxImageRow = in.height() - minImageRow;
		// Operation begins...
		for (int imageRow = minImageRow; imageRow < maxImageRow; ++imageRow) {
			for (int imageColumn = minImageColumn; imageColumn < maxImageColumn; ++imageColumn) {
				static IntPixelBGR accumulate;
				accumulate = {};
				static int kernelIndex;
				static int kernelRow;
				kernelIndex = 0;
				for (kernelRow = -minImageRow; kernelRow <= minImageRow; ++kernelRow) {
					static int kernelColumn;
					for (kernelColumn = -minImageColumn; kernelColumn <= minImageColumn; ++kernelColumn) {
						static const UCharPixelBGR* readPixel;
						static const int* kernelValue;
						readPixel = &inData[((imageRow + kernelRow) * imageWidth) + (imageColumn + kernelColumn)];
						kernelValue = &kernelData[kernelIndex];
						accumulate.r += readPixel->r * *kernelValue;
						accumulate.g += readPixel->g * *kernelValue;
						accumulate.b += readPixel->b * *kernelValue;
						++kernelIndex;
					}
				}
				static UCharPixelBGR* writePixel;
				writePixel = &outData[(imageRow * imageWidth) + imageColumn];
				writePixel->r = clamp(accumulate.r / kernelWeight, 0, 255);
				writePixel->g = clamp(accumulate.g / kernelWeight, 0, 255);
				writePixel->b = clamp(accumulate.b / kernelWeight, 0, 255);
			}
		}
	}

	void convolveMiddle(const Image& in, Image& out) {
		// Storing variables to avoid calling functions later...
		const int imageWidth = in.width();
		const int imageHeight = in.height();
		const UCharPixelBGR* inData = (UCharPixelBGR*)in.rawBegin();
		out.resize(imageWidth, imageHeight);
		UCharPixelBGR* outData = (UCharPixelBGR*)out.rawBegin();
		//const int* kernelData = (int*)kernel.rawBegin();
		//const int kernelWeight = kernel.weight();
		const int minImageColumn = 1;
		const int minImageRow = 1;
		const int maxImageColumn = in.width() - minImageColumn;
		const int maxImageRow = in.height() - minImageRow;
		// Operation begins...
		for (int imageRow = minImageRow; imageRow < maxImageRow; ++imageRow) {
			for (int imageColumn = minImageColumn; imageColumn < maxImageColumn; ++imageColumn) {
				static std::array<uchar, 9> accumulateRed;
				static std::array<uchar, 9> accumulateGreen;
				static std::array<uchar, 9> accumulateBlue;
				//accumulate = {};
				static int kernelIndex;
				static int kernelRow;
				kernelIndex = 0;
				for (kernelRow = -minImageRow; kernelRow <= minImageRow; ++kernelRow) {
					static int kernelColumn;
					for (kernelColumn = -minImageColumn; kernelColumn <= minImageColumn; ++kernelColumn) {
						static const UCharPixelBGR* readPixel;
						static const int* kernelValue;
						readPixel = &inData[((imageRow + kernelRow) * imageWidth) + (imageColumn + kernelColumn)];
						//kernelValue = &kernelData[kernelIndex];
						//accumulate.r += readPixel->r * *kernelValue;
						//accumulate.g += readPixel->g * *kernelValue;
						//accumulate.b += readPixel->b * *kernelValue;
						accumulateRed[kernelIndex] = readPixel->r;
						accumulateGreen[kernelIndex] = readPixel->g;
						accumulateBlue[kernelIndex] = readPixel->b;
						++kernelIndex;
					}
				}
				static UCharPixelBGR* writePixel;
				writePixel = &outData[(imageRow * imageWidth) + imageColumn];
				std::sort(std::begin(accumulateRed), std::end(accumulateRed), std::greater<uchar>());
				std::sort(std::begin(accumulateGreen), std::end(accumulateGreen), std::greater<uchar>());
				std::sort(std::begin(accumulateBlue), std::end(accumulateBlue), std::greater<uchar>());
				writePixel->r = accumulateRed[4];
				writePixel->g = accumulateGreen[4];
				writePixel->b = accumulateBlue[4];
			}
		}
	}

	Kernel generateGaussian(double sigma) {
		auto k = Mat<double>::DataVector(9);
		double sum = 0.0;
		for (int y = -1; y <= 1; y++) {
			for (int x = -1; x <= 1; x++) {
				static int index;
				index = ((y + 1) * 3) + (x + 1);
				k[index] = exp(-(y*y + x * x) / (2.0 * sigma*sigma)) / (2.0 * M_PI*sigma*sigma);
			}
		}
		sum = std::accumulate(std::begin(k), std::end(k), 0.0);
		std::transform(std::begin(k), std::end(k), std::begin(k), std::bind2nd(std::divides<double>(), sum));
		std::transform(std::begin(k), std::end(k), std::begin(k), std::bind1st(std::multiplies<double>(), 1000.0));
		return Kernel{ 3, 3, std::vector<int>(std::begin(k), std::end(k)), 1000 };
	}

	
	// Point filters...
	void toGrayScaleAverage(Image& out) {
		for (UCharPixelBGR& pixel : out.dataVector()) {
			int val = (pixel.r + pixel.g + pixel.b) / 3;
			pixel.r = val;
			pixel.g = val;
			pixel.b = val;
		}
	}

	void toGrayScaleLuminosity(Image& out) {
		for (UCharPixelBGR& pixel : out.dataVector()) {
			auto range = { pixel.r, pixel.g, pixel.b };
			int val = (std::max(range) + std::min(range)) / 2;
			pixel.r = val;
			pixel.g = val;
			pixel.b = val;
		}
	}

	void toGrayScaleLuminance(Image& out) {
		for (UCharPixelBGR& pixel : out.dataVector()) {
			int val = pixel.r * 30 / 100;
			val += pixel.g * 59 / 100;
			val += pixel.b * 11 / 100;
			pixel.r = val;
			pixel.g = val;
			pixel.b = val;
		}
	}

	void toSepia(Image& out) {
		for (UCharPixelBGR& pixel : out.dataVector()) {
			pixel.r = clamp((pixel.r * 393 / 1000) + (pixel.g * 769 / 1000) + (pixel.b * 189 / 1000), 0, 255);
			pixel.g = clamp((pixel.r * 349 / 1000) + (pixel.g * 686 / 1000) + (pixel.b * 168 / 1000), 0, 255);
			pixel.b = clamp((pixel.r * 272 / 1000) + (pixel.g * 534 / 1000) + (pixel.b * 131 / 1000), 0, 255);
		}
	}

	void add(Image& out, uchar value) {
		for (UCharPixelBGR& pixel : out.dataVector()) {
			pixel.r = clamp(pixel.r + value, 0, 255);
			pixel.g = clamp(pixel.g + value, 0, 255);
			pixel.b = clamp(pixel.b + value, 0, 255);
		}
	}

	void substract(Image& out, uchar value) {
		for (UCharPixelBGR& pixel : out.dataVector()) {
			pixel.r = clamp(pixel.r - value, 0, 255);
			pixel.g = clamp(pixel.g - value, 0, 255);
			pixel.b = clamp(pixel.b - value, 0, 255);
		}
	}
	
	void calculateToneInfo(HistogramFrequency& hf, float totalPixels) {
		
		auto firstTone = hf.toneMap.begin();
		firstTone->second.probability = firstTone->second.frequency / totalPixels;
		firstTone->second.cdf = firstTone->second.frequency;
		firstTone->second.probabilityCDF = firstTone->second.probability;

		int max = firstTone->second.frequency;
		std::map<int, ToneInfo>::iterator toneInfo;

		for (toneInfo = std::next(firstTone); toneInfo != hf.toneMap.end(); ++toneInfo) {
			toneInfo->second.probability = toneInfo->second.frequency / totalPixels;
			toneInfo->second.cdf = std::prev(toneInfo)->second.cdf + toneInfo->second.frequency;
			toneInfo->second.probabilityCDF = std::prev(toneInfo)->second.probabilityCDF + toneInfo->second.probability;
			if (toneInfo->second.frequency > max)
				max = toneInfo->second.frequency;
		}
		hf.maxFrequency = max;
	}
	
	// Global filters
	Histogram getHistogram(const Image& in) {
		Histogram histogram;
		float totalPixels = in.width() * in.height();
		// calculating frequency values
		for (const UCharPixelBGR& pixel : in.dataVector()) {
			++histogram.red.toneMap[pixel.r].frequency;
			++histogram.green.toneMap[pixel.g].frequency;
			++histogram.blue.toneMap[pixel.b].frequency;
		}

		calculateToneInfo(histogram.red, totalPixels);
		calculateToneInfo(histogram.green, totalPixels);
		calculateToneInfo(histogram.blue, totalPixels);
		return histogram;
	}

	
	void histogramSimpleEQ(Image& out) {
		auto histogram = getHistogram(out);
		for (auto& pixel : out.dataVector()) {
			pixel.r = 255 * histogram.red.toneMap[pixel.r].probabilityCDF;
			pixel.g = 255 * histogram.green.toneMap[pixel.g].probabilityCDF;
			pixel.b = 255 * histogram.blue.toneMap[pixel.b].probabilityCDF;
		}
	}

	void histogramUniformEQ(Image& out) {
		auto histogram = getHistogram(out);
		for (auto& pixel : out.dataVector()) {
			pixel.r = (std::prev(histogram.red.toneMap.end())->first - histogram.red.toneMap.begin()->first) * histogram.red.toneMap[pixel.r].probabilityCDF + histogram.red.toneMap.begin()->first;
			pixel.g = (std::prev(histogram.green.toneMap.end())->first - histogram.green.toneMap.begin()->first) * histogram.green.toneMap[pixel.g].probabilityCDF + histogram.green.toneMap.begin()->first;
			pixel.b = (std::prev(histogram.blue.toneMap.end())->first - histogram.blue.toneMap.begin()->first) * histogram.blue.toneMap[pixel.b].probabilityCDF + histogram.blue.toneMap.begin()->first;
		}
	}

	void histogramExponentialEQ(Image& out, double alpha) {
		auto histogram = getHistogram(out);
		for (auto& pixel : out.dataVector()) {
			pixel.r = histogram.red.toneMap.begin()->first - (1.0f / alpha) * std::log(1.0f - histogram.red.toneMap[pixel.r].probabilityCDF);
			pixel.g = histogram.green.toneMap.begin()->first - (1.0f / alpha) * std::log(1.0f - histogram.green.toneMap[pixel.g].probabilityCDF);
			pixel.b = histogram.blue.toneMap.begin()->first - (1.0f / alpha) * std::log(1.0f - histogram.blue.toneMap[pixel.b].probabilityCDF);
		}
	}
	

	//
	void makeHistogram(const Image& in, Image& out) {
		out.resize(256, 256);
		const auto histogram = getHistogram(in);

		const auto maxValue = std::max({ histogram.red.maxFrequency, histogram.green.maxFrequency, histogram.blue.maxFrequency });

		std::fill(out.dataVector().begin(), out.dataVector().end(), UCharPixelBGR{});

		const auto fillFrequencies = []
		(const HistogramFrequency& channel, std::vector<UCharPixelBGR>& pixels, float maxFrequency, const UCharPixelBGR& fillColor) {
			for (auto& val : channel.toneMap) {
				int row = (int)(val.second.frequency / maxFrequency * 255.0f);
				for (int i = 0; i <= row; i++) {
					pixels[(i * 256) + val.first].r += fillColor.r;
					pixels[(i * 256) + val.first].g += fillColor.g;
					pixels[(i * 256) + val.first].b += fillColor.b;
				}

			}
		};
		fillFrequencies(histogram.red, out.dataVector(), maxValue, UCharPixelBGR{ 0, 0, 255 });
		fillFrequencies(histogram.green, out.dataVector(), maxValue, UCharPixelBGR{ 0, 255, 0 });
		fillFrequencies(histogram.blue, out.dataVector(), maxValue, UCharPixelBGR{ 255, 0, 0 });
	}

	
	// Custom ...
	void invert(Image& out) {
		for (UCharPixelBGR& pixel : out.dataVector()) {
			pixel.r = (uchar)~pixel.r;
			pixel.g = (uchar)~pixel.g;
			pixel.b = (uchar)~pixel.b;
		}
	}

	void binary(Image& out) {
		toGrayScaleLuminance(out);
		histogramSimpleEQ(out);
		for (UCharPixelBGR& pixel : out.dataVector()) {
			pixel.r = pixel.g = pixel.b = pixel.r > 127 ? 255 : 0;
		}
	}
}