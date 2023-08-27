/* -*-c++-*- AD-Census - Copyright (C) 2020.
* Author	: Yingsong Li(Ethan Li) <ethan.li.whu@gmail.com>
* https://github.com/ethan-li-coding/AD-Census
* Describe	: header of class ScanlineOptimizer
*/

#ifndef AD_CENSUS_SCANLNE_OPTIMIZER_H_
#define AD_CENSUS_SCANLNE_OPTIMIZER_H_

#include <algorithm>

#include "adcensus_types.h"

/**
 * \brief 掃描線優化器
 */
class ScanlineOptimizer {
public:
	ScanlineOptimizer();
	~ScanlineOptimizer();


	/**
	 * \brief 設置數據
	 * \param img_left		// 左影像數據，三通道
	 * \param img_right 	// 右影像數據，三通道
	 * \param cost_init 	// 初始代價數組
	 * \param cost_aggr 	// 聚合代價數組
	 */
	void SetData(const uint8* img_left, const uint8* img_right, float32* cost_init, float32* cost_aggr);

	/**
	 * \brief
	 * \param width			// 影像寬
	 * \param height		// 影像高
	 * \param min_disparity	// 最小視差
	 * \param max_disparity // 最大視差
	 * \param p1			// p1
	 * \param p2			// p2
	 * \param tso			// tso
	 */
	void SetParam(const sint32& width, const sint32& height, const sint32& min_disparity, const sint32& max_disparity, const float32& p1, const float32& p2, const sint32& tso);

	/**
	 * \brief 優化 */
	void Optimize();

private:
	/**
	* \brief 左右路徑優化 → ←
	* \param cost_so_src		輸入，SO前代價數據
	* \param cost_so_dst		輸出，SO後代價數據
	* \param is_forward			輸入，是否為正方向（正方向為從左到右，反方向為從右到左）
	*/
	void ScanlineOptimizeLeftRight(const float32* cost_so_src, float32* cost_so_dst, bool is_forward = true);

	/**
	* \brief 上下路徑優化 ↓ ↑
	* \param cost_so_src		輸入，SO前代價數據
	* \param cost_so_dst		輸出，SO後代價數據
	* \param is_forward			輸入，是否為正方向（正方向為從上到下，反方向為從下到上）
	*/
	void ScanlineOptimizeUpDown(const float32* cost_so_src, float32* cost_so_dst, bool is_forward = true);

	/** \brief 計算顏色距離 */
	inline sint32 ColorDist(const ADColor& c1, const ADColor& c2) {
		return std::max(abs(c1.r - c2.r), std::max(abs(c1.g - c2.g), abs(c1.b - c2.b)));
	}

private:
	/** \brief 圖像尺寸 */
	sint32	width_;
	sint32	height_;

	/** \brief 影像數據 */
	const uint8* img_left_;
	const uint8* img_right_;

	/** \brief 初始代價數組 */
	float32* cost_init_;
	/** \brief 聚合代價數組 */
	float32* cost_aggr_;

	/** \brief 最小視差值 */
	sint32 min_disparity_;
	/** \brief 最大視差值 */
	sint32 max_disparity_;
	/** \brief 初始的p1值 */
	float32 so_p1_;
	/** \brief 初始的p2值 */
	float32 so_p2_;
	/** \brief tso閾值 */
	sint32 so_tso_;
};
#endif