#include "BitmapData.h"
#include "Engine.h"
#include "PreProcessor.h"

extern "C"  _declspec(dllexport) void * CreateInstance() {
	return new Engine();
}

extern "C"  _declspec(dllexport) bool DestroyInstance(void * engine) {
	delete (Engine *)engine;
	return true;
}

extern "C"  _declspec(dllexport) bool AddElement(void * engine, wchar_t c, unsigned char * scan0, int width, int height, int stride) {
	BitmapData data(scan0, width, height, stride);
	return ((Engine *)engine)->AddElement(c, std::move(data));
}

extern "C"  _declspec(dllexport) bool ClearElements(void * engine) {
	return ((Engine *)engine)->ClearElements();
}

extern "C" _declspec(dllexport) bool GetStrings(void * engine, unsigned char * scan0, int width, int height, int stride, int spacing, int lineHeight, wchar_t ** buffer) {
	BitmapData data = BitmapData::CreateReference(scan0, width, height, stride);
	auto strings = ((Engine *)engine)->GetStrings(data, spacing, lineHeight);

	int lines = std::min((int)strings.size(), (data.Height - lineHeight) / lineHeight + 1);
	int countPerLine = (data.Width - spacing) / spacing + 1;
	for (int i = 0; i < lines; i++) {
		int count = std::min((int)strings[i].size(), countPerLine);
		for (int j = 0; j < count; j++) {
			buffer[i][j] = strings[i][j];
		}
	}

	return true;
}

extern "C" _declspec(dllexport) bool DrawStrings(void * engine, unsigned char * scan0, int width, int height, int stride, int spacing, int lineHeight, wchar_t ** buffer) {
	BitmapData data = BitmapData::CreateReference(scan0, width, height, stride);
	std::vector<std::wstring> strings;
	strings.resize((height - lineHeight) / lineHeight + 1);

	int lines = (data.Height - lineHeight) / lineHeight + 1;
	int countPerLine = (data.Width - spacing) / spacing + 1;

	for (int i = 0; i < lines; i++) {
		strings[i].resize(countPerLine);
		for (int j = 0; j < countPerLine; j++) {
			 strings[i][j] = buffer[i][j];
		}
	}

	((Engine *)engine)->DrawStrings(strings, data, spacing, lineHeight);
	return true;
}

extern "C" _declspec(dllexport) void ScaleColor(unsigned char * scan0, int width, int height, int stride, double scale) {
	BitmapData data = BitmapData::CreateReference(scan0, width, height, stride);
	PreProcessor::ScaleColor(data, scale);
}

extern "C" _declspec(dllexport) void Resize(unsigned char * scan0Raw, int widthRaw, int heightRaw, int strideRaw, unsigned char * scan0, int width, int height, int stride) {
	BitmapData dataRaw = BitmapData::CreateReference(scan0Raw, widthRaw, heightRaw, strideRaw);
	BitmapData data = BitmapData::CreateReference(scan0, width, height, stride);
	PreProcessor::Resize(dataRaw, data);
}