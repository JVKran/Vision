/*
* Copyright (c) 2015 DottedEye Designs, Alexander Hustinx, NeoTech Software, Rolf Smit - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
*/

#include <iostream>			// std::cout
#include <chrono>			// Timing
#include <exception>
#include <vector>
#include <algorithm>
#include "StudentPreProcessing.h"
#include "ImageIO.h"		// Image load and save functionality
#include "HereBeDragons.h"
#include "ImageFactory.h"
#include "DLLExecution.h"


void drawFeatureDebugImage(IntensityImage &image, FeatureMap &features);
bool executeSteps(DLLExecution * executor, const bool student, std::vector<int> & studentEdgeTimings, std::vector<int> & defaultEdgeTimings);

int main(int argc, char * argv[]) {

	std::vector<int> studentTotalTimings;
	std::vector<int> studentEdgeDetectionTimings;

	std::vector<int> defaultTotalTimings;
	std::vector<int> defaultEdgeDetectionTimings;

	std::vector<int> accuracyDifference;


		ImageFactory::setImplementation(ImageFactory::DEFAULT);
		//ImageFactory::setImplementation(ImageFactory::STUDENT);


		ImageIO::debugFolder = "C:\\Users\\kraan\\source\\repos\\Vision\\testsets\\TestSet Images\\Set A";
		ImageIO::isInDebugMode = true; //If set to false the ImageIO class will skip any image save function calls

		for (uint_fast8_t i = 1; i < 6; i++) {

			RGBImage* input = ImageFactory::newRGBImage();
			
			if (!ImageIO::loadImage("C:\\Users\\kraan\\source\\repos\\Vision\\testsets\\Set A\\TestSet Images\\" + std::to_string(i) + ".png", *input)) {
				std::cout << "Image could not be loaded!" << std::endl;
				system("pause");
				return 0;
			}

			for (uint_fast8_t j = 0; j < 40; j++) {
				DLLExecution* executor = new DLLExecution(input);

				auto start = std::chrono::high_resolution_clock::now();

				if (executeSteps(executor, true, studentEdgeDetectionTimings, defaultEdgeDetectionTimings)) {
					std::cout << "Student face recognition successful!" << std::endl;
					std::cout << "Facial parameters: " << std::endl;
					for (int i = 0; i < 16; i++) {
						std::cout << (i + 1) << ": " << executor->facialParameters[i] << std::endl;
					}
				}

				auto end = std::chrono::high_resolution_clock::now();
				auto milliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				studentTotalTimings.push_back(milliSeconds);

				IntensityImage* studentResult = executor->resultPreProcessingStep4;

				start = std::chrono::high_resolution_clock::now();

				if (executeSteps(executor, false, studentEdgeDetectionTimings, defaultEdgeDetectionTimings)) {
					std::cout << "Default face recognition successful!" << std::endl;
					std::cout << "Facial parameters: " << std::endl;
					for (int i = 0; i < 16; i++) {
						std::cout << (i + 1) << ": " << executor->facialParameters[i] << std::endl;
					}
				}

				end = std::chrono::high_resolution_clock::now();
				milliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				defaultTotalTimings.push_back(milliSeconds);


				IntensityImage* defaultResult = executor->resultPreProcessingStep4;

				cv::Mat studentEdges;
				imageToMat(*studentResult, studentEdges);

				cv::Mat defaultEdges;
				imageToMat(*defaultResult, defaultEdges);

				cv::Mat result;
				cv::compare(studentEdges, defaultEdges, result, cv::CMP_EQ);		// Bitwise and operator
				accuracyDifference.push_back(float(100.f / countNonZero(defaultEdges)) * countNonZero(result));					// Hoeveel pixels zijn wit.

				delete executor;
			}
		}

		std::sort(studentTotalTimings.begin(), studentTotalTimings.end());
		std::sort(defaultTotalTimings.begin(), defaultTotalTimings.end());
		std::sort(accuracyDifference.begin(), accuracyDifference.end());

		/* These can easily be plotted with matplotlib in the python terminal.
		>>> import matplotlib.pyplot as plt
		>>> defaultTimings = [31, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 37, 37, 37, 38, 38, 40, 40, 41, 44, 45, 45, 47, 47, 48, 48, 48, 48, 49, 49, 49, 49, 49, 49, 50, 50, 50, 50, 51, 51, 51, 51, 52, 52, 52, 52, 53, 53, 53, 53, 53, 54, 54, 54, 55, 55, 56, 56, 56, 56, 56, 57, 58, 59, 59, 59, 60, 60, 61, 61, 65, 67, 69, 69, 70, 77, 82, 87, 91, 92]
		>>> num_bins = 20
		>>> plt.hist(defaultTimings, num_bins)
		>>> plt.show()
		*/

		std::cout << "Student total timings:" << std::endl << '[';
		for (const auto& milliSeconds : studentTotalTimings) {
			std::cout << milliSeconds << ',';
		}
		std::cout << ']' << std::endl << "Default total timings: " << std::endl << '[';
		for (const auto& milliSeconds : defaultTotalTimings) {
			std::cout << milliSeconds << ',';
		}
		std::cout << ']' << std::endl << "Student edge timings: " << std::endl << '[';
		for (const auto& milliSeconds : studentEdgeDetectionTimings) {
			std::cout << milliSeconds << ',';
		}
		std::cout << ']' << std::endl << "Default edge timings: " << std::endl << '[';
		for (const auto& milliSeconds : defaultEdgeDetectionTimings) {
			std::cout << milliSeconds << ',';
		}
		std::cout << ']' << std::endl << "Accuracy: " << std::endl << '[';
		for (const auto& accuracy : accuracyDifference) {
			std::cout << accuracy << ',';
		}
		std::cout << ']' << std::endl;
		system("pause");
	
	return 1;
}










bool executeSteps(DLLExecution* executor, const bool student, std::vector<int>& studentEdgeTimings, std::vector<int>& defaultEdgeTimings) {

	//Execute the four Pre-processing steps
	if (!executor->executePreProcessingStep1(false)) {
		std::cout << "Pre-processing step 1 failed!" << std::endl;
		return false;
	}

	if (!executor->executePreProcessingStep2(false)) {
		std::cout << "Pre-processing step 2 failed!" << std::endl;
		return false;
	}
	ImageIO::saveIntensityImage(*executor->resultPreProcessingStep2, ImageIO::getDebugFileName("Pre-processing-2.png"));


	auto start = std::chrono::high_resolution_clock::now();
	if (!executor->executePreProcessingStep3(student)) {
		std::cout << "Pre-processing step 3 failed!" << std::endl;
		return false;
	}
	ImageIO::saveIntensityImage(*executor->resultPreProcessingStep3, ImageIO::getDebugFileName("Pre-processing-3.png"));

	if (!executor->executePreProcessingStep4(student)) {
		std::cout << "Pre-processing step 4 failed!" << std::endl;
		return false;
	}
	ImageIO::saveIntensityImage(*executor->resultPreProcessingStep4, ImageIO::getDebugFileName("Pre-processing-4.png"));

	auto end = std::chrono::high_resolution_clock::now();
	auto milliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	if (student) {
		studentEdgeTimings.push_back(milliSeconds);
	} else {
		defaultEdgeTimings.push_back(milliSeconds);
	}



	//Execute the localization steps
	if (!executor->prepareLocalization()) {
		std::cout << "Localization preparation failed!" << std::endl;
		return false;
	}

	if (!executor->executeLocalizationStep1(false)) {
		std::cout << "Localization step 1 failed!" << std::endl;
		return false;
	}

	if (!executor->executeLocalizationStep2(false)) {
		std::cout << "Localization step 2 failed!" << std::endl;
		return false;
	}

	if (!executor->executeLocalizationStep3(false)) {
		std::cout << "Localization step 3 failed!" << std::endl;
		return false;
	}

	if (!executor->executeLocalizationStep4(false)) {
		std::cout << "Localization step 4 failed!" << std::endl;
		return false;
	}

	if (!executor->executeLocalizationStep5(false)) {
		std::cout << "Localization step 5 failed!" << std::endl;
		return false;
	}



	//Execute the extraction steps
	if (!executor->prepareExtraction()) {
		std::cout << "Extraction preparation failed!" << std::endl;
		return false;
	}

	if (!executor->executeExtractionStep1(false)) {
		std::cout << "Extraction step 1 failed!" << std::endl;
		return false;
	}

	if (!executor->executeExtractionStep2(false)) {
		std::cout << "Extraction step 2 failed!" << std::endl;
		return false;
	}

	if (!executor->executeExtractionStep3(false)) {
		std::cout << "Extraction step 3 failed!" << std::endl;
		return false;
	}


	//Post processing and representation
	if (!executor->executePostProcessing()) {
		std::cout << "Post-processing failed!" << std::endl;
		return false;
	}

	drawFeatureDebugImage(*executor->resultPreProcessingStep1, executor->featuresScaled);

	if (!executor->executeRepresentation()) {
		std::cout << "Representation failed!" << std::endl;
		return false;
	}
	return true;
}

void drawFeatureDebugImage(IntensityImage &image, FeatureMap &features) {
	RGB colorRed(244, 67, 54);
	RGBImage * debug = ImageFactory::newRGBImage(image.getWidth(), image.getHeight());
	ImageIO::intensityToRGB(image, *debug);

	//Nose
	Point2D<double> noseLeft = features.getFeature(Feature::FEATURE_NOSE_END_LEFT)[0];
	Point2D<double> noseRight = features.getFeature(Feature::FEATURE_NOSE_END_RIGHT)[0];
	Point2D<double> nostrilLeft = features.getFeature(Feature::FEATURE_NOSTRIL_LEFT)[0];
	Point2D<double> nostrilRight = features.getFeature(Feature::FEATURE_NOSTRIL_RIGHT)[0];
	Point2D<double> noseBottom = features.getFeature(Feature::FEATURE_NOSE_BOTTOM)[0];


	//These (weird) methods can be used to draw debug points
	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, noseLeft, colorRed);
	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, noseRight, colorRed);
	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, nostrilLeft, colorRed);
	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, nostrilRight, colorRed);
	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, noseBottom, colorRed);

	//Chin
	std::vector<Point2D<double>> points = features.getFeature(Feature::FEATURE_CHIN_CONTOUR).getPoints();
	for (size_t i = 0; i < points.size(); i++) {
		HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, points[i], colorRed);
	}

	//Eye
	Feature leftEye = features.getFeature(Feature::FEATURE_EYE_LEFT_RECT);
	Feature rightEye = features.getFeature(Feature::FEATURE_EYE_RIGHT_RECT);


	//These (weird) methods can be used to draw debug rects
	HereBeDragons::AsHisTriumphantPrizeProudOfThisPride(*debug, leftEye[0], leftEye[1], colorRed);
	HereBeDragons::AsHisTriumphantPrizeProudOfThisPride(*debug, rightEye[0], rightEye[1], colorRed);


	//Head
	Feature headTop = features.getFeature(Feature::FEATURE_HEAD_TOP);
	Feature headLeftNoseMiddle = features.getFeature(Feature::FEATURE_HEAD_LEFT_NOSE_MIDDLE);
	Feature headLeftNoseBottom = features.getFeature(Feature::FEATURE_HEAD_LEFT_NOSE_BOTTOM);
	Feature headRightNoseMiddle = features.getFeature(Feature::FEATURE_HEAD_RIGHT_NOSE_MIDDLE);
	Feature headRightNoseBottom = features.getFeature(Feature::FEATURE_HEAD_RIGHT_NOSE_BOTTOM);
	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, headTop[0], colorRed);
	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, headLeftNoseMiddle[0], colorRed);
	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, headLeftNoseBottom[0], colorRed);
	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, headRightNoseMiddle[0], colorRed);
	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, headRightNoseBottom[0], colorRed);

	//Mouth
	Point2D<double> mouthTop = features.getFeature(Feature::FEATURE_MOUTH_TOP)[0];
	Point2D<double> mouthBottom = features.getFeature(Feature::FEATURE_MOUTH_BOTTOM)[0];
	Point2D<double> mouthLeft = features.getFeature(Feature::FEATURE_MOUTH_CORNER_LEFT)[0];
	Point2D<double> mouthRight = features.getFeature(Feature::FEATURE_MOUTH_CORNER_RIGHT)[0];

	//This (weird) method can be used to draw a debug line
	HereBeDragons::ButRisingAtThyNameDothPointOutThee(*debug, mouthLeft, mouthRight, colorRed);

	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, mouthTop, colorRed);
	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, mouthBottom, colorRed);
	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, mouthLeft, colorRed);
	HereBeDragons::TriumphInLoveFleshStaysNoFatherReason(*debug, mouthRight, colorRed);

	ImageIO::saveRGBImage(*debug, ImageIO::getDebugFileName("feature-points-debug.png"));
	delete debug;
}