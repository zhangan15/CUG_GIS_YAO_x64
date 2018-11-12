/********************************************************************
* Logistic Regression Classifier V0.11
* Implemented by Rui Xia(rxiacn@gmail.com) , Wang Tao(linclonwang@163.com)
* Last updated on 2012-10-25. 
*********************************************************************/
#ifndef GEO_LOGIC_REGRESSION
#define GEO_LOGIC_REGRESSION

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <climits>
#include <math.h>
#include <time.h>

# define VERSION       "V0.11"
# define VERSION_DATE  "2012-10-24"

using namespace std;

const long double LOG_LIM = 1e-300;

struct sparse_feat
{
	vector<int> id_vec;
	vector<float> value_vec;
};

class LR 
{
private:
	vector<sparse_feat> samp_feat_vec;
    vector<int> samp_class_vec;
	int feat_set_size;
	int class_set_size;
	vector< vector<float> > omega;
     
public:
    LR();
    ~LR();
	void save_model(string model_file);
    bool load_model(string model_file);
	void load_training_file(string training_file);
	bool load_ca_points_file(char* ca_points_file);
	void init_omega();
	
    float train_sgd(int max_loop, double loss_thrd, float learn_rate, float lambda, int avg);
	vector<float> calc_score(sparse_feat &samp_feat);
	vector<float> score_to_prb(vector<float> &score);
    int score_to_class(vector<float> &score);
    
	float classify_testing_file(string testing_file, string output_file, int output_format);
	bool classify_data(vector<float> inputdata, vector<float>& prob);

private:
	void read_samp_file(string samp_file, vector<sparse_feat> &samp_feat_vec, vector<int> &samp_class_vec);
	void update_online(int samp_class, sparse_feat &samp_feat, float learn_rate, float lambda);
	void calc_loss(double *loss, float *acc);
	float calc_acc(vector<int> &test_class_vec, vector<int> &pred_class_vec);	
	float sigmoid(float x);
	vector<float> softmax(vector<float> &score_vec);
	vector<string> string_split(string terms_str, string spliting_tag);

	//
	bool read_ca_points_file(char* samp_file, vector<sparse_feat> &samp_feat_vec, vector<int> &samp_class_vec);

};

#endif


