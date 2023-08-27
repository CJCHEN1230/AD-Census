/* -*-c++-*- AD-Census - Copyright (C) 2020.
* Author	: Yingsong Li(Ethan Li) <ethan.li.whu@gmail.com>
* https://github.com/ethan-li-coding/AD-Census
* Describe	: implement of class MultiStepRefiner
*/

#ifndef AD_CENSUS_MULTISTEP_REFINER_H_
#define AD_CENSUS_MULTISTEP_REFINER_H_

#include "adcensus_types.h"
#include "cross_aggregator.h"

class MultiStepRefiner
{
public:
	MultiStepRefiner();
	~MultiStepRefiner();

	/**
	 * \brief 初始化
	 * \param width		影像寬
	 * \param height	影像高
	 * \return true:初始化成功
	 */
	bool Initialize(const sint32& width, const sint32& height);

	/**
	 * \brief 設置多步優化的數據
	 * \param img_left			// 左影像數據，三通道
	 * \param cost				// 代價數據
	 * \param cross_arms		// 十字交叉臂數據
	 * \param disp_left			// 左視圖視差數據
	 * \param disp_right		// 右視圖視差數據
	 */
	void SetData(const uint8* img_left, float32* cost, const CrossArm* cross_arms, float32* disp_left, float32* disp_right);


	/**
	 * \brief 設置多步優化的參數
	 * \param min_disparity					// 最小視差
	 * \param max_disparity					// 最大視差
	 * \param irv_ts						// Iterative Region Voting參數ts
	 * \param irv_th						// Iterative Region Voting參數th
	 * \param lrcheck_thres					// 一致性檢查閾值
	 * \param do_lr_check					// 是否檢查左右一致性
	 * \param do_region_voting				// 是否做內插填充
	 * \param do_interpolating				// 是否局部投票填充
	 * \param do_discontinuity_adjustment	// 是否做非連續區調整
	 */
	void SetParam(const sint32& min_disparity, const sint32& max_disparity, const sint32& irv_ts, const float32& irv_th, const float32& lrcheck_thres,
		const bool& do_lr_check, const bool& do_region_voting, const bool& do_interpolating, const bool& do_discontinuity_adjustment);

	/** \brief 多步視差優化 */
	void Refine();

private:
	//------4小步視差優化------//
	/** \brief 離群點檢測 */
	void OutlierDetection();
	/** \brief 迭代局部投票 */
	void IterativeRegionVoting();
	/** \brief 內插填充 */
	void ProperInterpolation();
	/** \brief 深度非連續區視差調整 */
	void DepthDiscontinuityAdjustment();

	/** \brief 視差圖邊緣檢測	 */
	static void EdgeDetect(uint8* edge_mask, const float32* disp_ptr, const sint32& width, const sint32& height, const float32 threshold);
private:
	/** \brief 圖像尺寸 */
	sint32	width_;
	sint32	height_;

	/** \brief 左影像數據（三通道） */
	const uint8* img_left_;

	/** \brief 代價數據 */
	float32* cost_;
	/** \brief 交叉臂數據 */
	const CrossArm* cross_arms_;

	/** \brief 左視圖視差數據 */
	float* disp_left_;
	/** \brief 右視圖視差數據 */
	float* disp_right_;

	/** \brief 左視圖邊緣數據 */
	vector<uint8> vec_edge_left_;

	/** \brief 最小視差值 */
	sint32 min_disparity_;
	/** \brief 最大視差值 */
	sint32 max_disparity_;

	/** \brief Iterative Region Voting參數ts */
	sint32	irv_ts_;
	/** \brief Iterative Region Voting參數th */
	float32 irv_th_;


	float32 lrcheck_thres_;

	/** \brief 是否檢查左右一致性 */
	bool	do_lr_check_;
	/** \brief 是否局部投票填充 */
	bool	do_region_voting_;
	/** \brief 是否做內插填充 */
	bool	do_interpolating_;
	/** \brief 是否做非連續區調整 */
	bool	do_discontinuity_adjustment_;

	/** \brief 遮擋區像素集	*/
	vector<pair<int, int>> occlusions_;
	/** \brief 誤匹配區像素集	*/
	vector<pair<int, int>> mismatches_;
};
#endif