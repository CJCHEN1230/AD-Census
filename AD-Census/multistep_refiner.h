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
	 * \brief ��l��
	 * \param width		�v���e
	 * \param height	�v����
	 * \return true:��l�Ʀ��\
	 */
	bool Initialize(const sint32& width, const sint32& height);

	/**
	 * \brief �]�m�h�B�u�ƪ��ƾ�
	 * \param img_left			// ���v���ƾڡA�T�q�D
	 * \param cost				// �N���ƾ�
	 * \param cross_arms		// �Q�r��e�u�ƾ�
	 * \param disp_left			// �����ϵ��t�ƾ�
	 * \param disp_right		// �k���ϵ��t�ƾ�
	 */
	void SetData(const uint8* img_left, float32* cost, const CrossArm* cross_arms, float32* disp_left, float32* disp_right);


	/**
	 * \brief �]�m�h�B�u�ƪ��Ѽ�
	 * \param min_disparity					// �̤p���t
	 * \param max_disparity					// �̤j���t
	 * \param irv_ts						// Iterative Region Voting�Ѽ�ts
	 * \param irv_th						// Iterative Region Voting�Ѽ�th
	 * \param lrcheck_thres					// �@�P���ˬd�H��
	 * \param do_lr_check					// �O�_�ˬd���k�@�P��
	 * \param do_region_voting				// �O�_��������R
	 * \param do_interpolating				// �O�_�����벼��R
	 * \param do_discontinuity_adjustment	// �O�_���D�s��Ͻվ�
	 */
	void SetParam(const sint32& min_disparity, const sint32& max_disparity, const sint32& irv_ts, const float32& irv_th, const float32& lrcheck_thres,
		const bool& do_lr_check, const bool& do_region_voting, const bool& do_interpolating, const bool& do_discontinuity_adjustment);

	/** \brief �h�B���t�u�� */
	void Refine();

private:
	//------4�p�B���t�u��------//
	/** \brief ���s�I�˴� */
	void OutlierDetection();
	/** \brief ���N�����벼 */
	void IterativeRegionVoting();
	/** \brief ������R */
	void ProperInterpolation();
	/** \brief �`�׫D�s��ϵ��t�վ� */
	void DepthDiscontinuityAdjustment();

	/** \brief ���t����t�˴�	 */
	static void EdgeDetect(uint8* edge_mask, const float32* disp_ptr, const sint32& width, const sint32& height, const float32 threshold);
private:
	/** \brief �Ϲ��ؤo */
	sint32	width_;
	sint32	height_;

	/** \brief ���v���ƾڡ]�T�q�D�^ */
	const uint8* img_left_;

	/** \brief �N���ƾ� */
	float32* cost_;
	/** \brief ��e�u�ƾ� */
	const CrossArm* cross_arms_;

	/** \brief �����ϵ��t�ƾ� */
	float* disp_left_;
	/** \brief �k���ϵ��t�ƾ� */
	float* disp_right_;

	/** \brief ��������t�ƾ� */
	vector<uint8> vec_edge_left_;

	/** \brief �̤p���t�� */
	sint32 min_disparity_;
	/** \brief �̤j���t�� */
	sint32 max_disparity_;

	/** \brief Iterative Region Voting�Ѽ�ts */
	sint32	irv_ts_;
	/** \brief Iterative Region Voting�Ѽ�th */
	float32 irv_th_;


	float32 lrcheck_thres_;

	/** \brief �O�_�ˬd���k�@�P�� */
	bool	do_lr_check_;
	/** \brief �O�_�����벼��R */
	bool	do_region_voting_;
	/** \brief �O�_��������R */
	bool	do_interpolating_;
	/** \brief �O�_���D�s��Ͻվ� */
	bool	do_discontinuity_adjustment_;

	/** \brief �B�װϹ�����	*/
	vector<pair<int, int>> occlusions_;
	/** \brief �~�ǰt�Ϲ�����	*/
	vector<pair<int, int>> mismatches_;
};
#endif