#pragma once
#include <vector>
#include <map>
#include <array>
namespace duck
{
	using uchar = std::uint8_t;
	using uint = std::uint32_t;
	using ByteVector = std::vector<uchar>;
	
	template<typename T> struct PixelBGR {
		T b = 0, g = 0, r = 0, offset = 0;
	};
	
	using UCharPixelBGR = PixelBGR<uchar>;
	using IntPixelBGR = PixelBGR<int>;
	
	template<typename DataType> class Mat {
	public:
		using DataVector = std::vector<DataType>;
	protected:
		DataVector _data{};
		//ByteVector bytes{};
		uint _width;
		uint _height;
	public:
		Mat(uint width, uint height, DataVector data = {}) :
			_width{ width },
			_height{ height },
			_data{ data }
		{}
		inline const uint& width() const { return _width; }
		inline const uint& height() const { return _height; }
		inline void* rawBegin() { return (void*)&_data[0]; }
		inline const void* rawBegin() const { return (void*)&_data[0]; }
		inline void* rawEnd() { return (void*)&_data[_data.size()]; }
	};

	
	class Image : public Mat<UCharPixelBGR> {
	public:
		Image(uint width = 0, uint height = 0);
		// use Size instead...?
		inline void resize(uint width, uint height);
		DataVector& dataVector();
		const DataVector& dataVector() const;
	};

	class Kernel : public Mat<int> {
	private:
		uint _weight = 0;
	public:
		Kernel(uint width, uint height, DataVector bytes, uint weight = 0);
		uint weight() const;
	};

	// Helpers...
	uchar clamp(int val, int min, int max);
	
	// Local filters...
	void convolve(const Kernel& kernel, const Image& in, Image& out);
	void convolveMiddle(const Image& in, Image& out);
	Kernel generateGaussian(double sigma);
	
	// Point filters...
	void toGrayScaleAverage(Image& out);
	void toGrayScaleLuminosity(Image& out);
	void toGrayScaleLuminance(Image& out);
	void toSepia(Image& out);
	void add(Image& out, uchar value);
	void substract(Image& out, uchar value);
	
	/*
	// Global filters
	struct ToneInfo {
		float frequency = 0.0f;
		float cdf = 0.0f;
		float probability = 0.0f;
		float probabilityCDF = 0.0f;
	};
	using HistogramChannel = std::map<int, ToneInfo>;
	using Histogram = std::array<HistogramChannel, 3>;
	Histogram getHistogram(const Image& in);
	void histogramSimpleEQ(Image& out);
	void histogramUniformEQ(Image& out);
	void histogramExponentialEQ(Image& out);
	
	//histogram makers...
	using HistogramPixels = std::array<UCharPixelBGR, 256 * 128>;
	struct HistogramImageData {
		HistogramPixels redChannel, greenChannel, blueChannel;
	};
	
	// makes a 256 * 128 pixel image data 
	void makeHistogram(const Image& in, HistogramImageData& out);
	
	// custom filters...
	void invert(Image& out);
	void binary(Image& out);*/
}