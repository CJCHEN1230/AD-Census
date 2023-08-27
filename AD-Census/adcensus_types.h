/* -*-c++-*- AD-Census - Copyright (C) 2020.
* Author	: Yingsong Li(Ethan Li) <ethan.li.whu@gmail.com>
*			  https://github.com/ethan-li-coding/AD-Census
* Describe	: header of adcensus_types
*/

#ifndef ADCENSUS_STEREO_TYPES_H_
#define ADCENSUS_STEREO_TYPES_H_

#include <cstdint>
#include <limits>
#include <vector>
using std::vector;
using std::pair;

#ifndef SAFE_DELETE
#define SAFE_DELETE(P) {if(P) delete[](P);(P)=nullptr;}
#endif

/** \brief ��¦�����O�W */
typedef int8_t			sint8;		// ���Ÿ�8����
typedef uint8_t			uint8;		// �L�Ÿ�8����
typedef int16_t			sint16;		// ���Ÿ�16����
typedef uint16_t		uint16;		// �L�Ÿ�16����
typedef int32_t			sint32;		// ���Ÿ�32����
typedef uint32_t		uint32;		// �L�Ÿ�32����
typedef int64_t			sint64;		// ���Ÿ�64����
typedef uint64_t		uint64;		// �L�Ÿ�64����
typedef float			float32;	// ���ׯB�I
typedef double			float64;	// ����ׯB�I

/** \brief float32�L�ĭ� */
constexpr auto Invalid_Float = std::numeric_limits<float32>::infinity();

constexpr auto Large_Float = 99999.0f;
constexpr auto Small_Float = -99999.0f;

/** \brief Census���f�ؤo���� */
enum CensusSize {
	Census5x5 = 0,
	Census9x7
};

/** \brief ADCensus�ѼƵ��c�� */
struct ADCensusOption {
	sint32  min_disparity;		// �̤p���t
	sint32	max_disparity;		// �̤j���t

	sint32	lambda_ad;			// ����AD�N���Ȫ��Ѽ�
	sint32	lambda_census;		// ����Census�N���Ȫ��Ѽ�
	sint32	cross_L1;			// �Q�r��e���f���Ŷ���ѼơGL1
	sint32  cross_L2;			// �Q�r��e���f���Ŷ���ѼơGL2
	sint32	cross_t1;			// �Q�r��e���f���C���ѼơGt1
	sint32  cross_t2;			// �Q�r��e���f���C���ѼơGt2
	float32	so_p1;				// ���y�u�u�ưѼ�p1
	float32	so_p2;				// ���y�u�u�ưѼ�p2
	sint32	so_tso;				// ���y�u�u�ưѼ�tso
	sint32	irv_ts;				// Iterative Region Voting�k�Ѽ�ts
	float32 irv_th;				// Iterative Region Voting�k�Ѽ�th

	float32	lrcheck_thres;		// ���k�@�P�ʬ����H��

	bool	do_lr_check;					// �O�_�ˬd���k�@�P��
	bool	do_filling;						// �O�_�����t��R
	bool	do_discontinuity_adjustment;	// �O�_���D�s��Ͻվ�

	ADCensusOption() : min_disparity(0), max_disparity(64),
		lambda_ad(10), lambda_census(30),
		cross_L1(34), cross_L2(17),
		cross_t1(20), cross_t2(6),
		so_p1(1.0f), so_p2(3.0f),
		so_tso(15), irv_ts(20), irv_th(0.4f),
		lrcheck_thres(1.0f),
		do_lr_check(true), do_filling(true), do_discontinuity_adjustment(false) {};
};

/**
* \brief �C�⵲�c��
*/
struct ADColor {
	uint8 r, g, b;
	ADColor() : r(0), g(0), b(0) {}
	ADColor(uint8 _b, uint8 _g, uint8 _r) {
		r = _r; g = _g; b = _b;
	}
};

#endif