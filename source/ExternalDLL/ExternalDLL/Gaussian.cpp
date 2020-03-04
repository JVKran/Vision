#include "Gaussian.hpp"

GaussianFilter::GaussianFilter(double standardDeviation, int maskSize) :
	standardDeviation(standardDeviation),
	maskSize(maskSize)
{
	factor = 1.0 / (2.0 * 3.1415926535897 * standardDeviation * standardDeviation);
	bottom = 2.0 * standardDeviation * standardDeviation;
}

std::vector<std::vector<int>> GaussianFilter::getFilter(int median){
	int middleIndex = (maskSize - 1) / 2;
	double middleValue = getWeight(middleIndex + 1, middleIndex + 1);
	double scale = median / middleValue;
	std::vector<std::vector<int>> filter;
	for (int i = 1; i < maskSize + 1; i++) {
		filter.push_back(std::vector<int>{});
		for (int j = 1; j < maskSize + 1; j++) {
			if (i == middleIndex + 1 && j == middleIndex + 1) {
				filter[middleIndex].push_back(static_cast<int>(middleValue * scale));
			} else {
				filter[i - 1].push_back(static_cast<int>(getWeight(i, j) * scale));
			}
		}
	}
	return filter;
}

double GaussianFilter::getWeight(const int x, const int y){
	static int halfMaskSize = (maskSize - 1) / 2;
	double top = std::pow((x - (halfMaskSize + 1)), 2) + std::pow((y - (halfMaskSize + 1)), 2);
	return factor * std::exp2(-(top) / (bottom));
}
