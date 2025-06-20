#include "BitmapComponent.h"
#include <wincodec.h>

ID2D1Bitmap* BitmapCreationSetup(
	ID2D1Bitmap* BitmapToCreate, const wchar_t* FileName, ID2D1HwndRenderTarget* Renderer)
{
	// If bitmap is already created, 
	// then release previous bitmap to avoid memory leak before making it nullptr
	if (BitmapToCreate)
	{
		BitmapToCreate->Release();
		BitmapToCreate = nullptr;
	}

	HRESULT Result;

	// Create Wic factory
	IWICImagingFactory* WicFactory = nullptr;
	Result = CoCreateInstance(
		CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&WicFactory));

	// Create decoder
	IWICBitmapDecoder* Decoder = nullptr;
	Result = WicFactory->CreateDecoderFromFilename(
		FileName,
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnDemand,
		&Decoder);

	// Failed to find file 
	if (!Decoder)
	{
		return nullptr;
	}

	// Create decoder frame
	IWICBitmapFrameDecode* DecoderFrame = nullptr;
	Result = Decoder->GetFrame(0, &DecoderFrame);

	// Create a converter from WIC bitmap to Direct2D bitmap
	IWICFormatConverter* WicConverter = nullptr;
	Result = WicFactory->CreateFormatConverter(&WicConverter);

	// Setup the WIC bitmap converter
	Result = WicConverter->Initialize(
		DecoderFrame,
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0,
		WICBitmapPaletteTypeCustom);

	Renderer->CreateBitmapFromWicBitmap(
		WicConverter, nullptr, &BitmapToCreate);

	if (WicFactory)
	{
		WicFactory->Release();
	}
	if (Decoder)
	{
		Decoder->Release();
	}
	if (DecoderFrame)
	{
		DecoderFrame->Release();
	}
	if (WicConverter)
	{
		WicConverter->Release();
	}

	return BitmapToCreate;
}

ID2D1Bitmap* SetupBitmap(
	ID2D1Bitmap* BitmapToSetup, const wchar_t* FileName, ID2D1HwndRenderTarget* Renderer)
{
	return BitmapCreationSetup(BitmapToSetup, FileName, Renderer);
}

void SetupBitmapComponent(
	BitmapComponent* BitmapComponentToSetup,
	ID2D1Bitmap* TextureAtlas,
	SVector TextureAtlasWidthHeight,
	SVector TextureAtlasOffsetMultiplierWidthHeight,
	bool UseEntireTextureAtlasAsBitmapSource)
{
	BitmapComponentToSetup->Bitmap = TextureAtlas;

	if (UseEntireTextureAtlasAsBitmapSource)
	{
		// Set the bitmap source the same size as the entire texture altas
		// (this is used for when there is a single texture instead of multiple "slots" in the texture atlas)
		BitmapComponentToSetup->BitmapParams.BitmapSource.X =
			TextureAtlas->GetSize().width;
		BitmapComponentToSetup->BitmapParams.BitmapSource.Y =
			TextureAtlas->GetSize().height;

		// Since computer graphics start from left to right
		// "BitmapOffsetLeft" is not set since default is at "0"
		BitmapComponentToSetup->BitmapParams.BitmapOffsetRight.X =
			BitmapComponentToSetup->BitmapParams.BitmapSource.X;
		BitmapComponentToSetup->BitmapParams.BitmapOffsetRight.Y =
			BitmapComponentToSetup->BitmapParams.BitmapSource.Y;
	}
	else
	{
		// Set the bitmap source the same size as the desired texture atlas "slot" width and height
		BitmapComponentToSetup->BitmapParams.BitmapSource.X = TextureAtlasWidthHeight.X;
		BitmapComponentToSetup->BitmapParams.BitmapSource.Y = TextureAtlasWidthHeight.Y;

		// Offset the bitmap source to the desired location of the texture atlas
		SetBitmapSourceLocationX(
			BitmapComponentToSetup,
			TextureAtlasWidthHeight.X,
			TextureAtlasOffsetMultiplierWidthHeight.X);
		SetBitmapSourceLocationY(
			BitmapComponentToSetup,
			TextureAtlasWidthHeight.Y,
			TextureAtlasOffsetMultiplierWidthHeight.Y);
	}
}

void SetBitmapSourceLocationX(
	BitmapComponent* BitmapToUpdate, int BitmapSourceWidth, int BitmapOffsetMultiplier)
{
	BitmapToUpdate->BitmapParams.BitmapSource.X = BitmapSourceWidth * BitmapOffsetMultiplier;

	BitmapToUpdate->BitmapParams.BitmapOffsetLeft.X =
		BitmapToUpdate->BitmapParams.BitmapSource.X - BitmapSourceWidth;

	BitmapToUpdate->BitmapParams.BitmapOffsetRight.X = BitmapSourceWidth;
}

void SetBitmapSourceLocationY(
	BitmapComponent* BitmapToUpdate, int BitmapSourceHeight, int BitmapOffsetMultiplier)
{
	BitmapToUpdate->BitmapParams.BitmapSource.Y = BitmapSourceHeight * BitmapOffsetMultiplier;

	BitmapToUpdate->BitmapParams.BitmapOffsetLeft.Y =
		BitmapToUpdate->BitmapParams.BitmapSource.Y - BitmapSourceHeight;

	BitmapToUpdate->BitmapParams.BitmapOffsetRight.Y = BitmapSourceHeight;
}
