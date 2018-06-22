# -*- coding: utf-8 -*-
"""
Created on Thu Jun 21 16:08:31 2018

@author: BigPlato
"""

import numpy as np
import xalglib
import math

#Pearson correlation coefficient, t test, f test, r2, rmse and mae

# Pearson product-moment correlation coefficient
# 
# Input parameters:
#     X       -   sample 1 (array indexes: [0..N-1])
#     Y       -   sample 2 (array indexes: [0..N-1])
#     N       -   N>=0, sample size:
#                 * if given, only N leading elements of X/Y are processed
#                 * if not given, automatically determined from input sizes
# 
# Result:
#     Pearson product-moment correlation coefficient
#     (zero for N=0 or N=1)
realval = [1,2,3,4,5,6,7,8,9,0]
simval = [2,2,3,4,6,6,7,8,8,9]
pearson_r = xalglib.pearsoncorr2(realval, simval)
print('pearson r =', pearson_r)


#calculate p value of t test
# This test checks three hypotheses about the mean of the given samples. The
# following tests are performed:
#     * two-tailed test (null hypothesis - the means are equal)
#     * left-tailed test (null hypothesis - the mean of the first sample  is
#       greater than or equal to the mean of the second sample)
#     * right-tailed test (null hypothesis - the mean of the first sample is
#       less than or equal to the mean of the second sample).
# 
# Test is based on the following assumptions:
#     * given samples have normal distributions
#     * samples are independent.
# Equality of variances is NOT required.
# 
# Input parameters:
#     X - sample 1. Array whose index goes from 0 to N-1.
#     N - size of the sample.
#     Y - sample 2. Array whose index goes from 0 to M-1.
#     M - size of the sample.
x = [1,2,3,4,5,6,7,8,9,0]
y = [2,2,3,4,6,6,7,8,8]
n,m = len(x), len(y)
bothtails, lefttail, righttail = xalglib.unequalvariancettest(x, n, y, m)
print ('t_p_value =', bothtails, ', t_p_left =', lefttail, ', t_p_right =', righttail)


# Two-sample F-test
# 
# This test checks three hypotheses about dispersions of the given  samples.
# The following tests are performed:
#     * two-tailed test (null hypothesis - the dispersions are equal)
#     * left-tailed test (null hypothesis  -  the  dispersion  of  the first
#       sample is greater than or equal to  the  dispersion  of  the  second
#       sample).
#     * right-tailed test (null hypothesis - the  dispersion  of  the  first
#       sample is less than or equal to the dispersion of the second sample)
# 
# The test is based on the following assumptions:
#     * the given samples have normal distributions
#     * the samples are independent.
# 
# Input parameters:
#     X   -   sample 1. Array whose index goes from 0 to N-1.
#     N   -   sample size.
#     Y   -   sample 2. Array whose index goes from 0 to M-1.
#     M   -   sample size.
# 
# Output parameters:
#     BothTails   -   p-value for two-tailed test.
#                     If BothTails is less than the given significance level
#                     the null hypothesis is rejected.
#     LeftTail    -   p-value for left-tailed test.
#                     If LeftTail is less than the given significance level,
#                     the null hypothesis is rejected.
#     RightTail   -   p-value for right-tailed test.
#                     If RightTail is less than the given significance level
#                     the null hypothesis is rejected.
x = [1,2,3,4,5,6,7,8,9,0]
y = [2,2,3,4,6,6,7,8,8,11]
n,m = len(x), len(y)
bothtails, lefttail, righttail = xalglib.ftest(x, n, y, m)
print ('f_p_value =', bothtails, ', f_p_left =', lefttail, ', f_p_right =', righttail)


#calculate r2 and rmse
realval = [1,2,3,4,5,6,7,8,9,0]
simval = [2,2,3,4,6,6,7,8,8,9]
np_realval = np.array(realval)
np_simval = np.array(simval)
ave_observe = np_realval.mean()
m1 = ((np_realval - np_simval)*(np_realval - np_simval)).sum()
m2 = ((np_realval - ave_observe)*(np_realval - ave_observe)).sum()
r_2 = 1 - m1 / m2
rmse = math.sqrt(m1 / len(np_realval))
rela_rmse = rmse / ave_observe
print ('r_2 =', r_2, ', rmse =', rmse, ', rela_rmse =', rela_rmse)

#calculate MAE (mean absolute error)
realval = [1,2,3,4,5,6,7,8,9,0]
simval = [2,2,3,4,6,6,7,8,8,9]
np_realval = np.array(realval)
np_simval = np.array(simval)
delta = abs(np_realval - np_simval)
mae = delta.mean()
rela_mae = (delta / np_realval).mean()
print ('mae =', mae, ', rela_mae =', rela_mae)

