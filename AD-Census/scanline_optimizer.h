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
 * \brief ���y�u�u�ƾ�
 */
class ScanlineOptimizer {
public:
	ScanlineOptimizer();
	~ScanlineOptimizer();


	/**
	 * \brief �]�m�ƾ�
	 * \param img_left		// ���v���ƾڡA�T�q�D
	 * \param img_right 	// �k�v���ƾڡA�T�q�D
	 * \param cost_init 	// ��l�N���Ʋ�
	 * \param cost_aggr 	// �E�X�N���Ʋ�
	 */
	void SetData(const uint8* img_left, const uint8* img_right, float32* cost_init, float32* cost_aggr);

	/**
	 * \brief
	 * \param width			// �v���e
	 * \param height		// �v����
	 * \param min_disparity	// �̤p���t
	 * \param max_disparity // �̤j���t
	 * \param p1			// p1
	 * \param p2			// p2
	 * \param tso			// tso
	 */
	void SetParam(const sint32& width, const sint32& height, const sint32& min_disparity, const sint32& max_disparity, const float32& p1, const float32& p2, const sint32& tso);

	/**
	 * \brief �u�� */
	void Optimize();

private:
	/**
	* \brief ���k���|�u�� �� ��
	* \param cost_so_src		��J�ASO�e�N���ƾ�
	* \param cost_so_dst		��X�ASO��N���ƾ�
	* \param is_forward			��J�A�O�_������V�]����V���q����k�A�Ϥ�V���q�k�쥪�^
	*/
	void ScanlineOptimizeLeftRight(const float32* cost_so_src, float32* cost_so_dst, bool is_forward = true);

	/**
	* \brief �W�U���|�u�� �� ��
	* \param cost_so_src		��J�ASO�e�N���ƾ�
	* \param cost_so_dst		��X�ASO��N���ƾ�
	* \param is_forward			��J�A�O�_������V�]����V���q�W��U�A�Ϥ�V���q�U��W�^
	*/
	void ScanlineOptimizeUpDown(const float32* cost_so_src, float32* cost_so_dst, bool is_forward = true);

	/** \brief �p���C��Z�� */
	inline sint32 ColorDist(const ADColor& c1, const ADColor& c2) {
		return std::max(abs(c1.r - c2.r), std::max(abs(c1.g - c2.g), abs(c1.b - c2.b)));
	}

private:
	/** \brief �Ϲ��ؤo */
	sint32	width_;
	sint32	height_;

	/** \brief �v���ƾ� */
	const uint8* img_left_;
	const uint8* img_right_;

	/** \brief ��l�N���Ʋ� */
	float32* cost_init_;
	/** \brief �E�X�N���Ʋ� */
	float32* cost_aggr_;

	/** \brief �̤p���t�� */
	sint32 min_disparity_;
	/** \brief �̤j���t�� */
	sint32 max_disparity_;
	/** \brief ��l��p1�� */
	float32 so_p1_;
	/** \brief ��l��p2�� */
	float32 so_p2_;
	/** \brief tso�H�� */
	sint32 so_tso_;
};
#endif