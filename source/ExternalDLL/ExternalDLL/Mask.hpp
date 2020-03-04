#pragma once

#include <vector>
#include "IntensityImage.h"
#include "imageFactory.h"

class Vector {
	public:
		int x;
		int y;

		Vector(int x = 0, int y = 0) :
			x(x),
			y(y)
		{}

		Vector(const Vector& vectorToCopy) :
			x(vectorToCopy.x),
			y(vectorToCopy.y)
		{}
};

class Mask {
	private:
		std::vector<std::vector<int>> mask;
		int divider;

		int sum;
		int calculateSum();
	public:
		Mask(const std::vector<std::vector<int>> mask, int divider = 0);

		int getHeight();
		int getWidth();

		Intensity maskPixel(const IntensityImage& original, Vector position, const Vector& dimensions);
		IntensityImage * applyMask(const IntensityImage& originalImage);
};
