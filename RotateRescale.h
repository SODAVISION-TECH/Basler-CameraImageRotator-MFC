// RotateRescale.h
// Rotates and Rescales Images (8bit Images only)
// Copyright (c) 2021 Matthew Breit - matt.breit@baslerweb.com or matt.breit@gmail.com
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ROTATERESCALE_H
#define ROTATERESCALE_H

#ifndef LINUX_BUILD
#define WIN_BUILD
#endif

#ifdef WIN_BUILD
#define _CRT_SECURE_NO_WARNINGS // suppress fopen_s warnings for convinience
#endif

// Include Pylon libraries (if needed)
#include <pylon/PylonIncludes.h>

namespace RotateRescale
{
	// The functions in this library need images with even widths & heights (ie: 1280 x 1024 vs 1279 x 1023)
	void prepareImage(Pylon::CPylonImage &srcImage);

	// our function to downsize the image. returns an image in RGB8Planar format to preserve color
	Pylon::CPylonImage rescale(Pylon::CPylonImage &srcImage, uint32_t factor);

	// our function to rotate the image clock-wise 90 degrees. returns an image in the same format as source.
	Pylon::CPylonImage rotate90(Pylon::CPylonImage &srcImage);

	// our function to rotate the image counter-clockwise 90 degrees. returns an image in the same format as source.
	Pylon::CPylonImage rotate270(Pylon::CPylonImage &srcImage);
}

// *********************************************************************************************************
// DEFINITIONS

inline void RotateRescale::prepareImage(Pylon::CPylonImage &srcImage)
{
	if (srcImage.GetWidth() % 2 != 0 || srcImage.GetHeight() % 2 != 0)
		srcImage.CopyImage(srcImage.GetAoi(0, 0, srcImage.GetWidth() - 1, srcImage.GetHeight() - 1), 0);
	else
		srcImage.CopyImage(srcImage);
}

inline Pylon::CPylonImage RotateRescale::rescale(Pylon::CPylonImage &srcImage, uint32_t factor)
{
	RotateRescale::prepareImage(srcImage);

	uint32_t srcHeight = srcImage.GetHeight();
	uint32_t srcWidth = srcImage.GetWidth();

	uint32_t destHeight = (srcHeight / factor);
	uint32_t destWidth = (srcWidth / factor);

	// If it's a color image, we'll interpolate and convert the src to RGB planar so we don't lose any color data if there is any.
	if (Pylon::IsColorImage(srcImage.GetPixelType()))
	{
		Pylon::CPylonImage destImage(Pylon::CPylonImage::Create(Pylon::EPixelType::PixelType_RGB8planar, destWidth, destHeight, 0));

		Pylon::CImageFormatConverter converter;
		converter.OutputPixelFormat.SetValue(Pylon::EPixelType::PixelType_RGB8planar);
		Pylon::CPylonImage srcPlanar;
		converter.Convert(srcPlanar, srcImage);

		Pylon::CPylonImage srcRed(srcPlanar.GetPlane(0));
		Pylon::CPylonImage srcGreen(srcPlanar.GetPlane(1));
		Pylon::CPylonImage srcBlue(srcPlanar.GetPlane(2));

		Pylon::CPylonImage destRed(destImage.GetPlane(0));
		Pylon::CPylonImage destGreen(destImage.GetPlane(1));
		Pylon::CPylonImage destBlue(destImage.GetPlane(2));

		uint8_t* pSrcRed = (uint8_t*)srcRed.GetBuffer();
		uint8_t* pDestRed = (uint8_t*)destRed.GetBuffer();

		uint8_t* pSrcGreen = (uint8_t*)srcGreen.GetBuffer();
		uint8_t* pDestGreen = (uint8_t*)destGreen.GetBuffer();

		uint8_t* pSrcBlue = (uint8_t*)srcBlue.GetBuffer();
		uint8_t* pDestBlue = (uint8_t*)destBlue.GetBuffer();

		for (uint64_t y = 0; y < destHeight; y++)
		{
			for (uint64_t x = 0; x < destWidth; x++, ++pDestRed, ++pDestGreen, ++pDestBlue)
			{
				*pDestRed = (uint8_t)(pSrcRed[(x * factor) + (srcWidth * y * factor)]);
				*pDestGreen = (uint8_t)(pSrcGreen[(x * factor) + (srcWidth* y * factor)]);
				*pDestBlue = (uint8_t)(pSrcBlue[(x * factor) + (srcWidth * y * factor)]);
			}
		}

		return destImage;
	}
	else
	{
		Pylon::CPylonImage destImage(Pylon::CPylonImage::Create(srcImage.GetPixelType(), destWidth, destHeight, 0));
		uint8_t* pSrc = (uint8_t*)srcImage.GetBuffer();
		uint8_t* pDest = (uint8_t*)destImage.GetBuffer();
		for (uint64_t y = 0; y < destHeight; y++)
		{
			for (uint64_t x = 0; x < destWidth; x++, ++pDest)
			{
				*pDest = (uint8_t)(pSrc[(x * factor) + (srcWidth * y * factor)]);
			}
		}

		return destImage;
	}
}

inline Pylon::CPylonImage RotateRescale::rotate90(Pylon::CPylonImage &srcImage)
{
	RotateRescale::prepareImage(srcImage);

	uint32_t srcWidth = srcImage.GetWidth();
	uint32_t srcHeight = srcImage.GetHeight();

	uint8_t* pSrc = (uint8_t*)srcImage.GetBuffer();

	Pylon::CPylonImage destImage(Pylon::CPylonImage::Create(srcImage.GetPixelType(), srcHeight, srcWidth, 0));
	uint8_t* pDest = (uint8_t*)destImage.GetBuffer();

	// rotation algorithm
	for (int y = 0, col = srcHeight - 1; y < srcHeight; ++y, --col)
	{
		int offset = y * srcWidth;
		for (int x = 0; x < srcWidth; x++)
		{
			pDest[(x * srcHeight) + col] = (uint8_t)pSrc[x + offset];
		}
	}

	// take care that the bayer pattern alignment changes when rotating (and depends on direction)
	if (srcImage.GetPixelType() == Pylon::EPixelType::PixelType_BayerBG8)
		destImage.ChangePixelType(Pylon::EPixelType::PixelType_BayerGB8);
	else if (srcImage.GetPixelType() == Pylon::EPixelType::PixelType_BayerGB8)
		destImage.ChangePixelType(Pylon::EPixelType::PixelType_BayerRG8);
	else if (srcImage.GetPixelType() == Pylon::EPixelType::PixelType_BayerRG8)
		destImage.ChangePixelType(Pylon::EPixelType::PixelType_BayerGR8);
	else if (srcImage.GetPixelType() == Pylon::EPixelType::PixelType_BayerGR8)
		destImage.ChangePixelType(Pylon::EPixelType::PixelType_BayerBG8);
	else if (srcImage.GetPixelType() == Pylon::EPixelType::PixelType_Mono8)
		destImage.ChangePixelType(Pylon::EPixelType::PixelType_Mono8);

	return destImage;
}

inline Pylon::CPylonImage RotateRescale::rotate270(Pylon::CPylonImage &srcImage)
{
	RotateRescale::prepareImage(srcImage);

	uint32_t srcWidth = srcImage.GetWidth();
	uint32_t srcHeight = srcImage.GetHeight();

	uint8_t* pSrc = (uint8_t*)srcImage.GetBuffer();

	Pylon::CPylonImage destImage(Pylon::CPylonImage::Create(srcImage.GetPixelType(), srcHeight, srcWidth, 0));
	uint8_t* pDest = (uint8_t*)destImage.GetBuffer();

	// rotation algorithm
	for (int y = 0, col = 0; y < srcHeight, col < srcHeight; ++y, ++col)
	{
		int offset = y * srcWidth;
		for (int x = 0; x < srcWidth; x++)
		{
			pDest[((srcWidth * srcHeight) - srcHeight) - (x * srcHeight) + col] = (uint8_t)pSrc[x + offset];
		}
	}

	// take care that the bayer pattern alignment changes when rotating (and depends on direction)
	if (srcImage.GetPixelType() == Pylon::EPixelType::PixelType_BayerBG8)
		destImage.ChangePixelType(Pylon::EPixelType::PixelType_BayerGR8);
	else if (srcImage.GetPixelType() == Pylon::EPixelType::PixelType_BayerGB8)
		destImage.ChangePixelType(Pylon::EPixelType::PixelType_BayerBG8);
	else if (srcImage.GetPixelType() == Pylon::EPixelType::PixelType_BayerRG8)
		destImage.ChangePixelType(Pylon::EPixelType::PixelType_BayerGB8);
	else if (srcImage.GetPixelType() == Pylon::EPixelType::PixelType_BayerGR8)
		destImage.ChangePixelType(Pylon::EPixelType::PixelType_BayerRG8);
	else if (srcImage.GetPixelType() == Pylon::EPixelType::PixelType_Mono8)
		destImage.ChangePixelType(Pylon::EPixelType::PixelType_Mono8);

	return destImage;
}
#endif
// *********************************************************************************************************