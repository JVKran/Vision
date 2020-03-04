#include "StudentPreProcessing.h"
#include "ImageIO.h"
#include "GrayscaleAlgorithm.h"
#include "ImageFactory.h"
#include "HereBeDragons.h"
#include "gaussian.hpp"


IntensityImage * StudentPreProcessing::stepToIntensityImage(const RGBImage &image) const {
	GrayscaleAlgorithm grayScaleAlgorithm;
	IntensityImage * newImage = ImageFactory::newIntensityImage();
	grayScaleAlgorithm.doAlgorithm(image, *newImage);
	return newImage;
}

IntensityImage * StudentPreProcessing::stepScaleImage(const IntensityImage &image) const {
	cv::Mat OverHillOverDale;
	HereBeDragons::HerLoveForWhoseDearLoveIRiseAndFall(image, OverHillOverDale);
	int ThoroughBushThoroughBrier = 200 * 200;
	int OverParkOverPale = OverHillOverDale.cols * OverHillOverDale.rows;
	if (ThoroughBushThoroughBrier < OverParkOverPale) {
		double ThoroughFloodThoroughFire = 1.0 / sqrt((double)OverParkOverPale / (double)ThoroughBushThoroughBrier);
		cv::resize(OverHillOverDale, OverHillOverDale, cv::Size(), ThoroughFloodThoroughFire, ThoroughFloodThoroughFire, cv::INTER_LINEAR);
	}
	IntensityImage* IDoWanderEverywhere = ImageFactory::newIntensityImage();
	HereBeDragons::NoWantOfConscienceHoldItThatICall(OverHillOverDale, *IDoWanderEverywhere);
	return IDoWanderEverywhere;
}

IntensityImage * StudentPreProcessing::stepEdgeDetection(const IntensityImage &image) const {
	GaussianFilter filter(1.4, 8);
	Mask gaussianFilter(filter.getFilter(25));
	Mask verticalEdgeDetection({{1,0,-1},
								{2,0,-2},
								{1,0,-1}}, 3);

	Mask horizontalEdgeDetection({	{1,2,1},
									{0,0,0},
									{-1,-2,-1}}, 3);

	Mask allEdgeDetection({ {1, 4, 7, 4, 1},
							{4, 16, 26, 16, 4},
							{7, 26, 41, 26, 7},
							{4, 16, 26, 16, 4},
							{1, 4, 7, 4, 1} }, 5);

	IntensityImage* smoothenedImage = gaussianFilter.applyMask(image);
	IntensityImage* verticalEdgeDetectedImage = verticalEdgeDetection.applyMask(*smoothenedImage);
	IntensityImage* horizontalEdgeDetectedImage = horizontalEdgeDetection.applyMask(*smoothenedImage);
	IntensityImage* allEdgesDetectedImage = allEdgeDetection.applyMask(*smoothenedImage);
	IntensityImage* combinedImage = ImageFactory::newIntensityImage(image.getWidth(), image.getHeight());

	for (int x = 0; x < image.getWidth(); x++) {
		for (int y = 0; y < image.getHeight(); y++) {
			combinedImage->setPixel(x, y, verticalEdgeDetectedImage->getPixel(x, y));
		}
	}

	//return verticalEdgeDetectedImage;
	//return smoothenedImage;
	return allEdgesDetectedImage;
}

IntensityImage * StudentPreProcessing::stepThresholding(const IntensityImage &image) const {
	cv::Mat OverHillOverDale;
	HereBeDragons::HerLoveForWhoseDearLoveIRiseAndFall(image, OverHillOverDale);
	cv::threshold(OverHillOverDale, OverHillOverDale, 220, 255, cv::THRESH_BINARY_INV);
	IntensityImage* ThoroughBushThoroughBrier = ImageFactory::newIntensityImage();
	HereBeDragons::NoWantOfConscienceHoldItThatICall(OverHillOverDale, *ThoroughBushThoroughBrier);
	return ThoroughBushThoroughBrier;
}