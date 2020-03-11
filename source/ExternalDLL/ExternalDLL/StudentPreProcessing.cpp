/*
* Copyright (c) 2015 DottedEye Designs, Alexander Hustinx, NeoTech Software, Rolf Smit - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
*/

#include "StudentPreProcessing.h"
#include "ImageIO.h"
#include "GrayscaleAlgorithm.h"
#include "ImageFactory.h"
#include "HereBeDragons.h"

void imageToMat(const IntensityImage& orignalImage, cv::Mat& destinationMat) {
	destinationMat.create(orignalImage.getHeight(), orignalImage.getWidth(), CV_8UC1);

	for (int x = 0; x < destinationMat.cols; x++) {
		for (int y = 0; y < destinationMat.rows; y++) {
			destinationMat.at<uchar>(y, x) = orignalImage.getPixel(x, y);
		}
	}
}

void matToImage(const cv::Mat& originalMat, IntensityImage& destinationImage) {
	if (originalMat.type() != CV_8UC1) {
		throw std::exception("OpenCV Mat image not an 8-bit single channel image!");
	}

	destinationImage.set(originalMat.cols, originalMat.rows);

	for (int x = 0; x < originalMat.cols; x++) {
		for (int y = 0; y < originalMat.rows; y++) {
			destinationImage.setPixel(x, y, originalMat.at<uchar>(y, x));
		}
	}
}

IntensityImage* StudentPreProcessing::stepToIntensityImage(const RGBImage& image) const {
	return nullptr;
}

IntensityImage* StudentPreProcessing::stepScaleImage(const IntensityImage& image) const {
	return nullptr;
}

IntensityImage* StudentPreProcessing::stepEdgeDetection(const IntensityImage& image) const {
	// return givenEdgeDetector(image);
	return cannyEdgeDetector(image);
	// return fastCanny(image);
}

IntensityImage* StudentPreProcessing::stepThresholding(const IntensityImage& image) const {
	cv::Mat convertedMat;
	imageToMat(image, convertedMat);
	int threshold = 100;
	cv::threshold(convertedMat, convertedMat, threshold, 255, cv::THRESH_BINARY_INV);
	IntensityImage* thresholdedImage = ImageFactory::newIntensityImage();
	matToImage(convertedMat, *thresholdedImage);
	return thresholdedImage;
}

// All implemented edge detectors are listed below.

IntensityImage* StudentPreProcessing::givenEdgeDetector(const IntensityImage& image) const {
	cv::Mat convertedMat;
	imageToMat(image, convertedMat);
	//cv::medianBlur(*image, *image, 3);
	cv::Mat FilteredImage;
	filter2D(convertedMat, FilteredImage, CV_8U, givenLaplacianOperator, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
	IntensityImage* edgeDetectedImage = ImageFactory::newIntensityImage();
	matToImage(FilteredImage, *edgeDetectedImage);
	return edgeDetectedImage;
}

IntensityImage* StudentPreProcessing::cannyEdgeDetector(const IntensityImage& image) const {
	int filterWidth = 3;
	int filterHeight = 3;
	float standardDeviation = 2;
	cv::Mat convertedMat;
	imageToMat(image, convertedMat);
	// http://dev.theomader.com/gaussian-kernel-calculator/
	cv::GaussianBlur(convertedMat, convertedMat, cv::Size(filterWidth, filterHeight), standardDeviation, standardDeviation, cv::BORDER_DEFAULT);

	cv::Mat horizontalDetectedImage;
	//filter2D(convertedMat, horizontalDetectedImage, CV_8U, horizontalSobelOperator, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);		// Werkt soort van.
	Sobel(convertedMat, horizontalDetectedImage, CV_8U, 0, 1, 3);																		// Werkt ook soort van.
	//													x, y, maskSize

	cv::Mat verticalDetectedImage;
	//filter2D(convertedMat, verticalDetectedImage, CV_8U, verticalSobelOperator, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
	Sobel(convertedMat, verticalDetectedImage, CV_8U, 1, 0, 3);
	//												  x, y, maskSize

	cv::Mat combinedDetectedImage;
	float verticalWeight = 1;
	float horizontalWeight = 1;
	combinedDetectedImage = verticalDetectedImage * verticalWeight + horizontalDetectedImage * horizontalWeight;		// Combineert afbeeldingen.

	IntensityImage* edgeDetectedImage = ImageFactory::newIntensityImage();
	matToImage(combinedDetectedImage, *edgeDetectedImage);
	return edgeDetectedImage;
}

IntensityImage* StudentPreProcessing::fastCanny(const IntensityImage& image) const{
	cv::Mat orginalImage;
	imageToMat(image, orginalImage);
	cv::Mat imageGrey, edgeDetectedImage;
	edgeDetectedImage.create(orginalImage.size(), orginalImage.type());

	int edgeThresh = 1;
	int lowThreshold = 100;
	int const max_lowThreshold = 100;
	int ratio = 3;
	int kernel_size = 3;

	cv::cvtColor(orginalImage, imageGrey, CV_BGR2GRAY);
	blur(imageGrey, edgeDetectedImage, cv::Size(3, 3));
	cv::Canny(edgeDetectedImage, edgeDetectedImage, lowThreshold, lowThreshold * ratio, kernel_size);
	IntensityImage* finishedImage = ImageFactory::newIntensityImage();
	matToImage(edgeDetectedImage, *finishedImage);
	cv::imshow("detected", edgeDetectedImage);
	return finishedImage;
}
