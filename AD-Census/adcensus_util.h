/* -*-c++-*- AD-Census - Copyright (C) 2020.
* Author	: Yingsong Li(Ethan Li) <ethan.li.whu@gmail.com>
* https://github.com/ethan-li-coding/AD-Census
* Describe	: header of adcensus_util
*/

#pragma once
#include <algorithm>
#include "adcensus_types.h"


namespace adcensus_util
{
	/**
	* \brief census變換
	* \param source	輸入，影像數據
	* \param census	輸出，census值數組，預分配空間
	* \param width	輸入，影像寬
	* \param height	輸入，影像高
	*/
	void census_transform_9x7(const uint8* source, vector<uint64>& census, const sint32& width, const sint32& height);
	// Hamming距離
	uint8 Hamming64(const uint64& x, const uint64& y);

	/**
	* \brief 中值濾波
	* \param in				輸入，源數據
	* \param out			輸出，目標數據
	* \param width			輸入，寬度
	* \param height			輸入，高度
	* \param wnd_size		輸入，窗口寬度
	*/
	void MedianFilter(const float32* in, float32* out, const sint32& width, const sint32& height, const sint32 wnd_size);
}