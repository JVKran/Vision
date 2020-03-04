#include "Mask.hpp"

Mask::Mask(const std::vector<std::vector<int>> mask, int divider):
	mask(mask),
	divider(divider)
{
	sum = calculateSum();
	if (divider == 0) {
		divider = mask.size();
	}
}

int Mask::calculateSum() {
	int currentSum = 0;
	for (const auto& row : mask) {
		for (const auto& value : row) {
			currentSum += value;
		}
	}
	return currentSum;
}

int Mask::getHeight(){
	return mask.size();
}

int Mask::getWidth(){
	if (!getHeight()) {				//If height != 0
		return mask[0].size();
	}
	return 0;
}

IntensityImage* Mask::applyMask(const IntensityImage& originalImage) {
	IntensityImage* appliedImage = ImageFactory::newIntensityImage(originalImage.getWidth(), originalImage.getHeight());
	int height = originalImage.getHeight();
	int width = originalImage.getWidth();

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < width; y++) {
			appliedImage->setPixel(x, y, maskPixel(originalImage, Vector(x, y), Vector(height, width)));
		}
	}
	return appliedImage;
}

Intensity Mask::maskPixel(const IntensityImage& originalImage, Vector position, const Vector& dimensions){
	position.x -= (getWidth() - 1) / 2;
	position.y -= (getHeight() - 1) / 2;
	int newPixelIntensity = 0;

	for (int i = 0; i < getHeight(); i++) {
		for (int j = 0; j < getWidth(); j++) {
			Intensity originalPixel = originalImage.getPixel(position.x + j, position.y + i);
			if (originalPixel != NULL) {
				if (divider != 0) {
					newPixelIntensity += (originalPixel * mask[j][i]) / divider;
				} else if (sum > 1) {
					newPixelIntensity += (originalPixel * mask[j][i]) / sum;
				} else {
					newPixelIntensity += originalPixel * mask[j][i];
				}
			}
		}
	}

	if (newPixelIntensity > 255) {
		return 255;
	}
	return newPixelIntensity;
}
