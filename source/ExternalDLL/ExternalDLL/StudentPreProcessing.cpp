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

/* Convert IntensityImage to cv::Mat
   This function converts the first parameter (an IntensityImage) to the type
   of the second parameter (a cv::Mat).
*/
void imageToMat(const IntensityImage& orignalImage, cv::Mat& destinationMat) {
	destinationMat.create(orignalImage.getHeight(), orignalImage.getWidth(), CV_8UC1);

	for (int x = 0; x < destinationMat.cols; x++) {
		for (int y = 0; y < destinationMat.rows; y++) {
			destinationMat.at<uchar>(y, x) = orignalImage.getPixel(x, y);
		}
	}
}

/* Convert cv::Mat to IntensityImage
   This function converts the first parameter (a cv::Mat) to the type
   of the second parameter (an IntensityImage).
*/
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
	// Remember to change threshold in headerfile when another method is chosen!

	// return laplacianOperator(image);
	// return cannyOperator(image);
	return sobelOperator(image);
}

/* Thresholding
   This function applies the thresholding to the given parameter.
*/
IntensityImage* StudentPreProcessing::stepThresholding(const IntensityImage& image) const {
	cv::Mat convertedMat;
	imageToMat(image, convertedMat);
	cv::threshold(convertedMat, convertedMat, threshold, 255, cv::THRESH_BINARY_INV);
	IntensityImage* thresholdedImage = ImageFactory::newIntensityImage();
	matToImage(convertedMat, *thresholdedImage);
	return thresholdedImage;
}

/* Laplacian Operator
   This function implements the Laplacian Operator by applying the large Laplacian Operator.
   This image is then converted to an IntensityImage after which it is returned.
*/
IntensityImage* StudentPreProcessing::laplacianOperator(const IntensityImage& image) const {
	cv::Mat convertedMat;
	imageToMat(image, convertedMat);
	cv::Mat FilteredImage;

	filter2D(convertedMat, FilteredImage, CV_8U, laplacianOperatorLarge, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);
	IntensityImage* edgeDetectedImage = ImageFactory::newIntensityImage();

	matToImage(FilteredImage, *edgeDetectedImage);
	return edgeDetectedImage;
}

/* Sobel Operator
   This function implements the Sobel Operator by applying the small horizontal en vertical
   sobel operators. Lastly, the two filtered images are combined.
   This combined image is then converted to an IntensityImage after which it is returned.
*/
IntensityImage* StudentPreProcessing::sobelOperator(const IntensityImage& image) const {
	cv::Mat convertedMat;
	imageToMat(image, convertedMat);

	cv::Mat horizontalDetectedImage;
	filter2D(convertedMat, horizontalDetectedImage, CV_8U, horizontalSobelOperatorSmall, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

	cv::Mat verticalDetectedImage;
	filter2D(convertedMat, verticalDetectedImage, CV_8U, verticalSobelOperatorSmall, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

	cv::Mat combinedDetectedImage;
	combinedDetectedImage = verticalDetectedImage + horizontalDetectedImage;

	IntensityImage* edgeDetectedImage = ImageFactory::newIntensityImage();
	matToImage(combinedDetectedImage, *edgeDetectedImage);
	return edgeDetectedImage;
}

/* Canny Operator
   This function implements the Canny Operator by first applying a Gaussian Mask after which the
   small horizontal and vertical operator are applied. Lastly, the two filtered images are combined.
   This combined image is then converted to an IntensityImage after which it is returned.
*/
IntensityImage* StudentPreProcessing::cannyOperator(const IntensityImage& image) const {
	int filterWidth = 5;
	int filterHeight = 5;
	float standardDeviation = 0.7;
	cv::Mat convertedMat;
	imageToMat(image, convertedMat);
	filter2D(convertedMat, convertedMat, CV_8U, gaussianOperator, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

	cv::Mat horizontalDetectedImage;
	filter2D(convertedMat, horizontalDetectedImage, CV_8U, horizontalSobelOperatorSmall, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

	cv::Mat verticalDetectedImage; 
	filter2D(convertedMat, verticalDetectedImage, CV_8U, verticalSobelOperatorSmall, cv::Point(-1, -1), 0, cv::BORDER_DEFAULT);

	cv::Mat combinedDetectedImage;
	float verticalWeight = 1;
	float horizontalWeight = 1;
	combinedDetectedImage = verticalDetectedImage * verticalWeight + horizontalDetectedImage * horizontalWeight;

	IntensityImage* edgeDetectedImage = ImageFactory::newIntensityImage();
	matToImage(combinedDetectedImage, *edgeDetectedImage);
	return edgeDetectedImage;
}
