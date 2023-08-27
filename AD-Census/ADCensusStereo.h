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
	* \brief ������l�ơA�����@�Ǥ��s���w���t�B�Ѽƪ��w�]�m��
	* \param width		��J�A�ֽu����v���e
	* \param height		��J�A�ֽu����v����
	* \param option		��J�A��k�Ѽ�
	*/
	bool Initialize(const sint32& width, const sint32& height, const ADCensusOption& option);

	/**
	* \brief ����ǰt
	* \param img_left	��J�A���v���ƾګ��w�A3�q�D�m��ƾ�
	* \param img_right	��J�A�k�v���ƾګ��w�A3�q�D�m��ƾ�
	* \param disp_left	��X�A���v�����t�ϫ��w�A�w�����t�M�v�����ؤo�����s�Ŷ�
	*/
	bool Match(const uint8* img_left, const uint8* img_right, float32* disp_left);

	/**
	* \brief ���]
	* \param width		��J�A�ֽu����v���e
	* \param height		��J�A�ֽu����v����
	* \param option		��J�A��k�Ѽ�
	*/
	bool Reset(const uint32& width, const uint32& height, const ADCensusOption& option);

private:
	/** \brief �N���p�� */
	void ComputeCost();

	/** \brief �N���E�X */
	void CostAggregation();

	/** \brief ���y�u�u��	 */
	void ScanlineOptimize();

	/** \brief �h�B�J���t�u��	*/
	void MultiStepRefine();

	/** \brief ���t�p��]�����ϡ^*/
	void ComputeDisparity();

	/** \brief ���t�p��]�k���ϡ^*/
	void ComputeDisparityRight();

	/** \brief ���s���� */
	void Release();

private:
	/** \brief ��k�Ѽ� */
	ADCensusOption option_;

	/** \brief �v���e */
	sint32 width_;
	/** \brief �v���� */
	sint32 height_;

	/** \brief ���v���ƾڡA3�q�D�m��ƾ� */
	const uint8* img_left_;
	/** \brief �k�v���ƾ�	�A3�q�D�m��ƾ� */
	const uint8* img_right_;

	/** \brief �N���p�⾹ */
	CostComputor cost_computer_;
	/** \brief �N���E�X�� */
	CrossAggregator aggregator_;
	/** \brief ���y�u�u�ƾ� */
	ScanlineOptimizer scan_line_;
	/** \brief �h�B�u�ƾ� */
	MultiStepRefiner refiner_;

	/** \brief ���v�����t�� */
	float32* disp_left_;
	/** \brief �k�v�����t�� */
	float32* disp_right_;

	/** \brief �O�_��l�ƼЧ�	*/
	bool is_initialized_;
};