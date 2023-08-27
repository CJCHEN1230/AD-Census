/* -*-c++-*- AD-Census - Copyright (C) 2020.
* Author	: Yingsong Li(Ethan Li) <ethan.li.whu@gmail.com>
*			  https://github.com/ethan-li-coding/AD-Census
* Describe	: header of ad-census stereo class
*/
#pragma once

#include "adcensus_types.h"
#include "cost_computor.h"
#include "cross_aggregator.h"
#include "scanline_optimizer.h"
#include "multistep_refiner.h"

class ADCensusStereo {
public:
	ADCensusStereo();
	~ADCensusStereo();

	/**
	* \brief 類的初始化，完成一些內存的預分配、參數的預設置等
	* \param width		輸入，核線像對影像寬
	* \param height		輸入，核線像對影像高
	* \param option		輸入，算法參數
	*/
	bool Initialize(const sint32& width, const sint32& height, const ADCensusOption& option);

	/**
	* \brief 執行匹配
	* \param img_left	輸入，左影像數據指針，3通道彩色數據
	* \param img_right	輸入，右影像數據指針，3通道彩色數據
	* \param disp_left	輸出，左影像視差圖指針，預先分配和影像等尺寸的內存空間
	*/
	bool Match(const uint8* img_left, const uint8* img_right, float32* disp_left);

	/**
	* \brief 重設
	* \param width		輸入，核線像對影像寬
	* \param height		輸入，核線像對影像高
	* \param option		輸入，算法參數
	*/
	bool Reset(const uint32& width, const uint32& height, const ADCensusOption& option);

private:
	/** \brief 代價計算 */
	void ComputeCost();

	/** \brief 代價聚合 */
	void CostAggregation();

	/** \brief 掃描線優化	 */
	void ScanlineOptimize();

	/** \brief 多步驟視差優化	*/
	void MultiStepRefine();

	/** \brief 視差計算（左視圖）*/
	void ComputeDisparity();

	/** \brief 視差計算（右視圖）*/
	void ComputeDisparityRight();

	/** \brief 內存釋放 */
	void Release();

private:
	/** \brief 算法參數 */
	ADCensusOption option_;

	/** \brief 影像寬 */
	sint32 width_;
	/** \brief 影像高 */
	sint32 height_;

	/** \brief 左影像數據，3通道彩色數據 */
	const uint8* img_left_;
	/** \brief 右影像數據	，3通道彩色數據 */
	const uint8* img_right_;

	/** \brief 代價計算器 */
	CostComputor cost_computer_;
	/** \brief 代價聚合器 */
	CrossAggregator aggregator_;
	/** \brief 掃描線優化器 */
	ScanlineOptimizer scan_line_;
	/** \brief 多步優化器 */
	MultiStepRefiner refiner_;

	/** \brief 左影像視差圖 */
	float32* disp_left_;
	/** \brief 右影像視差圖 */
	float32* disp_right_;

	/** \brief 是否初始化標志	*/
	bool is_initialized_;
};