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
* \brief ��e�Q�r�u���c
* ���F����s�e�ΡA�u�������]�m��uint8�A�o�N�����u���̪�����W�L255
*/
struct CrossArm {
	uint8 left, right, top, bottom;
	CrossArm() : left(0), right(0), top(0), bottom(0) { }
};
/**\brief �̤j�u�� */
#define MAX_ARM_LENGTH 255 

/**
 * \brief �Q�r��e��N���E�X��
 */
class CrossAggregator {
public:
	CrossAggregator();
	~CrossAggregator();

	/**
	 * \brief ��l�ƥN���E�X��
	 * \param width		�v���e
	 * \param height	�v����
	 * \return true:��l�Ʀ��\
	 */
	bool Initialize(const sint32& width, const sint32& height, const sint32& min_disparity, const sint32& max_disparity);

	/**
	 * \brief �]�m�N���E�X�����ƾ�
	 * \param img_left		// ���v���ƾڡA�T�q�D
	 * \param img_right		// �k�v���ƾڡA�T�q�D
	 * \param cost_init		// ��l�N���Ʋ�
	 */
	void SetData(const uint8* img_left, const uint8* img_right, const float32* cost_init);

	/**
	 * \brief �]�m�N���E�X�����Ѽ�
	 * \param cross_L1		// L1
	 * \param cross_L2		// L2
	 * \param cross_t1		// t1
	 * \param cross_t2		// t2
	 */
	void SetParams(const sint32& cross_L1, const sint32& cross_L2, const sint32& cross_t1, const sint32& cross_t2);

	/** \brief �E�X */
	void Aggregate(const sint32& num_iters);

	/** \brief ����Ҧ��������Q�r��e�u�ƾګ��w */
	CrossArm* get_arms_ptr();

	/** \brief ����E�X�N���Ʋի��w */
	float32* get_cost_ptr();
private:
	/** \brief �c�ؤQ�r��e�u */
	void BuildArms();
	/** \brief �j�������u */
	void FindHorizontalArm(const sint32& x, const sint32& y, uint8& left, uint8& right) const;
	/** \brief �j���ݪ��u */
	void FindVerticalArm(const sint32& x, const sint32& y, uint8& top, uint8& bottom) const;
	/** \brief �p�⹳��������Ϲ����ƶq */
	void ComputeSupPixelCount();
	/** \brief �E�X�Y�ӵ��t */
	void AggregateInArms(const sint32& disparity, const bool& horizontal_first);

	/** \brief �p���C��Z�� */
	inline sint32 ColorDist(const ADColor& c1, const ADColor& c2) const {
		return std::max(abs(c1.r - c2.r), std::max(abs(c1.g - c2.g), abs(c1.b - c2.b)));
	}
private:
	/** \brief �Ϲ��ؤo */
	sint32	width_;
	sint32	height_;

	/** \brief ��e�u */
	vector<CrossArm> vec_cross_arms_;

	/** \brief �v���ƾ� */
	const uint8* img_left_;
	const uint8* img_right_;

	/** \brief ��l�N���Ʋի��w */
	const float32* cost_init_;
	/** \brief �E�X�N���Ʋ� */
	vector<float32> cost_aggr_;

	/** \brief �{�ɥN���ƾ� */
	vector<float32> vec_cost_tmp_[2];
	/** \brief �x�s����Ϲ����ƶq�Ʋ� 0�G�����u�u�� 1�G�ݪ��u�u�� */
	vector<uint16> vec_sup_count_[2];
	vector<uint16> vec_sup_count_tmp_;

	sint32	cross_L1_;			// �Q�r��e���f���Ŷ���ѼơGL1
	sint32  cross_L2_;			// �Q�r��e���f���Ŷ���ѼơGL2
	sint32	cross_t1_;			// �Q�r��e���f���C���ѼơGt1
	sint32  cross_t2_;			// �Q�r��e���f���C���ѼơGt2
	sint32  min_disparity_;			// �̤p���t
	sint32	max_disparity_;			// �̤j���t

	/** \brief �O�_���\��l�ƼЧ�	*/
	bool is_initialized_;
};
#endif