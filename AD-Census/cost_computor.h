/* -*-c++-*- AD-Census - Copyright (C) 2020.
* Author	: Yingsong Li(Ethan Li) <ethan.li.whu@gmail.com>
* https://github.com/ethan-li-coding/AD-Census
* Describe	: header of class CostComputor
*/

#ifndef AD_CENSUS_COST_COMPUTOR_H_
#define AD_CENSUS_COST_COMPUTOR_H_

#include "adcensus_types.h"

/**
 * \brief 代價計算器類
 */
class CostComputor {
public:
	CostComputor();
	~CostComputor();

	/**
	 * \brief 初始化
	 * \param width			影像寬
	 * \param height		影像高
	 * \param min_disparity	最小視差
	 * \param max_disparity	最大視差
	 * \return true: 初始化成功
	 */
	bool Initialize(const sint32& width, const sint32& height, const sint32& min_disparity, const sint32& max_disparity);

	/**
	 * \brief 設置代價計算器的數據
	 * \param img_left		// 左影像數據，三通道
	 * \param img_right		// 右影像數據，三通道
	 */
	void SetData(const uint8* img_left, const uint8* img_right);

	/**
	 * \brief 設置代價計算器的參數
	 * \param lambda_ad		// lambda_ad
	 * \param lambda_census // lambda_census
	 */
	void SetParams(const sint32& lambda_ad, const sint32& lambda_census);

	/** \brief 計算初始代價 */
	void Compute();

	/** \brief 獲取初始代價數組指針 */
	float32* get_cost_ptr();

private:
	/** \brief 計算灰度數據 */
	void ComputeGray();

	/** \brief Census變換 */
	void CensusTransform();

	/** \brief 計算代價 */
	void ComputeCost();
private:
	/** \brief 圖像尺寸 */
	sint32	width_;
	sint32	height_;

	/** \brief 影像數據 */
	const uint8* img_left_;
	const uint8* img_right_;

	/** \brief 左影像灰度數據	 */
	vector<uint8> gray_left_;
	/** \brief 右影像灰度數據	 */
	vector<uint8> gray_right_;

	/** \brief 左影像census數組	*/
	vector<uint64> census_left_;
	/** \brief 右影像census數組	*/
	vector<uint64> census_right_;

	/** \brief 初始匹配代價	*/
	vector<float32> cost_init_; //初始由左至右計算完的census結果，大小為width*height*disparity_range

	/** \brief lambda_ad*/
	sint32 lambda_ad_;
	/** \brief lambda_census*/
	sint32 lambda_census_;

	/** \brief 最小視差值 */
	sint32 min_disparity_;
	/** \brief 最大視差值 */
	sint32 max_disparity_;

	/** \brief 是否成功初始化標志	*/
	bool is_initialized_;
};
#endif