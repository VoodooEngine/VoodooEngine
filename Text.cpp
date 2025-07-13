#include "Text.h"
#include "VoodooEngine.h"

void CreateUITextFormat(VoodooEngine* Engine)
{
	IDWriteFactory* IDWriteFactory = nullptr;

	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&IDWriteFactory));

	IDWriteFactory->CreateTextFormat(
		L"Arial",
		NULL,
		DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		20.0f * 96.0f / 72.0f,
		L"en-US",
		&Engine->TextFormat);

	IDWriteFactory->Release();
}
