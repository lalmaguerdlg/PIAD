#pragma once
#define _USE_MATH_DEFINES
#include "Duck.h"
#include <functional>
#include <algorithm>
#include <numeric>
#include <math.h>
namespace duck
{
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
	
	/*
	// Global filters
	Histogram getHistogram(const Image& in) {
		Histogram histogram;
		float totalPixels = in.width() * in.height();
		// calculating frequency values
		for (const UCharPixelBGR& pixel : in.dataVector()) {
			++histogram[0][pixel.r].frequency;
			++histogram[1][pixel.g].frequency;
			++histogram[2][pixel.b].frequency;
		}
		//int i = 0;
		for (auto& histogramChannel : histogram) {
			histogramChannel.begin()->second.probability = histogramChannel.begin()->second.frequency / totalPixels;
			histogramChannel.begin()->second.cdf = histogramChannel.begin()->second.frequency;
			histogramChannel.begin()->second.probabilityCDF = histogramChannel.begin()->second.probability;
			for (auto& toneInfo = std::next(histogramChannel.begin()); toneInfo != histogramChannel.end(); ++toneInfo) {
				toneInfo->second.probability = toneInfo->second.frequency / totalPixels;
				toneInfo->second.cdf = std::prev(toneInfo)->second.cdf + toneInfo->second.frequency;
				toneInfo->second.probabilityCDF = std::prev(toneInfo)->second.probabilityCDF + toneInfo->second.probability;
			}
			//++i;
		}
		return histogram;
	}

	void histogramSimpleEQ(Image& out) {
		auto histogram = getHistogram(out);
		for (auto& pixel : out.dataVector()) {
			pixel.r = 255 * histogram[0][pixel.r].probabilityCDF;
			pixel.g = 255 * histogram[1][pixel.g].probabilityCDF;
			pixel.b = 255 * histogram[2][pixel.b].probabilityCDF;
		}
	}

	void histogramUniformEQ(Image& out) {
		auto histogram = getHistogram(out);
		for (auto& pixel : out.dataVector()) {
			pixel.r = (std::prev(histogram[0].end())->first - histogram[0].begin()->first) * histogram[0][pixel.r].probabilityCDF + histogram[0].begin()->first;
			pixel.g = (std::prev(histogram[1].end())->first - histogram[1].begin()->first) * histogram[1][pixel.g].probabilityCDF + histogram[1].begin()->first;
			pixel.b = (std::prev(histogram[2].end())->first - histogram[2].begin()->first) * histogram[2][pixel.b].probabilityCDF + histogram[2].begin()->first;
		}
	}

	void histogramExponentialEQ(Image& out) {
		auto histogram = getHistogram(out);
		for (auto& pixel : out.dataVector()) {
			pixel.r = histogram[0].begin()->first - (1.0f / 200.0f) * std::log(1.0f - histogram[0][pixel.r].probabilityCDF);
			pixel.g = histogram[1].begin()->first - (1.0f / 200.0f) * std::log(1.0f - histogram[1][pixel.g].probabilityCDF);
			pixel.b = histogram[2].begin()->first - (1.0f / 200.0f) * std::log(1.0f - histogram[2][pixel.b].probabilityCDF);
		}
	}
	//
	void makeHistogram(const Image& in, HistogramImageData& out) {
		const auto histogram = getHistogram(in);
		const auto maxFreq = [](const std::pair<int, ToneInfo>& p1, const std::pair<int, ToneInfo>& p2) {
			return p1.second.frequency < p2.second.frequency;
		};
		const auto maxRedFrequency = std::max_element(histogram[0].begin(), histogram[0].end(), maxFreq)->second.frequency;
		const auto maxGreenFrequency = std::max_element(histogram[1].begin(), histogram[1].end(), maxFreq)->second.frequency;
		const auto maxBlueFrequency = std::max_element(histogram[2].begin(), histogram[2].end(), maxFreq)->second.frequency;
		std::fill(out.redChannel.begin(), out.redChannel.end(), UCharPixelBGR{});
		std::fill(out.greenChannel.begin(), out.greenChannel.end(), UCharPixelBGR{});
		std::fill(out.blueChannel.begin(), out.blueChannel.end(), UCharPixelBGR{});
		const auto fillFrequencies = []
		(const HistogramChannel& channel, HistogramPixels& pixels, float maxFrequency, const UCharPixelBGR& fillColor) {
			for (auto& val : channel) {
				int row = (int)(val.second.frequency / maxFrequency * 127.0f);
				for (int i = 0; i <= row; i++) {
					pixels[(i * 256) + val.first] = fillColor;
				}
			}
		};
		fillFrequencies(histogram[0], out.redChannel, maxRedFrequency, UCharPixelBGR{ 64, 64, 255 });
		fillFrequencies(histogram[1], out.greenChannel, maxGreenFrequency, UCharPixelBGR{ 64, 255, 64 });
		fillFrequencies(histogram[2], out.blueChannel, maxBlueFrequency, UCharPixelBGR{ 255, 64, 66 });
	}

	//
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
	}*/
}