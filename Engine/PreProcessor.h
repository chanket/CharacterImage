#pragma once
#include <tuple>
#include <algorithm>
#include "BitmapData.h"

class PreProcessor
{
public:
	typedef std::tuple<unsigned char, unsigned char, unsigned char> Color;

	static Color GetAverageColor(const BitmapData & data) {
		long long sumB = 0, sumG = 0, sumR = 0;

#pragma omp parallel for schedule(dynamic, 10) reduction(+: sumB) reduction(+: sumG) reduction(+: sumR)
		for (int y = 0; y < data.Height; y++) {
			unsigned char * line = data.Scan0 + data.Stride * y;
			for (int x = 0; x < data.Width; x++)
			{
				sumB += line[x * 3 + 0];
				sumG += line[x * 3 + 1];
				sumR += line[x * 3 + 2];
			}
		}

		Color retval;
		long long size = (long long)data.Width * data.Height;
		std::get<0>(retval) = sumB / size;
		std::get<1>(retval) = sumG / size;
		std::get<2>(retval) = sumR / size;

		return retval;
	}

	static void ScaleColor(BitmapData & data, double scale)
	{
#pragma omp parallel for schedule(dynamic, 10)
		for (int y = 0; y < data.Height; y++) {
			unsigned char * line = data.Scan0 + data.Stride * y;
			for (int x = 0; x < data.Width; x++)
			{
				int b = line[x * 3 + 0];
				int g = line[x * 3 + 1];
				int r = line[x * 3 + 2];

				b = (int)(b + scale);
				g = (int)(g + scale);
				r = (int)(r + scale);
				
				b = std::max(std::min(b, 255), 0);
				g = std::max(std::min(g, 255), 0);
				r = std::max(std::min(r, 255), 0);

				line[x * 3 + 0] = b;
				line[x * 3 + 1] = g;
				line[x * 3 + 2] = r;
			}
		}
	}

	static void Resize(const BitmapData & dataRaw, BitmapData & data)
	{
		double scaleX = 1.0 * dataRaw.Width / data.Width;
		double scaleY = 1.0 * dataRaw.Height / data.Height;

#pragma omp parallel for schedule(dynamic, 10)
		for (int y = 0; y < data.Height; y++) {
			double yRaw = y * scaleY;
			int yRaw0 = std::max(0, (int)std::floor(yRaw));
			int yRaw1 = std::min(dataRaw.Height - 1, (int)std::ceil(yRaw));
			if (yRaw1 == yRaw0 && yRaw1 + 1 < dataRaw.Height) yRaw1++;
			if (yRaw1 == yRaw0 && yRaw0 - 1 >= 0) yRaw0--;
			double yPropotion0 = 1.0 - (yRaw - yRaw0);
			double yPropotion1 = 1.0 - (yRaw1 - yRaw);

			unsigned char * line = data.Scan0 + data.Stride * y;
			unsigned char * lineRaw0 = dataRaw.Scan0 + dataRaw.Stride * yRaw0;
			unsigned char * lineRaw1 = dataRaw.Scan0 + dataRaw.Stride * yRaw1;
			for (int x = 0; x < data.Width; x++)
			{
				double xRaw = x * scaleX;
				int xRaw0 = std::max(0, (int)std::floor(xRaw));
				int xRaw1 = std::min(dataRaw.Width - 1, (int)std::ceil(xRaw));
				if (xRaw1 == xRaw0 && xRaw1 + 1 < dataRaw.Width) xRaw1++;
				if (xRaw1 == xRaw0 && xRaw0 - 1 >= 0) xRaw0--;
				double xPropotion0 = 1.0 - (xRaw - xRaw0);
				double xPropotion1 = 1.0 - (xRaw1 - xRaw);

				double b = 0, g = 0, r = 0;
				b += yPropotion0 * xPropotion0 * lineRaw0[xRaw0 * 3 + 0];
				b += yPropotion0 * xPropotion1 * lineRaw0[xRaw1 * 3 + 0];
				b += yPropotion1 * xPropotion0 * lineRaw1[xRaw0 * 3 + 0];
				b += yPropotion1 * xPropotion1 * lineRaw1[xRaw1 * 3 + 0];
				g += yPropotion0 * xPropotion0 * lineRaw0[xRaw0 * 3 + 1];
				g += yPropotion0 * xPropotion1 * lineRaw0[xRaw1 * 3 + 1];
				g += yPropotion1 * xPropotion0 * lineRaw1[xRaw0 * 3 + 1];
				g += yPropotion1 * xPropotion1 * lineRaw1[xRaw1 * 3 + 1];
				r += yPropotion0 * xPropotion0 * lineRaw0[xRaw0 * 3 + 2];
				r += yPropotion0 * xPropotion1 * lineRaw0[xRaw1 * 3 + 2];
				r += yPropotion1 * xPropotion0 * lineRaw1[xRaw0 * 3 + 2];
				r += yPropotion1 * xPropotion1 * lineRaw1[xRaw1 * 3 + 2];

				if (b > 255) b = 255;
				if (g > 255) g = 255;
				if (r > 255) r = 255;
				line[x * 3 + 0] = b;
				line[x * 3 + 1] = g;
				line[x * 3 + 2] = r;
			}
		}


	}
};