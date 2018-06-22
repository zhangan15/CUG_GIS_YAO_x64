bool accuracyEvaluation(alglib::real_1d_array realval, alglib::real_1d_array simval, double& r_2, double& rmse, double& rela_rmse, double& pearson_r,
	double& t_p_left, double& t_p_right, double& t_p_value, double& f_p_left, double& f_p_right, double& f_p_value, double& mae, double& rela_mae)
{
	if (realval.length() != simval.length() || realval.length() == 0)
	{
		std::cout << "evaluate data maybe 0 or unequal length. " << std::endl;
		return false;
	}

	//calculate pearson r
	pearson_r = alglib::pearsoncorr2(realval, simval);
	std::cout << "pearson r = " << pearson_r << std::endl;

	//calculate p value of t test
	alglib::unequalvariancettest(realval, realval.length(), simval, simval.length(), t_p_value, t_p_left, t_p_right);
	// global_ilog(QString("left p value = %1").arg(p_left, 0, 'f', 6));
	// global_ilog(QString("right p value = %1").arg(p_right, 0, 'f', 6));
	// global_ilog(QString("p value = %1").arg(p_value, 0, 'f', 6));
	alglib::ftest(realval, realval.length(), simval, simval.length(), f_p_value, f_p_left, f_p_right);

	std::cout << "t_p_value = " << t_p_value << ", t_p_left = " << t_p_left << ", t_p_right = " << t_p_right << std::endl;
	std::cout << "f_p_value = " << f_p_value << ", f_p_left = " << f_p_left << ", f_p_right = " << f_p_right << std::endl;


	//calculate r2 and rmse
	int i;
	double ave_observe = 0;
	for (i = 0; i < realval.length(); i++)
	{
		ave_observe += realval[i];
	}
	ave_observe /= (double)realval.length();

	//
	double m1 = 0, m2 = 0;
	for (i = 0; i < realval.length(); i++)
	{
		m1 += (realval[i] - simval[i])*(realval[i] - simval[i]);
		m2 += (realval[i] - ave_observe)*(realval[i] - ave_observe);
	}

	r_2 = 1 - m1 / m2;
	rmse = sqrtf(m1 / (double)realval.length());
	rela_rmse = rmse / ave_observe;

	std::cout << "r_2 = " << r_2 << std::endl;
	std::cout << "rmse = " << rmse << std::endl;

	// global_ilog(QString("standard r_2 = %1").arg(r_2, 0, 'f', 6));
	// global_ilog(QString("rmse = %1").arg(rmse, 0, 'f', 6));

	//calculate MAE (mean absolute error)
	mae = 0;
	rela_mae = 0;
	for (int i = 0; i < realval.length(); i++)
	{
		double delta = realval[i] - simval[i];
		if (delta < 0)
			delta = -delta;

		mae += delta;
		rela_mae = rela_mae + delta / realval[i];
	}
	mae = mae / (double)realval.length();
	rela_mae = rela_mae / (double)realval.length();

	std::cout << "mae = " << mae << std::endl;

	return true;
}