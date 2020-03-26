/*
* Copyright (c) 2015 DottedEye Designs, Alexander Hustinx, NeoTech Software, Rolf Smit - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
*/

#pragma once
#include "PreProcessing.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/types_c.h"
#include "opencv2/highgui/highgui.hpp"

void imageToMat(const IntensityImage& orignalImage, cv::Mat& destinationMat);
void matToImage(const cv::Mat& originalMat, IntensityImage& destinationImage);

class StudentPreProcessing : public PreProcessing {
private:
	int threshold = 70;
	// 70 works for sobel and canny
	// 220 works for laplacian.


	cv::Mat laplacianOperatorLarge = (cv::Mat_<float>(9, 9) <<  0, 0, 0, 1, 1, 1, 0, 0, 0,
																0, 0, 0, 1, 1, 1, 0, 0, 0,
																0, 0, 0, 1, 1, 1, 0, 0, 0,
																1, 1, 1,-4,-4,-4, 1, 1, 1,
																1, 1, 1,-4,-4,-4, 1, 1, 1,
																1, 1, 1,-4,-4,-4, 1, 1, 1,
																0, 0, 0, 1, 1, 1, 0, 0, 0,
																0, 0, 0, 1, 1, 1, 0, 0, 0,
																0, 0, 0, 1, 1, 1, 0, 0, 0);

	cv::Mat verticalSobelOperatorLarge = (cv::Mat_<float>(9, 9) <<	-1, -1, -1, 0, 0, 0, 1, 1, 1,
																	-1, -1, -1, 0, 0, 0, 1, 1, 1,
																	-1, -1, -1, 0, 0, 0, 1, 1, 1,
																	-2, -2, -2, 0, 0, 0, 2, 2, 2,
																	-2, -2, -2, 0, 0, 0, 2, 2, 2,
																	-2, -2, -2, 0, 0, 0, 2, 2, 2,
																	-1, -1, -1, 0, 0, 0, 1, 1, 1,
																	-1, -1, -1, 0, 0, 0, 1, 1, 1,
																	-1, -1, -1, 0, 0, 0, 1, 1, 1);

	cv::Mat horizontalSobelOperatorLarge = (cv::Mat_<float>(6, 6) << 1, 1, 1, 2, 2, 2, 1, 1, 1,
																1, 1, 1, 2, 2, 2, 1, 1, 1,
																1, 1, 1, 2, 2, 2, 1, 1, 1,
																0, 0, 0, 0, 0, 0, 0, 0, 0,
																0, 0, 0, 0, 0, 0, 0, 0, 0, 
																0, 0, 0, 0, 0, 0, 0, 0, 0,
																-1, -1, -1, -2, -2, -2, -1, -1, -1,
																-1, -1, -1, -2, -2, -2, -1, -1, -1,
																-1, -1, -1, -2, -2, -2, -1, -1, -1);

	cv::Mat verticalSobelOperator = (cv::Mat_<float>(6, 6) <<   -1,-1, 0, 0, 1, 1,
																-1,-1, 0, 0, 1, 1,
																-2,-2, 0, 0, 2, 2,
																-2,-2, 0, 0, 2, 2,
																-1,-1, 0, 0, 1, 1,
																-1,-1, 0, 0, 1, 1);

	cv::Mat verticalSobelOperatorSmall = (cv::Mat_<float>(3, 3) <<	-1, 0, 1,
																	-2, 0, 2,
																	-1, 0, 1);

	cv::Mat horizontalSobelOperator = (cv::Mat_<float>(6, 6) << 1, 1, 2, 2, 1, 1,
																1, 1, 2, 2, 1, 1,
																0, 0, 0, 0, 0, 0,
																0, 0, 0, 0, 0, 0,
																-1,-1,-2,-2,-1,-1,
																-1,-1,-2,-2,-1,-1);

	cv::Mat horizontalSobelOperatorSmall = (cv::Mat_<float>(3, 3) << 1, 2, 1,
																	0, 0, 0,
																	-1,-2,-1);

	cv::Mat gaussianOperator = (cv::Mat_<float>(5, 5) <<	0.000252, 0.00352, 0.008344, 0.00352, 0.000252,
															0.00352, 0.049081, 0.11634, 0.049081, 0.00352,
															0.008344, 0.11634, 0.275768, 0.11634, 0.008344,
															0.00352, 0.049081, 0.11634, 0.049081, 0.00352,
															0.000252, 0.00352, 0.008344, 0.00352, 0.000252);


	// This function implements the laplacian operator by using the large laplacian operator above.
	IntensityImage * laplacianOperator(const IntensityImage& image) const;

	// This function implements the laplacian operator by using the small horizontal and vertical sobel operators.
	IntensityImage * cannyOperator(const IntensityImage& image) const;

	// This function implements the laplacian operator by using the small horizontal and vertical sobel operators.
	IntensityImage* sobelOperator(const IntensityImage& image) const;
public:
	IntensityImage * stepToIntensityImage(const RGBImage &image) const;
	IntensityImage * stepScaleImage(const IntensityImage &image) const;
	IntensityImage * stepEdgeDetection(const IntensityImage &image) const;
	IntensityImage * stepThresholding(const IntensityImage &image) const;
};
