/* -*-c++-*- AD-Census - Copyright (C) 2020.
* Author	: Yingsong Li(Ethan Li) <ethan.li.whu@gmail.com>
* https://github.com/ethan-li-coding/AD-Census
* Describe	: implement of class ScanlineOptimizer
*/

#include "scanline_optimizer.h"

#include <cassert>

ScanlineOptimizer::ScanlineOptimizer() : width_(0), height_(0), img_left_(nullptr), img_right_(nullptr),
cost_init_(nullptr), cost_aggr_(nullptr),
min_disparity_(0), max_disparity_(0),
so_p1_(0), so_p2_(0),
so_tso_(0) {}

ScanlineOptimizer::~ScanlineOptimizer() {}

void ScanlineOptimizer::SetData(const uint8* img_left, const uint8* img_right, float32* cost_init,
	float32* cost_aggr)
{
	img_left_ = img_left;
	img_right_ = img_right;
	cost_init_ = cost_init;
	cost_aggr_ = cost_aggr;
}

void ScanlineOptimizer::SetParam(const sint32& width, const sint32& height, const sint32& min_disparity,
	const sint32& max_disparity, const float32& p1, const float32& p2, const sint32& tso)
{
	width_ = width;
	height_ = height;
	min_disparity_ = min_disparity;
	max_disparity_ = max_disparity;
	so_p1_ = p1;
	so_p2_ = p2;
	so_tso_ = tso;
}

void ScanlineOptimizer::Optimize()
{
	if (width_ <= 0 || height_ <= 0 ||
		img_left_ == nullptr || img_right_ == nullptr ||
		cost_init_ == nullptr || cost_aggr_ == nullptr) {
		return;
	}

	// 4方向掃描線優化
	// 模塊的首次輸入是上一步代價聚合後的數據，也就是cost_aggr_
	// 我們把四個方向的優化按次序進行，並利用cost_init_及cost_aggr_間次保存臨時數據，這樣不用開辟額外的內存來存儲中間結果
	// 模塊的最終輸出也是cost_aggr_

	// left to right
	ScanlineOptimizeLeftRight(cost_aggr_, cost_init_, true);
	// right to left
	ScanlineOptimizeLeftRight(cost_init_, cost_aggr_, false);
	// up to down
	ScanlineOptimizeUpDown(cost_aggr_, cost_init_, true);
	// down to up
	ScanlineOptimizeUpDown(cost_init_, cost_aggr_, false);
}

void ScanlineOptimizer::ScanlineOptimizeLeftRight(const float32* cost_so_src, float32* cost_so_dst, bool is_forward)
{
	const auto width = width_;
	const auto height = height_;
	const auto min_disparity = min_disparity_;
	const auto max_disparity = max_disparity_;
	const auto p1 = so_p1_;
	const auto p2 = so_p2_;
	const auto tso = so_tso_;

	assert(width > 0 && height > 0 && max_disparity > min_disparity);

	// 視差範圍
	const sint32 disp_range = max_disparity - min_disparity;

	// 正向(左->右) ：is_forward = true ; direction = 1
	// 反向(右->左) ：is_forward = false; direction = -1;
	const sint32 direction = is_forward ? 1 : -1;

	// 聚合
	for (sint32 y = 0u; y < height; y++) {
		// 路徑頭為每一行的首(尾,dir=-1)列像素
		auto cost_init_row = (is_forward) ? (cost_so_src + y * width * disp_range) : (cost_so_src + y * width * disp_range + (width - 1) * disp_range);
		auto cost_aggr_row = (is_forward) ? (cost_so_dst + y * width * disp_range) : (cost_so_dst + y * width * disp_range + (width - 1) * disp_range);
		auto img_row = (is_forward) ? (img_left_ + y * width * 3) : (img_left_ + y * width * 3 + 3 * (width - 1));
		const auto img_row_r = img_right_ + y * width * 3;
		sint32 x = (is_forward) ? 0 : width - 1;

		// 路徑上當前顏色值和上一個顏色值
		ADColor color(img_row[0], img_row[1], img_row[2]);
		ADColor color_last = color;

		// 路徑上上個像素的代價數組，多兩個元素是為了避免邊界溢出（首尾各多一個）
		std::vector<float32> cost_last_path(disp_range + 2, Large_Float);

		// 初始化：第一個像素的聚合代價值等於初始代價值
		memcpy(cost_aggr_row, cost_init_row, disp_range * sizeof(float32));
		memcpy(&cost_last_path[1], cost_aggr_row, disp_range * sizeof(float32));
		//指向下一個pixel
		cost_init_row += direction * disp_range;
		cost_aggr_row += direction * disp_range;
		img_row += direction * 3;
		x += direction;

		// 路徑上上個像素的最小代價值
		float32 mincost_last_path = Large_Float;
		for (auto cost : cost_last_path) {
			mincost_last_path = std::min(mincost_last_path, cost);
		}

		// 自方向上第2個像素開始按順序聚合
		for (sint32 j = 0; j < width - 1; j++) {
			color = ADColor(img_row[0], img_row[1], img_row[2]);
			const uint8 d1 = ColorDist(color, color_last);
			uint8 d2 = d1;
			float32 min_cost = Large_Float;
			for (sint32 d = 0; d < disp_range; d++) {
				const sint32 xr = x - d - min_disparity;
				if (xr > 0 && xr < width - 1) {
					const ADColor color_r = ADColor(img_row_r[3 * xr], img_row_r[3 * xr + 1], img_row_r[3 * xr + 2]);
					const ADColor color_last_r = ADColor(img_row_r[3 * (xr - direction)],
						img_row_r[3 * (xr - direction) + 1],
						img_row_r[3 * (xr - direction) + 2]);
					d2 = ColorDist(color_r, color_last_r);
				}

				// 計算P1和P2
				float32 P1(0.0f), P2(0.0f);
				if (d1 < tso && d2 < tso) {
					P1 = p1; P2 = p2;
				}
				else if (d1 < tso && d2 >= tso) {
					P1 = p1 / 4; P2 = p2 / 4;
				}
				else if (d1 >= tso && d2 < tso) {
					P1 = p1 / 4; P2 = p2 / 4;
				}
				else if (d1 >= tso && d2 >= tso) {
					P1 = p1 / 10; P2 = p2 / 10;
				}

				// Lr(p,d) = C(p,d) + min( Lr(p-r,d), Lr(p-r,d-1) + P1, Lr(p-r,d+1) + P1, min(Lr(p-r))+P2 ) - min(Lr(p-r))
				const float32  cost = cost_init_row[d];
				//cost_last_path因前後多一個空間
				const float32 l1 = cost_last_path[d + 1];//這個位置其實是位置d
				const float32 l2 = cost_last_path[d] + P1;//這個位置其實是位置d-1
				const float32 l3 = cost_last_path[d + 2] + P1;//這個位置其實是位置d+1
				const float32 l4 = mincost_last_path + P2;

				float32 cost_s = cost + static_cast<float32>(std::min(std::min(l1, l2), std::min(l3, l4)));
				cost_s /= 2;

				cost_aggr_row[d] = cost_s;
				min_cost = std::min(min_cost, cost_s);
			}

			// 重置上個像素的最小代價值和代價數組
			mincost_last_path = min_cost;
			memcpy(&cost_last_path[1], cost_aggr_row, disp_range * sizeof(float32));

			// 下一個像素
			cost_init_row += direction * disp_range;
			cost_aggr_row += direction * disp_range;
			img_row += direction * 3;
			x += direction;

			// 像素值重新賦值
			color_last = color;
		}
	}
}

void ScanlineOptimizer::ScanlineOptimizeUpDown(const float32* cost_so_src, float32* cost_so_dst, bool is_forward)
{
	const auto width = width_;
	const auto height = height_;
	const auto min_disparity = min_disparity_;
	const auto max_disparity = max_disparity_;
	const auto p1 = so_p1_;
	const auto p2 = so_p2_;
	const auto tso = so_tso_;

	assert(width > 0 && height > 0 && max_disparity > min_disparity);

	// 視差範圍
	const sint32 disp_range = max_disparity - min_disparity;

	// 正向(上->下) ：is_forward = true ; direction = 1
	// 反向(下->上) ：is_forward = false; direction = -1;
	const sint32 direction = is_forward ? 1 : -1;

	// 聚合
	for (sint32 x = 0; x < width; x++) {
		// 路徑頭為每一列的首(尾,dir=-1)行像素
		auto cost_init_col = (is_forward) ? (cost_so_src + x * disp_range) : (cost_so_src + (height - 1) * width * disp_range + x * disp_range);
		auto cost_aggr_col = (is_forward) ? (cost_so_dst + x * disp_range) : (cost_so_dst + (height - 1) * width * disp_range + x * disp_range);
		auto img_col = (is_forward) ? (img_left_ + 3 * x) : (img_left_ + (height - 1) * width * 3 + 3 * x);
		sint32 y = (is_forward) ? 0 : height - 1;

		// 路徑上當前灰度值和上一個灰度值
		ADColor color(img_col[0], img_col[1], img_col[2]);
		ADColor color_last = color;

		// 路徑上上個像素的代價數組，多兩個元素是為了避免邊界溢出（首尾各多一個）
		std::vector<float32> cost_last_path(disp_range + 2, Large_Float);

		// 初始化：第一個像素的聚合代價值等於初始代價值
		memcpy(cost_aggr_col, cost_init_col, disp_range * sizeof(float32));
		memcpy(&cost_last_path[1], cost_aggr_col, disp_range * sizeof(float32));
		cost_init_col += direction * width * disp_range;
		cost_aggr_col += direction * width * disp_range;
		img_col += direction * width * 3;
		y += direction;

		// 路徑上上個像素的最小代價值
		float32 mincost_last_path = Large_Float;
		for (auto cost : cost_last_path) {
			mincost_last_path = std::min(mincost_last_path, cost);
		}

		// 自方向上第2個像素開始按順序聚合
		for (sint32 i = 0; i < height - 1; i++) {
			color = ADColor(img_col[0], img_col[1], img_col[2]);
			const uint8 d1 = ColorDist(color, color_last);
			uint8 d2 = d1;
			float32 min_cost = Large_Float;
			for (sint32 d = 0; d < disp_range; d++) {
				const sint32 xr = x - d - min_disparity;
				if (xr > 0 && xr < width - 1) {
					const ADColor color_r = ADColor(img_right_[y * width * 3 + 3 * xr], img_right_[y * width * 3 + 3 * xr + 1], img_right_[y * width * 3 + 3 * xr + 2]);
					const ADColor color_last_r = ADColor(img_right_[(y - direction) * width * 3 + 3 * xr],
						img_right_[(y - direction) * width * 3 + 3 * xr + 1],
						img_right_[(y - direction) * width * 3 + 3 * xr + 2]);
					d2 = ColorDist(color_r, color_last_r);
				}
				// 計算P1和P2
				float32 P1(0.0f), P2(0.0f);
				if (d1 < tso && d2 < tso) {
					P1 = p1; P2 = p2;
				}
				else if (d1 < tso && d2 >= tso) {
					P1 = p1 / 4; P2 = p2 / 4;
				}
				else if (d1 >= tso && d2 < tso) {
					P1 = p1 / 4; P2 = p2 / 4;
				}
				else if (d1 >= tso && d2 >= tso) {
					P1 = p1 / 10; P2 = p2 / 10;
				}

				// Lr(p,d) = C(p,d) + min( Lr(p-r,d), Lr(p-r,d-1) + P1, Lr(p-r,d+1) + P1, min(Lr(p-r))+P2 ) - min(Lr(p-r))
				const float32  cost = cost_init_col[d];
				const float32 l1 = cost_last_path[d + 1];
				const float32 l2 = cost_last_path[d] + P1;
				const float32 l3 = cost_last_path[d + 2] + P1;
				const float32 l4 = mincost_last_path + P2;

				float32 cost_s = cost + static_cast<float32>(std::min(std::min(l1, l2), std::min(l3, l4)));
				cost_s /= 2;

				cost_aggr_col[d] = cost_s;
				min_cost = std::min(min_cost, cost_s);
			}

			// 重置上個像素的最小代價值和代價數組
			mincost_last_path = min_cost;
			memcpy(&cost_last_path[1], cost_aggr_col, disp_range * sizeof(float32));

			// 下一個像素
			cost_init_col += direction * width * disp_range;
			cost_aggr_col += direction * width * disp_range;
			img_col += direction * width * 3;
			y += direction;

			// 像素值重新賦值
			color_last = color;
		}
	}
}