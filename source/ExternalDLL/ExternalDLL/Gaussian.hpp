#pragma once

#include <vector>
#include <cmath>

class GaussianFilter {
	private:
		double standardDeviation;
		int maskSize;

		double factor;
		double bottom;
	public:
		GaussianFilter(double standardDeviation, int maskSize);

		std::vector<std::vector<int>> getFilter(int median);
		double getWeight(const int x, const int y);
};