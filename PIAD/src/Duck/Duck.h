#pragma once
#include <vector>
#include <map>
#include <array>

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>



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
		static Kernel Identity() {
			return { 3, 3, {
					0, 0, 0,
					0, 1, 0,
					0, 0, 0
				} };
		}
	};

	enum class Filter {
		None = -1,
		PGrayScaleAverage = 0,
		PGrayScaleLuminosity,
		PGrayScaleLuminance,
		PSepia,
		PAddition,
		PSubstraction,

		LBlur,
		LSharp,
		LGaussian,
		LWeightedAverage,
		LSubstraction,
		LSobelNorth,
		LSobelSouth,
		LSobelEast,
		LSobelWest,
		LSobel,
		LLaplacian,
		LMenosLaplacian,
		LMedian,

		GHistogramSimpleEQ,
		GHistogramUniformEQ,
		GHistogramExponentialEQ,
		GHog,

		CInverse,
		CBinary,
		_LastEnumElement,
	};

	/*enum class PointFilter {
		GrayScaleAverage = 0,
		GrayScaleLuminosity,
		GrayScaleLuminance,
		Sepia,
		Addition,
		Substraction,
	};

	enum class LocalFilter {

	};

	enum class GlobalFilter {

	};*/
	extern std::array<std::wstring, (int)Filter::_LastEnumElement> FilterNames;

	// Helpers...
	uchar clamp(int val, int min, int max);
	
	// Local filters...
	void convolve(const Kernel& kernel, const Image& in, Image& out);
	void convolveMiddle(const Image& in, Image& out);
	Kernel generateGaussian(double sigma);

	void toSobel(const Image & in, Image & out);
	


	// Point filters...
	void toGrayScaleAverage(Image& out);
	void toGrayScaleLuminosity(Image& out);
	void toGrayScaleLuminance(Image& out);
	void toSepia(Image& out);
	void add(Image& out, uchar value);
	void substract(Image& out, uchar value);
	
	
	// Histogram
	struct ToneInfo {
		float frequency = 0.0f;
		float cdf = 0.0f;
		float probability = 0.0f;
		float probabilityCDF = 0.0f;
	};

	struct HistogramFrequency {
		std::map<int, ToneInfo> toneMap;
		int maxFrequency;
	};
	//using Histogram = std::array<HistogramChannel, 3>;

	struct Histogram {
		HistogramFrequency red;
		HistogramFrequency green;
		HistogramFrequency blue;
	};
	
	Histogram getHistogram(const Image& in);

	//histogram makers...
	using HistogramPixels = std::array<UCharPixelBGR, 256 * 256>;

	// Global filters
	
	void histogramSimpleEQ(Image& out);
	void histogramUniformEQ(Image& out);
	void histogramExponentialEQ(Image & out, double alpha);
	

	// makes a 256 * 128 pixel image data 
	void makeHistogram(const Image& in, Image& out);
	
	
	// custom filters...
	void invert(Image& out);
	void binary(Image& out);

	void Image2Mat(Image & in, cv::Mat & out);
	void Mat2Image(const cv::Mat& in, Image& out);

	void HOG(Image & out, const cv::HOGDescriptor & hog, const std::vector<cv::Scalar>& colorList);

}