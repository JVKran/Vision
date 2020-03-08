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
	//return givenEdgeDetector(image);
	return cannyEdgeDetector(image);
}

IntensityImage* StudentPreProcessing::stepThresholding(const IntensityImage& image) const {
	cv::Mat convertedMat;
	imageToMat(image, convertedMat);
	int threshold = 254;
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
	cv::Mat convertedMat;
	imageToMat(image, convertedMat);
	cv::GaussianBlur(convertedMat, convertedMat, cv::Size(filterWidth, filterHeight), 0, 0, cv::BORDER_DEFAULT);

	cv::Mat horizontalDetectedImage;
	filter2D(convertedMat, horizontalDetectedImage, CV_8U, horizontalSobelOperator, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

	cv::Mat verticalDetectedImage;
	filter2D(convertedMat, verticalDetectedImage, CV_8U, verticalSobelOperator, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

	cv::Mat combinedDetectedImage;
	float alpha = 1;
	float beta = 1;
	addWeighted(verticalDetectedImage, alpha, horizontalDetectedImage, beta, 0.0, combinedDetectedImage);

	IntensityImage* edgeDetectedImage = ImageFactory::newIntensityImage();
	matToImage(combinedDetectedImage, *edgeDetectedImage);
	return edgeDetectedImage;
}
