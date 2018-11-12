#ifndef IMMUNE_H
#define IMMUNE_H
#include <stdio.h>
#include <math.h>
#include "AG.h"
#include "AIS.h"
void GetClassifier(ais* Fais,ag& myag,int ABNUMBER,int  NUMJ,int NUMAG ,int AGNUMBER,double VARIATIONPG,double EXCHANGEPG,int CLASSES,int CycleIndex);
void ClassifySoft(ais *Fais,double *NewAg,int cows,int classes,double *result,int ABNUMBER);
int ClassifyHerd(ais *Fais,int ABNUMBER);
#endif