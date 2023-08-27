/* -*-c++-*- AD-Census - Copyright (C) 2020.
* Author	: Yingsong Li(Ethan Li) <ethan.li.whu@gmail.com>
* https://github.com/ethan-li-coding/AD-Census
* Describe	: header of class CrossAggregator
*/

#ifndef AD_CENSUS_CROSS_AGGREGATOR_H_
#define AD_CENSUS_CROSS_AGGREGATOR_H_

#include "adcensus_types.h"
#include <algorithm>

/**
* \brief 交叉十字臂結構
* 為了限制內存占用，臂長類型設置為uint8，這意味著臂長最長不能超過255
*/
struct CrossArm {
	uint8 left, right, top, bottom;
	CrossArm() : left(0), right(0), top(0), bottom(0) { }
};
/**\brief 最大臂長 */
#define MAX_ARM_LENGTH 255 

/**
 * \brief 十字交叉域代價聚合器
 */
class CrossAggregator {
public:
	CrossAggregator();
	~CrossAggregator();

	/**
	 * \brief 初始化代價聚合器
	 * \param width		影像寬
	 * \param height	影像高
	 * \return true:初始化成功
	 */
	bool Initialize(const sint32& width, const sint32& height, const sint32& min_disparity, const sint32& max_disparity);

	/**
	 * \brief 設置代價聚合器的數據
	 * \param img_left		// 左影像數據，三通道
	 * \param img_right		// 右影像數據，三通道
	 * \param cost_init		// 初始代價數組
	 */
	void SetData(const uint8* img_left, const uint8* img_right, const float32* cost_init);

	/**
	 * \brief 設置代價聚合器的參數
	 * \param cross_L1		// L1
	 * \param cross_L2		// L2
	 * \param cross_t1		// t1
	 * \param cross_t2		// t2
	 */
	void SetParams(const sint32& cross_L1, const sint32& cross_L2, const sint32& cross_t1, const sint32& cross_t2);

	/** \brief 聚合 */
	void Aggregate(const sint32& num_iters);

	/** \brief 獲取所有像素的十字交叉臂數據指針 */
	CrossArm* get_arms_ptr();

	/** \brief 獲取聚合代價數組指針 */
	float32* get_cost_ptr();
private:
	/** \brief 構建十字交叉臂 */
	void BuildArms();
	/** \brief 搜索水平臂 */
	void FindHorizontalArm(const sint32& x, const sint32& y, uint8& left, uint8& right) const;
	/** \brief 搜索豎直臂 */
	void FindVerticalArm(const sint32& x, const sint32& y, uint8& top, uint8& bottom) const;
	/** \brief 計算像素的支持區像素數量 */
	void ComputeSupPixelCount();
	/** \brief 聚合某個視差 */
	void AggregateInArms(const sint32& disparity, const bool& horizontal_first);

	/** \brief 計算顏色距離 */
	inline sint32 ColorDist(const ADColor& c1, const ADColor& c2) const {
		return std::max(abs(c1.r - c2.r), std::max(abs(c1.g - c2.g), abs(c1.b - c2.b)));
	}
private:
	/** \brief 圖像尺寸 */
	sint32	width_;
	sint32	height_;

	/** \brief 交叉臂 */
	vector<CrossArm> vec_cross_arms_;

	/** \brief 影像數據 */
	const uint8* img_left_;
	const uint8* img_right_;

	/** \brief 初始代價數組指針 */
	const float32* cost_init_;
	/** \brief 聚合代價數組 */
	vector<float32> cost_aggr_;

	/** \brief 臨時代價數據 */
	vector<float32> vec_cost_tmp_[2];
	/** \brief 儲存支持區像素數量數組 0：水平臂優先 1：豎直臂優先 */
	vector<uint16> vec_sup_count_[2];
	vector<uint16> vec_sup_count_tmp_;

	sint32	cross_L1_;			// 十字交叉窗口的空間域參數：L1
	sint32  cross_L2_;			// 十字交叉窗口的空間域參數：L2
	sint32	cross_t1_;			// 十字交叉窗口的顏色域參數：t1
	sint32  cross_t2_;			// 十字交叉窗口的顏色域參數：t2
	sint32  min_disparity_;			// 最小視差
	sint32	max_disparity_;			// 最大視差

	/** \brief 是否成功初始化標志	*/
	bool is_initialized_;
};
#endif