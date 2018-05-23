#ifndef  CLASS_ALGLIB_RF_20150923AABB
#define  CLASS_ALGLIB_RF_20150923AABB



#include "alglib/statistics.h"
#include "alglib/dataanalysis.h"
#include "alglib/alglibmisc.h"



#define RF_CLASS 0
#define RF_INTER 1

namespace CRFWeights
{
// 	bool calRFWeights_CLASS(alglib::real_2d_array dataset, alglib::decisionforest *pDF, alglib::real_1d_array RFWeights);
// 	bool calRFWeights_INTER(alglib::real_2d_array dataset, alglib::decisionforest *pDF, alglib::real_1d_array RFWeights);
	bool calRFWeights(alglib::real_2d_array dataset, alglib::decisionforest *pDF, alglib::real_1d_array& RFWeights, int rf_type);
};
#endif