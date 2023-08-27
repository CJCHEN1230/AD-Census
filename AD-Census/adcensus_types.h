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

/** \brief 膀娄摸 */
typedef int8_t			sint8;		// Τ才腹8俱计
typedef uint8_t			uint8;		// 礚才腹8俱计
typedef int16_t			sint16;		// Τ才腹16俱计
typedef uint16_t		uint16;		// 礚才腹16俱计
typedef int32_t			sint32;		// Τ才腹32俱计
typedef uint32_t		uint32;		// 礚才腹32俱计
typedef int64_t			sint64;		// Τ才腹64俱计
typedef uint64_t		uint64;		// 礚才腹64俱计
typedef float			float32;	// 虫弘疊翴
typedef double			float64;	// 蛮弘疊翴

/** \brief float32礚 */
constexpr auto Invalid_Float = std::numeric_limits<float32>::infinity();

constexpr auto Large_Float = 99999.0f;
constexpr auto Small_Float = -99999.0f;

/** \brief Census怠へ摸 */
enum CensusSize {
	Census5x5 = 0,
	Census9x7
};

/** \brief ADCensus把计挡篶砰 */
struct ADCensusOption {
	sint32  min_disparity;		// 程跌畉
	sint32	max_disparity;		// 程跌畉

	sint32	lambda_ad;			// 北AD基把计
	sint32	lambda_census;		// 北Census基把计
	sint32	cross_L1;			// ユ怠丁办把计L1
	sint32  cross_L2;			// ユ怠丁办把计L2
	sint32	cross_t1;			// ユ怠肅︹办把计t1
	sint32  cross_t2;			// ユ怠肅︹办把计t2
	float32	so_p1;				// 苯磞絬纔て把计p1
	float32	so_p2;				// 苯磞絬纔て把计p2
	sint32	so_tso;				// 苯磞絬纔て把计tso
	sint32	irv_ts;				// Iterative Region Voting猭把计ts
	float32 irv_th;				// Iterative Region Voting猭把计th

	float32	lrcheck_thres;		// オ璓┦霩

	bool	do_lr_check;					// 琌浪琩オ璓┦
	bool	do_filling;						// 琌暗跌畉恶
	bool	do_discontinuity_adjustment;	// 琌暗獶硈尿跋秸俱

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
* \brief 肅︹挡篶砰
*/
struct ADColor {
	uint8 r, g, b;
	ADColor() : r(0), g(0), b(0) {}
	ADColor(uint8 _b, uint8 _g, uint8 _r) {
		r = _r; g = _g; b = _b;
	}
};

#endif