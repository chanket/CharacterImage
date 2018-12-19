#pragma once
#include <memory>

struct BitmapData
{
private:
	bool deleteScan0 = true;
	BitmapData() {

	}

public:
	//����ָ�����ݵ�����
	//�������Ķ��󲻻Ḵ��scan0������ʱҲ�����ͷ�scan0
	static BitmapData CreateReference(unsigned char * scan0, int width, int height, int stride) {
		BitmapData retval;
		retval.Width = width;
		retval.Height = height;
		retval.Stride = stride;
		retval.Scan0 = scan0;
		retval.deleteScan0 = false;
		return retval;
	}

	//���캯��
	BitmapData(unsigned char * scan0, int width, int height, int stride) {
		Width = width;
		Height = height;
		Stride = stride;
		Scan0 = new unsigned char[Height * Stride];
		deleteScan0 = true;
		std::memcpy(Scan0, scan0, Height * Stride);
	};

	//��������
	BitmapData(const BitmapData & copy) {
		Width = copy.Width;
		Height = copy.Height;
		Stride = copy.Stride;
		Scan0 = new unsigned char[Height * Stride];
		deleteScan0 = true;
		std::memcpy(Scan0, copy.Scan0, Height * Stride);
	}

	//�ƶ�����
	BitmapData(BitmapData && move) {
		Width = move.Width;
		Height = move.Height;
		Stride = move.Stride;
		Scan0 = move.Scan0;
		deleteScan0 = move.deleteScan0;
		move.Width = 0;
		move.Height = 0;
		move.Stride = 0;
		move.Scan0 = nullptr;
	}

	//����
	~BitmapData() {
		if (Scan0 != nullptr && deleteScan0) delete Scan0;
	}

	unsigned char * Scan0 = nullptr;
	int Width = 0;
	int Height = 0;
	int Stride = 0;
};

