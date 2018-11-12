//#define XML_CONFIG		"F:/code/haplib_vs2010/GeoInversionApp/configure.xml"
#define LOG_MODULE		"SuperComputerCA"

//定义输入输出文件路径
#define NODE_FILEPATH_LANDCOVER		"Data/DataSource/SourceFilepath/LandCoverFilepath"
#define NODE_FILEPATH_AUXILIARY		"Data/DataSource/SourceFilepath/AuxiliaryDataFilepath"
#define NODE_FILEPATH_OUTPUT		"Data/DataSource/SourceFilepath/OutputFilepath"

//定义数据位置
#define NODE_DEM			"Data/DataSource/DEM"
#define NODE_LANDCOVER		"Data/DataSource/LandCoverFiles"
#define NODE_LANDCOVER_CODE	"Data/DataSource/LandCoverFiles/ClassifiCode"
#define NODE_POPULATION		"Data/DataSource/Population"

//定义增加概率位置
#define NODE_BOOL_ADD_PROB	"Data/DataSource/AddProbImageFile/IsUseAddProb"
#define NODE_ADD_PRON_FILE	"Data/DataSource/AddProbImageFile/Filepath"

//定义辅助数据数目
#define NUM_AUXILIARY_DATA	5

//辅助数据
#define NODE_AUXLIARY_DATA	"Data/DataSource/AuxiliaryDataFiles"		//version 2.0

//定义辅助数据位置
#define NODE_DISTANCE_CAPITAN_CITY		"Data/DataSource/AuxiliaryDataFile/ChinaData/CapitanCityDistance"
#define NODE_DISTANCE_PREFECTURE_CITY	"Data/DataSource/AuxiliaryDataFile/ChinaData/PrefectureCityDistance"
#define NODE_DISTANCE_COUNTY_TOWN		"Data/DataSource/AuxiliaryDataFile/ChinaData/CountyTownDistance"
#define NODE_DISTANCE_MAIN_ROAD			"Data/DataSource/AuxiliaryDataFile/ChinaData/MainRoadDistance"
#define NODE_DISTANCE_MAIN_RAILWAY		"Data/DataSource/AuxiliaryDataFile/ChinaData/MainRailwayDistance"

//定义模拟数据开始年、结束年和Gap
#define NODE_YEAR_OF_DATA_START		"Data/Parameters/Simulation/StartYear"
#define NODE_YEAR_OF_DATA_END		"Data/Parameters/Simulation/EndYear"
#define NODE_YEAR_OF_DATA_GAP		"Data/Parameters/Simulation/YearGap"

//定义其他参数
#define NODE_RANDOM_POINTS_NUM		"Data/Parameters/Simulation/RandomPointsNum"
#define RANDOM_POINTS_PARAM			2	//取(n*倍数)的点然后再后处理
#define NODE_PARAM_PROBABILITY_PARAM	"Data/Parameters/Simulation/ProbabilityType"
#define NODE_MAX_THREADS_NUM			"Data/Parameters/Simulation/MaxThreadNum"

//输出数据
#define NODE_OUTPUT_LOG_DIR_PATH							"Data/DataSource/SourceFilepath/OutputFilepath"
#define NODE_OUTPUT_URBAN_CHANGE_FILENAME					"Data/DataSource/OutputFiles/UrbanChangeFilepath"
#define NODE_OUTPUT_LAND_DEVELOP_SUITABLE_INDEX_FILENAME	"Data/DataSource/OutputFiles/LandDevelopSuitableIndexFilepath"
#define NODE_OUTPUT_REGRESSION_ANALYSIS_POINTS_PATH			"Data/DataSource/OutputFiles/RegressionAnalysisPointsFile"
#define NODE_OUTPUT_PROBABILTY_DEVELOPED_FILE_PATH			"Data/DataSource/OutputFiles/ProbabilityDevelopedAllFilepath"

#define NODE_OUTOUT_URBAN_CHANGE_RECLASSIFI_FILEPATH		"Data/DataSource/OutputFiles/UrbanReclassifiFilepath"

#define NODE_OUTPUT_URBAN_DEVELOPMENT_CA_RESULT				"Data/DataSource/OutputFiles/UrbanDevelopmentCAIterationsResult"

#define NODE_IS_OUTPUT_9BANDS_PROB_IMG						"Data/DataSource/OutputFiles/IsOutput9BandsProbabiltyImage"
#define NODE_OUTPUT_9BANDS_PROBIMG_FILEPATH					"Data/DataSource/OutputFiles/Op9BandsProbabilityFilepath"

//验证数据
#define NODE_OUTOUT_URBAN_CHANGE_RECLASSIFI_VALIDATION_FILEPATH		"Data/DataSource/OutputFiles/UrbanReclassifiFilepath_Validation"

//定义城市转换类型数目
#define  NUM_CODE_URBAN_CHANGE		10
//定义城市转换类型
#define CODE_UNKOWN_CHANGE					0					//无效数值
#define CODE_NOCITY_TO_CITY					1					//非城市到城市
#define CODE_NOCITY_TO_NOCITY				2					//非城市到非城市
#define CODE_NOCITY_TO_PROTECTIVE_AREA		3					//非城市到保护区
#define CODE_CITY_TO_CITY					4					//城市到城市
#define CODE_CITY_TO_NOCITY					5					//城市到非城市
#define CODE_CITY_TO_PROTECTIVE_AREA		6					//城市到保护区
#define CODE_PROTECTIVE_AREA_TO_CITY		7					//保护区域到城市
#define CODE_PROTECTIVE_AREA_TO_NOCITY		8					//保护区域到非城市
#define CODE_PROTECTIVE_AREA_UNCHANGE		9					//保护区域未改变

//定义城市发展适宜性指数
#define INDEX_DEVELOP_UNABLE	0	//不允许发展为城市
#define INDEX_DEVELOP_ABLE		1	//允许发展为城市
#define INDEX_PROTECTIVE		2	//限制发展区域

//定义城市土地利用类型
#define RECLASS_UNKNOWN		0	//未知
#define RECLASS_CITY		1	//城市
#define RECLASS_UNCITY		2	//非城市
#define RELCASS_PROTECTIVE	3	//限制发展用地


//耕地转换数据
#define FA_UNKNOWN				0	//未知
#define FA_FARMLAND_CHANGE		1	//耕地转变为城镇用地
#define FA_FARMLAND_UNCHANGE	2	//耕地未转变


//定义数据类型
#define LandCoverDT	unsigned char	//土地覆盖和城市转换图像数据类型
#define LandCoverDTCode 1

#define UrbanReclassDT	unsigned char	//土地重分类图像数据类型
#define UrbanReclassDTCode 1

#define LandSuitabilityDT unsigned char	//城市发展适宜性指数数据类型
#define LandSuitabilityDTCode 1

#define DistAuxiDT	float	//距离图像数据类型
#define DistAuxiDTCode 4

#define ProbabilityDT float	//概率图像数据类型
#define ProbabilityDTCode 4

#define CAResultDT UrbanReclassDT	//CA结果，和UrbanReclassDT一致
#define CAResultDTCode UrbanReclassDTCode

#define AnalysisDT LandCoverDT		//分析使用的数据类型
#define AnalysisDTCode LandCoverDTCode

//分类器类型
#define CLASSIFIER_MODE				"Data/Parameters/Simulation/RegressionMethod"

//随机森林参数
#define DF_PARAMETER_TREES_NUM		"Data/Parameters/RandomDecisionForest/DecisionForestTrees"
#define DF_PARAMETER_SET_PERSENT	"Data/Parameters/RandomDecisionForest/DecisionForestSetPersent"
#define DF_PARAMETER_BEST_SPLIT		"Data/Parameters/RandomDecisionForest/DecisionForestNRndVars"
#define DF_OUTPUT_RESULT_DATA		"Data/Parameters/RandomDecisionForest/DecisionForestResultData"

//逻辑回归参数
#define LR_MODEL_FILEPATH			"Data/Parameters/LogicRegression/ModelFilepath"
#define LR_PARAMETER_MAX_LOOP		"Data/Parameters/LogicRegression/MaxLoop"
#define LR_PARAMETER_LOSS_THREAD	"Data/Parameters/LogicRegression/LossThread"
#define LR_PARAMETER_LEARN_RATE		"Data/Parameters/LogicRegression/LearnRate"
#define LR_PARAMETER_LAMBDA			"Data/Parameters/LogicRegression/Lambda"
#define LR_PARAMETER_AVG_WEIGHT		"Data/Parameters/LogicRegression/AvgLoopWeight"
#define LR_PARAMETER_UPDATE_MODEL	"Data/Parameters/LogicRegression/UpdateModelFile"
#define LR_PARAMETER_PASS_ACCU		"Data/Parameters/LogicRegression/MinAccuracyToPass"

//神经网络参数
#define NN_PARAMETER_HIDDEN_NUM		"Data/Parameters/NeuralNetwork/HiddenLevelNum"
#define NN_PARAMETER_RESTART_NUM	"Data/Parameters/NeuralNetwork/RestartsNum"
#define NN_PARAMETER_ACCU_REPORT	"Data/Parameters/NeuralNetwork/AccuracyReport"

//免疫算法参数
#define AG_PARAMETER_CLASSIFIER_ORDER	"Data/Parameters/ImmuneAlgorithm/ClassifierOrder"
#define AG_PARAMETER_ANTIBODIES_NUM		"Data/Parameters/ImmuneAlgorithm/AntibodiesNumber"
#define AG_PARAMETER_VARIATION_PROB		"Data/Parameters/ImmuneAlgorithm/VariationPg"
#define AG_PARAMETER_EXCHANGE_PROB		"Data/Parameters/ImmuneAlgorithm/ExchangePg"
#define AG_PARAMETER_ITERATION_NUM		"Data/Parameters/ImmuneAlgorithm/TrainingIterationNum"
#define AG_PARAMETER_ACCU_REPORT		"Data/Parameters/ImmuneAlgorithm/AccuracyReport"

//CA迭代参数
#define CA_PARAMTER_MAX_ITER_NUM				"Data/Parameters/CAIteration/MaxIterationNum"
#define CA_PARAMETER_FORE_NEXT_DIFFERENCE_NUM	"Data/Parameters/CAIteration/ForeNextDifferenceNum"
#define CA_PARAMETER_DEVELOP_MIN_PROBABILITY	"Data/Parameters/CAIteration/UrbanDevelopedProbability"
#define CA_PARAMETER_IS_SIMULATE_PROTAREA		"Data/Parameters/CAIteration/IsSimulateProtectiveArea"
#define CA_PARAMETER_IS_NEIGHBORCAL_USEPROB		"Data/Parameters/CAIteration/IsUseProbImgCalNeighborProb"
#define CA_PARAMETER_IS_USE_FORE_PROBIMG		"Data/Parameters/CAIteration/IsUseForeProbImgToIteration"
#define CA_PARAMETER_URBAN_INCREASE_NUM			"Data/Parameters/CAIteration/UrbanIncreaseNum"
#define CA_PARAMETER_URBAN_INCREASE_FAST_METHOD	"Data/Parameters/CAIteration/UrbanIncreaseFastMethod"
#define CA_PARAMETER_FIND_RNDPTS_RESTART_TIMES	"Data/Parameters/CAIteration/FindRndPtsRestartTimes"

//耕地分析转变参数
#define FA_SRC_CLASSIMG_FILENAME		"Data/Parameters/FarmlandAnalysis/ClassifiImageFilename"
#define FA_FRAMLAND_CLASSI_CODE			"Data/Parameters/FarmlandAnalysis/FarmlandCode"
#define FA_CA_RESULT_FILES				"Data/Parameters/FarmlandAnalysis/CAResultFiles"
#define FA_OUTPUT_RESULT_FILES			"Data/Parameters/FarmlandAnalysis/AnalysisResultFiles"
#define FA_VALID_PROCESS				"Data/Parameters/FarmlandAnalysis/ValidProcess"

//精度评价参数
#define AE_CA_RESULT_FILES				"Data/Parameters/AccuracyEvaluation/CAResultFiles"
#define AE_OUTPUT_RESULT_FILES			"Data/Parameters/AccuracyEvaluation/AccuResultFiles"
#define AE_MAP_VECTOR_FILE				"Data/Parameters/AccuracyEvaluation/MapFile"
#define AE_SRC_RECLASS_FILE				"Data/Parameters/AccuracyEvaluation/SourceUrbanReclassifyFile"
#define AE_DST_RECLASS_FILE				"Data/Parameters/AccuracyEvaluation/TargetUrbanReclassifyFile"
#define AE_VALID_PROCESS				"Data/Parameters/AccuracyEvaluation/ValidProcess"