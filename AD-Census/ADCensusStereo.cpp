/* -*-c++-*- AD-Census - Copyright (C) 2020.
* Author	: Yingsong Li(Ethan Li) <ethan.li.whu@gmail.com>
*			  https://github.com/ethan-li-coding/AD-Census
* Describe	: implement of ad-census stereo class
*/
#include "ADCensusStereo.h"
#include <algorithm>
#include <chrono>
using namespace std::chrono;

ADCensusStereo::ADCensusStereo() : width_(0), height_(0), img_left_(nullptr), img_right_(nullptr),
disp_left_(nullptr), disp_right_(nullptr),
is_initialized_(false) { }

ADCensusStereo::~ADCensusStereo()
{
	Release();
	is_initialized_ = false;
}

bool ADCensusStereo::Initialize(const sint32& width, const sint32& height, const ADCensusOption& option)
{
	// ··· 賦值

	// 影像尺寸
	width_ = width;
	height_ = height;
	// 算法參數
	option_ = option;

	if (width <= 0 || height <= 0) {
		return false;
	}

	//··· 開辟內存空間
	const sint32 img_size = width_ * height_;
	const sint32 disp_range = option_.max_disparity - option_.min_disparity;
	if (disp_range <= 0) {
		return false;
	}

	// 視差圖
	disp_left_ = new float32[img_size];
	disp_right_ = new float32[img_size];

	// 初始化代價計算器，cost_init
	if (!cost_computer_.Initialize(width_, height_, option_.min_disparity, option_.max_disparity)) {
		is_initialized_ = false;
		return is_initialized_;
	}

	// 初始化代價聚合器
	if (!aggregator_.Initialize(width_, height_, option_.min_disparity, option_.max_disparity)) {
		is_initialized_ = false;
		return is_initialized_;
	}

	// 初始化多步優化器
	if (!refiner_.Initialize(width_, height_)) {
		is_initialized_ = false;
		return is_initialized_;
	}

	is_initialized_ = disp_left_ && disp_right_;

	return is_initialized_;
}

bool ADCensusStereo::Match(const uint8* img_left, const uint8* img_right, float32* disp_left)
{
	if (!is_initialized_) {
		return false;
	}
	if (img_left == nullptr || img_right == nullptr || disp_left == nullptr) {
		return false;
	}

	img_left_ = img_left;
	img_right_ = img_right;

	auto start = steady_clock::now();

	// 代價計算(計算Cost volumn)
	ComputeCost();

	auto end = steady_clock::now();
	auto tt = duration_cast<milliseconds>(end - start);
	printf("computing cost! timing :	%lf s\n", tt.count() / 1000.0);
	start = steady_clock::now();

	// 代價聚合
	CostAggregation();

	end = steady_clock::now();
	tt = duration_cast<milliseconds>(end - start);
	printf("cost aggregating! timing :	%lf s\n", tt.count() / 1000.0);
	start = steady_clock::now();

	// 掃描線優化
	ScanlineOptimize();

	end = steady_clock::now();
	tt = duration_cast<milliseconds>(end - start);
	printf("scanline optimizing! timing :	%lf s\n", tt.count() / 1000.0);
	start = steady_clock::now();

	// 計算左右視圖視差
	ComputeDisparity();
	ComputeDisparityRight();

	end = steady_clock::now();
	tt = duration_cast<milliseconds>(end - start);
	printf("computing disparities! timing :	%lf s\n", tt.count() / 1000.0);
	start = steady_clock::now();

	// 多步驟視差優化
	MultiStepRefine();

	end = steady_clock::now();
	tt = duration_cast<milliseconds>(end - start);
	printf("multistep refining! timing :	%lf s\n", tt.count() / 1000.0);
	start = steady_clock::now();

	// 輸出視差圖
	memcpy(disp_left, disp_left_, height_ * width_ * sizeof(float32));

	end = steady_clock::now();
	tt = duration_cast<milliseconds>(end - start);
	printf("output disparities! timing :	%lf s\n", tt.count() / 1000.0);

	return true;
}

bool ADCensusStereo::Reset(const uint32& width, const uint32& height, const ADCensusOption& option)
{
	// 釋放內存
	Release();

	// 重置初始化標記
	is_initialized_ = false;

	// 初始化
	return Initialize(width, height, option);
}

void ADCensusStereo::ComputeCost()
{
	// 設置代價計算器數據
	cost_computer_.SetData(img_left_, img_right_);
	// 設置代價計算器參數
	cost_computer_.SetParams(option_.lambda_ad, option_.lambda_census);
	// 計算代價
	cost_computer_.Compute();
}

void ADCensusStereo::CostAggregation()
{
	// 設置聚合器數據
	aggregator_.SetData(img_left_, img_right_, cost_computer_.get_cost_ptr());
	// 設置聚合器參數
	aggregator_.SetParams(option_.cross_L1, option_.cross_L2, option_.cross_t1, option_.cross_t2);
	// 代價聚合
	aggregator_.Aggregate(4);
}

void ADCensusStereo::ScanlineOptimize()
{
	// 設置優化器數據
	scan_line_.SetData(img_left_, img_right_, cost_computer_.get_cost_ptr(), aggregator_.get_cost_ptr());
	// 設置優化器參數
	scan_line_.SetParam(width_, height_, option_.min_disparity, option_.max_disparity, option_.so_p1, option_.so_p2, option_.so_tso);
	// 掃描線優化
	scan_line_.Optimize();
}

void ADCensusStereo::MultiStepRefine()
{
	// 設置多步優化器數據
	refiner_.SetData(img_left_, aggregator_.get_cost_ptr(), aggregator_.get_arms_ptr(), disp_left_, disp_right_);
	// 設置多步優化器參數
	refiner_.SetParam(option_.min_disparity, option_.max_disparity, option_.irv_ts, option_.irv_th, option_.lrcheck_thres,
		option_.do_lr_check, option_.do_filling, option_.do_filling, option_.do_discontinuity_adjustment);
	// 多步優化
	refiner_.Refine();
}

void ADCensusStereo::ComputeDisparity()
{
	const sint32& min_disparity = option_.min_disparity;
	const sint32& max_disparity = option_.max_disparity;
	const sint32 disp_range = max_disparity - min_disparity;
	if (disp_range <= 0) {
		return;
	}

	// 左影像視差圖
	const auto disparity = disp_left_;
	// 左影像聚合代價數組
	const auto cost_ptr = aggregator_.get_cost_ptr();

	const sint32 width = width_;
	const sint32 height = height_;

	// 為了加快讀取效率，把單個像素的所有代價值存儲到局部數組裡
	std::vector<float32> cost_local(disp_range);

	// ---逐像素計算最優視差
	for (sint32 i = 0; i < height; i++) {
		for (sint32 j = 0; j < width; j++) {
			float32 min_cost = Large_Float;
			sint32 best_disparity = 0;

			// ---遍歷視差範圍內的所有代價值，輸出最小代價值及對應的視差值
			for (sint32 d = min_disparity; d < max_disparity; d++) {
				const sint32 d_idx = d - min_disparity;
				const auto& cost = cost_local[d_idx] = cost_ptr[i * width * disp_range + j * disp_range + d_idx];
				if (min_cost > cost) {
					min_cost = cost;
					best_disparity = d;
				}
			}
			// ---子像素擬合
			if (best_disparity == min_disparity || best_disparity == max_disparity - 1) {
				disparity[i * width + j] = Invalid_Float;
				continue;
			}
			// 最優視差前一個視差的代價值cost_1，後一個視差的代價值cost_2
			const sint32 idx_1 = best_disparity - 1 - min_disparity;
			const sint32 idx_2 = best_disparity + 1 - min_disparity;
			const float32 cost_1 = cost_local[idx_1];
			const float32 cost_2 = cost_local[idx_2];
			// 解一元二次曲線極值
			const float32 denom = cost_1 + cost_2 - 2 * min_cost;
			if (denom != 0.0f) {
				disparity[i * width + j] = static_cast<float32>(best_disparity) + (cost_1 - cost_2) / (denom * 2.0f);
			}
			else {
				disparity[i * width + j] = static_cast<float32>(best_disparity);
			}
		}
	}
}

void ADCensusStereo::ComputeDisparityRight()
{
	const sint32& min_disparity = option_.min_disparity;
	const sint32& max_disparity = option_.max_disparity;
	const sint32 disp_range = max_disparity - min_disparity;
	if (disp_range <= 0) {
		return;
	}

	// 右影像視差圖
	const auto disparity = disp_right_;
	// 左影像聚合代價數組
	const auto cost_ptr = aggregator_.get_cost_ptr();

	const sint32 width = width_;
	const sint32 height = height_;

	// 為了加快讀取效率，把單個像素的所有代價值存儲到局部數組裡
	std::vector<float32> cost_local(disp_range);

	// ---逐像素計算最優視差
	// 通過左影像的代價，獲取右影像的代價
	// 右cost(xr,yr,d) = 左cost(xr+d,yl,d)
	for (sint32 i = 0; i < height; i++) {
		for (sint32 j = 0; j < width; j++) {
			float32 min_cost = Large_Float;
			sint32 best_disparity = 0;

			// ---統計候選視差下的代價值
			for (sint32 d = min_disparity; d < max_disparity; d++) {
				const sint32 d_idx = d - min_disparity;
				const sint32 col_left = j + d;
				if (col_left >= 0 && col_left < width) {
					const auto& cost = cost_local[d_idx] = cost_ptr[i * width * disp_range + col_left * disp_range + d_idx];
					if (min_cost > cost) {
						min_cost = cost;
						best_disparity = d;
					}
				}
				else {
					cost_local[d_idx] = Large_Float;
				}
			}

			// ---子像素擬合
			if (best_disparity == min_disparity || best_disparity == max_disparity - 1) {
				disparity[i * width + j] = best_disparity;
				continue;
			}

			// 最優視差前一個視差的代價值cost_1，後一個視差的代價值cost_2
			const sint32 idx_1 = best_disparity - 1 - min_disparity;
			const sint32 idx_2 = best_disparity + 1 - min_disparity;
			const float32 cost_1 = cost_local[idx_1];
			const float32 cost_2 = cost_local[idx_2];
			// 解一元二次曲線極值
			const float32 denom = cost_1 + cost_2 - 2 * min_cost;
			if (denom != 0.0f) {
				disparity[i * width + j] = static_cast<float32>(best_disparity) + (cost_1 - cost_2) / (denom * 2.0f);
			}
			else {
				disparity[i * width + j] = static_cast<float32>(best_disparity);
			}
		}
	}
}

void ADCensusStereo::Release()
{
	SAFE_DELETE(disp_left_);
	SAFE_DELETE(disp_right_);
}