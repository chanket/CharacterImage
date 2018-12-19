#pragma once
#include <map>
#include <vector>
#include <string>
#include <limits>
#include <algorithm>
#include <intrin.h>
#include "BitmapData.h"

class Engine
{
protected:
	std::map<wchar_t, BitmapData> Elements;

	long long GetErrorSSE(const BitmapData & image, const BitmapData & element, int xStart, int yStart) {
		long long error = 0;
		for (int y = 0; y < element.Height && y + yStart < image.Height; y++)
		{
			int xElement = 0;
			int xImage = xStart * 3;
			const int xEndElement = element.Width * 3;
			const int xEndImage = image.Width * 3;
			const auto lineElement = element.Scan0 + element.Stride * y;
			const auto lineImage = image.Scan0 + image.Stride * (y + yStart);

			//每次比较16个字节
			constexpr auto batchSSE = 16;
			for (; xElement + batchSSE < xEndElement && xImage + batchSSE < xEndImage; xElement += batchSSE, xImage += batchSSE) {
				const auto c0 = _mm_loadu_si128((__m128i *)(lineElement + xElement));
				const auto c1 = _mm_loadu_si128((__m128i *)(lineImage + xImage));
				const auto abs = _mm_sad_epu8(c0, c1);

				error += abs.m128i_i16[0] + abs.m128i_i16[4];
			}

			//尾数据
			for (; xElement < xEndElement && xImage < xEndImage; xElement++, xImage++) {
				const unsigned char c0 = lineElement[xElement];
				const unsigned char c1 = lineImage[xImage];

				error += std::abs(c0 - c1);
			}
		}

		return error;
	}

	void DrawElementSSE(BitmapData & image, const BitmapData & element, int xStart, int yStart) {
		for (int y = 0; y < element.Height && y + yStart < image.Height; y++)
		{
			int xElement = 0;
			int xImage = xStart * 3;
			const int xEndElement = element.Width * 3;
			const int xEndImage = image.Width * 3;
			const auto lineElement = element.Scan0 + element.Stride * y;
			const auto lineImage = image.Scan0 + image.Stride * (y + yStart);

			//每次绘制16个字节
			constexpr auto batchSSE = 16;
			for (; xElement + batchSSE < xEndElement && xImage + batchSSE < xEndImage; xElement += batchSSE, xImage += batchSSE) {
				const auto c0 = _mm_loadu_si128((__m128i *)(lineElement + xElement));
				const auto c1 = _mm_loadu_si128((__m128i *)(lineImage + xImage));
				const auto c2 = _mm_min_epu8(c0, c1);
				_mm_storeu_si128((__m128i *)(lineImage + xImage), c2);
			}

			//尾数据
			for (; xElement < xEndElement && xImage < xEndImage; xElement++, xImage++) {
				lineImage[xImage] = std::min(lineImage[xImage], lineElement[xElement]);
			}
		}
	}

public:
	Engine() {

	};
	virtual ~Engine() {

	};

public:
	bool AddElement(wchar_t c, BitmapData && data) {
		if (c == 0) return false;

		if (Elements.find(c) == Elements.end()) {
			Elements.insert(std::make_pair(c, std::move(data)));
		}
		else {
			return false;
		}
	}

	bool ClearElements() {
		Elements.clear();
		return true;
	}

	std::vector<std::wstring> GetStrings(const BitmapData & image, int spacing, int lineHeight) {
		std::vector<std::wstring> retval;
		retval.resize((image.Height - lineHeight) / lineHeight + 1);

#pragma omp parallel for schedule(dynamic, 1)
		for (int i = 0; i < retval.size(); i++) {
			int y = i * lineHeight;
			retval[i].reserve((image.Height - spacing) / spacing + 2);

			for (int x = 0; x < image.Width; x += spacing)
			{
				long long e = std::numeric_limits<long long>::max();
				wchar_t c = ' ';

				for (auto & element : Elements)
				{
					long long ee = GetErrorSSE(image, element.second, x, y);
					if (ee < e)
					{
						e = ee;
						c = element.first;
					}
				}

				retval[i] += c;
			}
		}

		return retval;
	}

	void DrawStrings(const std::vector<std::wstring> strings, BitmapData & image, int spacing, int lineHeight) {
		memset(image.Scan0, 255, image.Height * image.Stride);

#pragma omp parallel for schedule(dynamic, 1)
		for (int i = 0; i < strings.size(); i++) {
			int y = i * lineHeight;
			int x = 0;

			for (auto & c : strings[i]) {
				const auto iter = Elements.find(c);
				if (iter != Elements.end()) {
					DrawElementSSE(image, iter->second, x, y);
				}

				x += spacing;
			}
		}
	}
};

