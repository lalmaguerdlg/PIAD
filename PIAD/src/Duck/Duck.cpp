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
		L"Substraccion de la media",
		L"Sobel Norte",
		L"Sobel Sur",
		L"Sobel Este",
		L"Sobel Oeste",
		L"Sobel",
		L"Laplaciano",
		L"Menos laplaciano",
		L"Mediana",

		L"Equalizacion simple",
		L"Equalizacion uniforme",
		L"Equalizacion exponencial",
		L"Deteccion de personas (HOG)",

		L"Invertir",
		L"Binario"
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


	void toSobel(const Image& in, Image& out) {
		Kernel filter_x {3, 3, 
			{ -1, 0,  1,
			-2,  0, 2,
			 -1, 0,  1}, 1 };
		Kernel filter_y {3, 3, 
			{ -1, -2,  -1,
			0,  0, 0,
			 1, 2,  1}, 1 };

		Image C = in;
		Image F = in;
		convolve(filter_x, in, C);
		convolve(filter_y, in, F);

		UCharPixelBGR* colP = (UCharPixelBGR*)C.rawBegin();
		UCharPixelBGR* rowP = (UCharPixelBGR*)F.rawBegin();
		
		UCharPixelBGR sum{};
		
		for (auto& pixel : out.dataVector()) {
			sum.r = sqrt( (colP->r * colP->r) + (rowP->r * rowP->r) );
			sum.g = sqrt( (colP->g * colP->g) + (rowP->g * rowP->g) );
			sum.b = sqrt( (colP->b * colP->b) + (rowP->b * rowP->b) );
			pixel.r = clamp(sum.r, 0, 255);
			pixel.g = clamp(sum.g, 0, 255);
			pixel.b = clamp(sum.b, 0, 255);
			colP++;
			rowP++;
		}
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
		std::unordered_map<int, ToneInfo>::iterator toneInfo;

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


	void Image2Mat(Image& in, cv::Mat& out) {
		out = cv::Mat((int)in.height(), (int)in.width(), CV_8UC4, in.rawBegin());
		//cv::cvtColor(out, out, cv::COLOR_RGBA2BGRA);
		cv::flip(out, out, 0);
	}

	void Mat2Image(const cv::Mat& in, Image& out) {
		cv::Mat dst = cv::Mat(in.rows, in.cols, CV_8UC4);
		cv::flip(in, dst, 0);

		out.resize(in.cols, in.rows);
		out.dataVector().assign((duck::UCharPixelBGR*)dst.datastart, (duck::UCharPixelBGR*)dst.dataend);
	}

	void HOG(Image& out, const cv::HOGDescriptor& hog, const std::vector<cv::Scalar>& colorList) {

		cv::Mat img;

		Image2Mat(out, img);

		const int escala = 1;

		cv::Mat aux(img.rows / escala, img.cols / escala, CV_8U); //generamos dos imagenes reducidas en un tercio por lado o sea
		cv::Mat aux2(img.rows / escala, img.cols / escala, CV_8U);//un noveno de tamaño para acelerar su proceso
		cv::resize(img, aux, aux.size(), 0, 0, cv::INTER_LINEAR);
		cvtColor(aux, aux2, CV_BGR2GRAY);
		if (!img.data)
			return;
		//generamos un arreglo de rectangulo para encuadrar a la racita
		std::vector<cv::Rect> found, found_filtered;
		//le agregamos estilo para medir el tiempo de proceso, importante esta tecnica para
		//medir el desempeño
		//le pedimos que haga el trabajo de detectar
		hog.detectMultiScale(aux2, found, 0, cv::Size(8, 8), cv::Size(32, 32), 1.05, 2);
		//tiempo total de proceso
		//imprimelo, sino pa'que
		//contador de los ciclos para la cantidad de hallazgos
		size_t i, j;
		//comienza el ciclo
		for (i = 0; i < found.size(); i++)
		{
			//genera al primer rectangulo de los hallados
			cv::Rect r = found[i];
			//checa si no se repiten lso rectangulos
			for (j = 0; j < found.size(); j++)
				if (j != i && (r & found[j]) == r)
					break;
			//si se acabo el arreglo lo mete al final del otro arreglo
			//este solo tendra rectangulos no repetidos
			if (j == found.size())
				found_filtered.push_back(r);
		}
		//de los hallados, a dibujar
		int found_count = found_filtered.size();
		for (i = 0; i < found_filtered.size(); i++)
		{
			//dibuja el primero de los filtrados
			cv::Rect r = found_filtered[i];
			//dibujemos el rectangulo un poco mas grande de lo normal
			//pa que la racita no quede mal encuadrada, el 3 es de la
			//reduccion que habiamos hecho, estamos compensando
			r.x *= escala;
			r.x += cvRound(r.width*0.1);
			r.width = cvRound(r.width*0.8*escala);
			r.y = r.y*escala;
			r.y += cvRound(r.height*0.07);
			r.height = cvRound(r.height*0.8*escala);
			int index = i % colorList.size();
			cv::rectangle(img, r.tl(), r.br(), colorList[i], 3);
		}

		cv::putText(img, std::to_string(found_count), cv::Point2f(20, 70), cv::FONT_HERSHEY_PLAIN, 5, cv::Scalar(255, 255, 255, 255), 3);

		Mat2Image(img, out);

	}

}