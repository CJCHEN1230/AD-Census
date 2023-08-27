/* -*-c++-*- AD-Census - Copyright (C) 2020.
* Author	: Yingsong Li(Ethan Li) <ethan.li.whu@gmail.com>
* https://github.com/ethan-li-coding/AD-Census
* Describe	: header of class CostComputor
*/

#ifndef AD_CENSUS_COST_COMPUTOR_H_
#define AD_CENSUS_COST_COMPUTOR_H_

#include "adcensus_types.h"

/**
 * \brief �N���p�⾹��
 */
class CostComputor {
public:
	CostComputor();
	~CostComputor();

	/**
	 * \brief ��l��
	 * \param width			�v���e
	 * \param height		�v����
	 * \param min_disparity	�̤p���t
	 * \param max_disparity	�̤j���t
	 * \return true: ��l�Ʀ��\
	 */
	bool Initialize(const sint32& width, const sint32& height, const sint32& min_disparity, const sint32& max_disparity);

	/**
	 * \brief �]�m�N���p�⾹���ƾ�
	 * \param img_left		// ���v���ƾڡA�T�q�D
	 * \param img_right		// �k�v���ƾڡA�T�q�D
	 */
	void SetData(const uint8* img_left, const uint8* img_right);

	/**
	 * \brief �]�m�N���p�⾹���Ѽ�
	 * \param lambda_ad		// lambda_ad
	 * \param lambda_census // lambda_census
	 */
	void SetParams(const sint32& lambda_ad, const sint32& lambda_census);

	/** \brief �p���l�N�� */
	void Compute();

	/** \brief �����l�N���Ʋի��w */
	float32* get_cost_ptr();

private:
	/** \brief �p��ǫ׼ƾ� */
	void ComputeGray();

	/** \brief Census�ܴ� */
	void CensusTransform();

	/** \brief �p��N�� */
	void ComputeCost();
private:
	/** \brief �Ϲ��ؤo */
	sint32	width_;
	sint32	height_;

	/** \brief �v���ƾ� */
	const uint8* img_left_;
	const uint8* img_right_;

	/** \brief ���v���ǫ׼ƾ�	 */
	vector<uint8> gray_left_;
	/** \brief �k�v���ǫ׼ƾ�	 */
	vector<uint8> gray_right_;

	/** \brief ���v��census�Ʋ�	*/
	vector<uint64> census_left_;
	/** \brief �k�v��census�Ʋ�	*/
	vector<uint64> census_right_;

	/** \brief ��l�ǰt�N��	*/
	vector<float32> cost_init_; //��l�ѥ��ܥk�p�⧹��census���G�A�j�p��width*height*disparity_range

	/** \brief lambda_ad*/
	sint32 lambda_ad_;
	/** \brief lambda_census*/
	sint32 lambda_census_;

	/** \brief �̤p���t�� */
	sint32 min_disparity_;
	/** \brief �̤j���t�� */
	sint32 max_disparity_;

	/** \brief �O�_���\��l�ƼЧ�	*/
	bool is_initialized_;
};
#endif