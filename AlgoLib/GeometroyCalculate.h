/*  
���㼸��  
  
  
  
Ŀ¼   
  
  
�� ��Ļ�������   
1. ƽ��������֮����� 1   
2. �ж������Ƿ��غ� 1   
3. ʸ����� 1   
4. ʸ����� 2   
5. �жϵ��Ƿ����߶��� 2   
6. ��һ����ĳ����ת������� 2   
7. ��ʸ���н� 2   
  
�� �߶μ�ֱ�ߵĻ�������   
1. �����߶εĹ�ϵ 3   
2. ��㵽�߶�����ֱ�ߴ��ߵĴ��� 4   
3. �㵽�߶ε������ 4   
4. �㵽�߶�����ֱ�ߵľ��� 4   
5. �㵽���߼���������� 4   
6. �ж�Բ�Ƿ��ڶ������ 5   
7. ��ʸ���н����� 5   
8. ���߶�֮��ļн� 5   
9. �ж��߶��Ƿ��ཻ 6   
10.�ж��߶��Ƿ��ཻ�������ڶ˵㴦 6   
11.���߶�����ֱ�ߵķ��� 6   
12.��ֱ�ߵ�б�� 7   
13.��ֱ�ߵ���б�� 7   
14.������ĳֱ�ߵĶԳƵ� 7   
15.�ж�����ֱ���Ƿ��ཻ����ֱ�߽��� 7   
16.�ж��߶��Ƿ��ཻ������ཻ���ؽ��� 7   
  
�� ����γ����㷨ģ��   
1. �ж϶�����Ƿ�򵥶���� 8   
2. ������ζ����͹���� 9   
3. �ж϶�����Ƿ�͹����� 9   
4. ��������� 9   
5. �ж϶���ζ�������з��򣬷���һ 10   
6. �ж϶���ζ�������з��򣬷����� 10   
7. ���߷��жϵ��Ƿ��ڶ������ 10   
8. �жϵ��Ƿ���͹������� 11   
9. Ѱ�ҵ㼯��graham�㷨 12   
10.Ѱ�ҵ㼯͹���ľ������ 13   
11.�ж��߶��Ƿ��ڶ������ 14   
12.��򵥶���ε����� 15   
13.��͹����ε����� 17   
14.��϶��ڸ���������ڵ�һ���� 17   
15.��Ӷ������һ��������ö���ε����� 18   
16.�ж϶���εĺ��Ƿ���� 19   
  
�� Բ�Ļ�������   
1 .���Ƿ���Բ�� 20   
2 .�󲻹��ߵ�������ȷ����Բ 21   
  
�� ���εĻ�������   
1.��֪�����������꣬���4������ 22   
  
�� �����㷨������ 22   
  
�� ����   
1����Բ��ϵ�� 24   
2���ж�Բ�Ƿ��ھ����ڣ� 24   
3���㵽ƽ��ľ��룺 25   
4�����Ƿ���ֱ��ͬ�ࣺ 25   
5�����淴���ߣ� 25   
6�����ΰ����� 26   
7����Բ���㣺 27   
8����Բ��������� 28   
9. Բ��ֱ�߹�ϵ�� 29   
10. ����Բ�� 30   
11. ���е㣺 31   
12. �߶ε��������� 31   
13����ʽ�� 32   
*/   
/* ��Ҫ������ͷ�ļ� */    
#include <cmath>    
#include "Comlib.h"
   
/* ���õĳ������� */    
const double    INF     = 1E200;   
const double    EP      = 1E-10;  
const int       MAXV    = 300; 
const double    PI      = 3.14159265;


#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

template<class TT> void swap(TT &a, TT &b )
{
	TT temp=a;
	a=b;
	b=temp;
}
   
/* �������νṹ*/    
struct ALGOLIB_API POINT    
{    
    double x;    
    double y;    
    POINT(double a=0, double b=0) { x=a; y=b;} //constructor     
};    
struct ALGOLIB_API LINESEG    
{    
    POINT s;    
    POINT e;    
    LINESEG(POINT a, POINT b) { s=a; e=b;}    
    LINESEG() { }    
};    
struct ALGOLIB_API LINE           // ֱ�ߵĽ������� a*x+b*y+c=0  Ϊͳһ��ʾ��Լ�� a >= 0     
{    
   double a;    
   double b;    
   double c;    
   LINE(double d1=1, double d2=-1, double d3=0) {a=d1; b=d2; c=d3;}    
};    
   
/**********************  
 *                    *   
 *   ��Ļ�������     *   
 *                    *   
 **********************/    
   
double ALGOLIB_API dist(POINT p1,POINT p2);
bool ALGOLIB_API equal_point(POINT p1,POINT p2);
/******************************************************************************   
r=multiply(sp,ep,op),�õ�(sp-op)��(ep-op)�Ĳ��   
r>0��ep��ʸ��opsp����ʱ�뷽��   
r=0��opspep���㹲�ߣ�   
r<0��ep��ʸ��opsp��˳ʱ�뷽��   
*******************************************************************************/    
double ALGOLIB_API multiply(POINT sp,POINT ep,POINT op);
/*   
r=dotmultiply(p1,p2,op),�õ�ʸ��(p1-op)��(p2-op)�ĵ�����������ʸ��������ʸ��   
r<0����ʸ���н�Ϊ��ǣ�  
r=0����ʸ���н�Ϊֱ�ǣ�  
r>0����ʸ���н�Ϊ�۽�   
*******************************************************************************/    
double ALGOLIB_API dotmultiply(POINT p1,POINT p2,POINT p0);
/******************************************************************************   
�жϵ�p�Ƿ����߶�l��  
������(p���߶�l���ڵ�ֱ����) && (��p�����߶�lΪ�Խ��ߵľ�����)  
*******************************************************************************/    
bool ALGOLIB_API  online(LINESEG l,POINT p);
// ���ص�p�Ե�oΪԲ����ʱ����תalpha(��λ������)�����ڵ�λ��     
POINT ALGOLIB_API rotate(POINT o,double alpha,POINT p);

/* ���ض�����o�㣬��ʼ��Ϊos����ֹ��Ϊoe�ļн�(��λ������)   
    �Ƕ�С��pi��������ֵ   
    �Ƕȴ���pi�����ظ�ֵ   
    �����������߶�֮��ļн�   
ԭ��  
    r = dotmultiply(s,e,o) / (dist(o,s)*dist(o,e))  
    r'= multiply(s,e,o)  
  
    r >= 1   angle = 0;  
    r <= -1  angle = -PI  
    -1<r<1 && r'>0 angle = arccos(r)  
    -1<r<1 && r'<=0    angle = -arccos(r)  
*/    
double ALGOLIB_API angle(POINT o,POINT s,POINT e);


/*****************************\   
*                             *   
*      �߶μ�ֱ�ߵĻ�������   *   
*                             *   
\*****************************/    

/* �жϵ����߶εĹ�ϵ,��;�ܹ㷺   
�������Ǹ�������Ĺ�ʽд�ģ�P�ǵ�C���߶�AB����ֱ�ߵĴ���   
  
                AC dot AB   
        r =     ---------   
                 ||AB||^2   
             (Cx-Ax)(Bx-Ax) + (Cy-Ay)(By-Ay)   
          = -------------------------------   
                          L^2   
  
    r has the following meaning:   
  
        r=0      P = A   
        r=1      P = B   
        r<0       P is on the backward extension of AB   
        r>1      P is on the forward extension of AB   
        0<r<1  P is interior to AB   
*/    
double ALGOLIB_API relation(POINT p,LINESEG l);
  
// ���C���߶�AB����ֱ�ߵĴ��� P     
POINT ALGOLIB_API perpendicular(POINT p,LINESEG l);

/* ���p���߶�l����̾���,�������߶��Ͼ�õ�����ĵ�np   
ע�⣺np���߶�l�ϵ���p����ĵ㣬��һ���Ǵ��� */    
double ALGOLIB_API ptolinesegdist(POINT p,LINESEG l,POINT &np);

// ���p���߶�l����ֱ�ߵľ���,��ע�Ȿ�������ϸ�����������      
double ALGOLIB_API ptoldist(POINT p,LINESEG l);

/* ����㵽���߼����������,�����������.   
ע�⣺���õ���ptolineseg()���� */    
double ALGOLIB_API ptopointset(int vcount,POINT pointset[],POINT p,POINT &q);

/* �ж�Բ�Ƿ��ڶ������.ptolineseg()������Ӧ��2 */    
bool ALGOLIB_API CircleInsidePolygon(int vcount,POINT center,double radius,POINT polygon[]);

/* ��������ʸ��l1��l2�ļнǵ�����(-1 --- 1)ע�⣺������������нǵĻ���ע�ⷴ���Һ����Ķ������Ǵ� 0��pi */    
double ALGOLIB_API cosine(LINESEG l1,LINESEG l2);

// �����߶�l1��l2֮��ļн� ��λ������ ��Χ(-pi��pi)     
double ALGOLIB_API lsangle(LINESEG l1,LINESEG l2);


// ����߶�u��v�ཻ(�����ཻ�ڶ˵㴦)ʱ������true     
//    
//�ж�P1P2����Q1Q2�������ǣ�( P1 - Q1 ) �� ( Q2 - Q1 ) * ( Q2 - Q1 ) �� ( P2 - Q1 ) >= 0��    
//�ж�Q1Q2����P1P2�������ǣ�( Q1 - P1 ) �� ( P2 - P1 ) * ( P2 - P1 ) �� ( Q2 - P1 ) >= 0��    
bool ALGOLIB_API intersect(LINESEG u,LINESEG v);


//  (�߶�u��v�ཻ)&&(���㲻��˫���Ķ˵�) ʱ����true        
bool ALGOLIB_API intersect_A(LINESEG u,LINESEG v);

// �߶�v����ֱ�����߶�u�ཻʱ����true���������ж��߶�u�Ƿ�����߶�v      
bool ALGOLIB_API intersect_l(LINESEG u,LINESEG v);

// ������֪�������꣬����������ֱ�߽������̣� a*x+b*y+c = 0  (a >= 0)      
LINE ALGOLIB_API makeline(POINT p1,POINT p2);

// ����ֱ�߽������̷���ֱ�ߵ�б��k,ˮƽ�߷��� 0,��ֱ�߷��� 1e200     
double ALGOLIB_API slope(LINE l);

// ����ֱ�ߵ���б��alpha ( 0 - pi)     
double ALGOLIB_API alpha(LINE l);

// ���p����ֱ��l�ĶԳƵ�      
POINT ALGOLIB_API symmetry(LINE l,POINT p);

// �������ֱ�� l1(a1*x+b1*y+c1 = 0), l2(a2*x+b2*y+c2 = 0)�ཻ������true���ҷ��ؽ���p      
bool ALGOLIB_API lineintersect(LINE l1,LINE l2,POINT &p); // �� L1��L2     

// ����߶�l1��l2�ཻ������true�ҽ�����(inter)���أ����򷵻�false     
bool ALGOLIB_API intersection(LINESEG l1,LINESEG l2,POINT &inter);    
   
/******************************\   
*                             *   
* ����γ����㷨ģ��       *   
*                             *   
\******************************/    
   
// ������ر�˵�����������ζ���Ҫ����ʱ������     
   
/*   
����ֵ������Ķ�����Ǽ򵥶���Σ�����true   
Ҫ �����붥�����а���ʱ������   
˵ �����򵥶���ζ��壺   
1��ѭ�������������߶ζԵĽ�������֮�乲�еĵ�����   
2�������ڵ��߶β��ཻ   
������Ĭ�ϵ�һ�������Ѿ�����   
*/    
bool ALGOLIB_API issimple(int vcount,POINT polygon[]);

// ����ֵ��������˳�򷵻ض���ζ����͹�����жϣ�bc[i]=1,iff:��i��������͹����     
void ALGOLIB_API checkconvex(int vcount,POINT polygon[],bool bc[]);

// ����ֵ�������polygon��͹�����ʱ������true      
bool ALGOLIB_API isconvex(int vcount,POINT polygon[]);

// ���ض�������(signed)�����붥�㰴��ʱ������ʱ��������ֵ�����򷵻ظ�ֵ     
double area_of_polygon(int vcount,POINT polygon[]);

// ������붥�㰴��ʱ�����У�����true     
bool ALGOLIB_API isconterclock(int vcount,POINT polygon[]);    

// ��һ���ж϶���ζ������з���ķ���      
bool ALGOLIB_API isccwize(int vcount,POINT polygon[]);

/********************************************************************************************     
���߷��жϵ�q������polygon��λ�ù�ϵ��Ҫ��polygonΪ�򵥶���Σ�������ʱ������   
   ������ڶ�����ڣ�   ����0   
   ������ڶ���α��ϣ� ����1   
   ������ڶ�����⣺    ����2   
*********************************************************************************************/    
int ALGOLIB_API insidepolygon(int vcount,POINT Polygon[],POINT q);

//��q��͹�����polygon��ʱ������true��ע�⣺�����polygonһ��Ҫ��͹�����      
bool ALGOLIB_API InsideConvexPolygon(int vcount,POINT polygon[],POINT q); // �����������Σ�     

/**********************************************   
Ѱ��͹����graham ɨ�跨   
PointSetΪ����ĵ㼯��   
chΪ�����͹���ϵĵ㼯��������ʱ�뷽������;   
nΪPointSet�еĵ����Ŀ   
lenΪ�����͹���ϵĵ�ĸ���   
**********************************************/    
void ALGOLIB_API Graham_scan(POINT PointSet[],POINT ch[],int n,int &len);

// ���������㼯͹�ǣ�����˵��ͬgraham�㷨        
void ALGOLIB_API ConvexClosure(POINT PointSet[],POINT ch[],int n,int &len);

/*********************************************************************************************    
    �ж��߶��Ƿ��ڼ򵥶������(ע�⣺����������͹����Σ�������㷨���Ի���)   
    ��Ҫ����һ���߶ε������˵㶼�ڶ�����ڣ�   
    ��Ҫ���������߶κͶ���ε����б߶����ڽ���   
    ��;�� 1. �ж������Ƿ��ڼ򵥶������   
            2. �жϼ򵥶�����Ƿ�����һ���򵥶������   
**********************************************************************************************/    
bool ALGOLIB_API LinesegInsidePolygon(int vcount,POINT polygon[],LINESEG l);

/*********************************************************************************************    
������򵥶����polygon������   
��Ҫ�������漸��������   
    void AddPosPart(); �����ұ���������   
    void AddNegPart(); ���������������   
    void AddRegion(); �����������   
��ʹ�øó���ʱ�������xtr,ytr,wtr,xtl,ytl,wtl���ȫ�ֱ����Ϳ���ʹ��Щ��������ʽ�õ�����,  
��Ҫע�⺯���������͵���Ҫ����Ӧ�仯   
**********************************************************************************************/    
void ALGOLIB_API AddPosPart(double x, double y, double w, double &xtr, double &ytr, double &wtr);

void ALGOLIB_API AddNegPart(double x, double y, double w, double &xtl, double &ytl, double &wtl);
    
void ALGOLIB_API AddRegion ( double x1, double y1, double x2, double y2, double &xtr, double &ytr,    
        double &wtr, double &xtl, double &ytl, double &wtl);

POINT ALGOLIB_API cg_simple(int vcount,POINT polygon[]);

// ��͹����ε�����,Ҫ���������ΰ���ʱ������     
POINT ALGOLIB_API gravitycenter(int vcount,POINT polygon[]);
   
/************************************************  
����һ�򵥶���Σ��ҳ�һ���϶��ڸö�����ڵĵ�   
����1 ��ÿ�������������һ��͹����   
����2 ��������>=4�ļ򵥶����������һ���Խ���   
����  �� x���������С�ĵ�϶���͹����   
    y���������С�ĵ�϶���͹����              
************************************************/    
POINT ALGOLIB_API a_point_insidepoly(int vcount,POINT polygon[]);

/***********************************************************************************************  
��Ӷ������һ��p������һ���򵥶���ε�����,������ڷ����е�,����rp�������е�,lp�����е�   
ע�⣺p��һ��Ҫ�ڶ������ ,���붥����������ʱ������   
ԭ ��    ������ڶ�����ڿ϶�������;͹�������Ψһ�������е�,������ξͿ����ж����������е�;   
        ���polygon��͹����Σ��е�ֻ������ֻҪ�ҵ��Ϳ���,���Ի�����㷨   
        ����ǰ�����λ���һ���㷨�������:���󰼶���ε�͹��,Ȼ����͹��������   
/***********************************************************************************************/    
void ALGOLIB_API pointtangentpoly(int vcount,POINT polygon[],POINT p,POINT &rp,POINT &lp);

// ��������polygon�ĺ˴��ڣ�����true�����غ��ϵ�һ��p.���㰴��ʱ�뷽������      
bool ALGOLIB_API core_exist(int vcount,POINT polygon[],POINT &p);

/*************************\   
*                        *   
* Բ�Ļ�������           *   
*                        *   
\*************************/    
/******************************************************************************  
����ֵ �� ��p��Բ��(�����߽�)ʱ������true   
��;  �� ��ΪԲΪ͹���������жϵ㼯�����ߣ�������Ƿ���Բ��ʱ��  
    ֻ��Ҫ��һ�жϵ��Ƿ���Բ�ڼ��ɡ�   
*******************************************************************************/    
bool ALGOLIB_API point_in_circle(POINT o,double r,POINT p);
    
/******************************************************************************  
�� ; ���󲻹��ߵ�����ȷ��һ��Բ   
�� �� ��������p1,p2,p3   
����ֵ ��������㹲�ߣ�����false����֮������true��Բ����q���أ��뾶��r����   
*******************************************************************************/    
bool ALGOLIB_API cocircle(POINT p1,POINT p2,POINT p3,POINT &q,double &r);

int ALGOLIB_API line_circle(LINE l,POINT o,double r,POINT &p1,POINT &p2);    
   
/**************************\   
*                         *   
* ���εĻ�������          *   
*                         *   
\**************************/    
/*   
˵������Ϊ���ε������ԣ������㷨���Ի���   
1.�жϾ����Ƿ������   
ֻҪ�жϸõ�ĺ�������������Ƿ���ھ��ε����ұߺ����±�֮�䡣   
2.�ж��߶Ρ����ߡ�������Ƿ��ھ�����   
��Ϊ�����Ǹ�͹��������ֻҪ�ж����ж˵��Ƿ��ھ����оͿ����ˡ�   
3.�ж�Բ�Ƿ��ھ�����   
Բ�ھ����еĳ�Ҫ�����ǣ�Բ���ھ�������Բ�İ뾶С�ڵ���Բ�ĵ������ıߵľ������Сֵ��   
*/    
// ��֪���ε���������(a,b,c)��������ĸ�����d������. ע�⣺��֪��������������������     
POINT ALGOLIB_API rect4th(POINT a,POINT b,POINT c);


/*************************\   
*                       *   
* �����㷨������       *   
*                       *   
\*************************/    
/*   
��δʵ�ֵ��㷨��   
1. ������㼯����СԲ   
2. �����εĽ�   
3. �򵥶���ε������ʷ�   
4. Ѱ�Ұ����㼯����С����   
5. ���ߵĻ���   
6. �жϾ����Ƿ��ھ�����   
7. �жϾ����ܷ���ھ�����   
8. ���β���������ܳ�   
9. ���β�������   
10.���β��ıհ�   
11.���εĽ�   
12.�㼯�е�������   
13.����εĲ�   
14.Բ�Ľ��벢   
15.ֱ����Բ�Ĺ�ϵ   
16.�߶���Բ�Ĺ�ϵ   
17.�����εĺ˼��������   
18.��㼯�в��ཻ��� railwai   
*//*   
Ѱ�Ұ����㼯����С����   
ԭ���þ�������һ������㼯��͹�ǵ�ĳ���߹���   
First take the convex hull of the points. Let the resulting convex   
polygon be P. It has been known for some time that the minimum   
area rectangle enclosing P must have one rectangle side flush with   
(i.e., collinear with and overlapping) one edge of P. This geometric   
fact was used by Godfried Toussaint to develop the "rotating calipers"   
algorithm in a hard-to-find 1983 paper, "Solving Geometric Problems   
with the Rotating Calipers" (Proc. IEEE MELECON). The algorithm   
rotates a surrounding rectangle from one flush edge to the next,   
keeping track of the minimum area for each edge. It achieves O(n)   
time (after hull computation). See the "Rotating Calipers Homepage"   
http://www.cs.mcgill.ca/~orm/rotcal.frame.html for a description   
and applet.   
*//*   
���ߵĻ��� α�����£�   
Input: tol = the approximation tolerance   
L = {V0,V1,...,Vn-1} is any n-vertex polyline   
  
Set start = 0;   
Set k = 0;   
Set W0 = V0;   
for each vertex Vi (i=1,n-1)   
{   
if Vi is within tol from Vstart   
then ignore it, and continue with the next vertex   
  
Vi is further than tol away from Vstart   
so add it as a new vertex of the reduced polyline   
Increment k++;   
Set Wk = Vi;   
Set start = i; as the new initial vertex   
}   
  
Output: W = {W0,W1,...,Wk-1} = the k-vertex simplified polyline   
*/    
/********************\   
*                   *   
* ����                *   
*                   *   
\********************/    
   
//��Բ��ϵ��     
/* ��Բ��   
���룺 return 1��   
���У� return 2��   
�ཻ�� return 3��   
���У� return 4��   
�ں��� return 5��   
*/    
int ALGOLIB_API CircleRelation(POINT p1, double r1, POINT p2, double r2);
    
//�ж�Բ�Ƿ��ھ����ڣ�    
// �ж�Բ�Ƿ��ھ����ڣ��Ǿͷ���true�������ˮƽ�������ĸ����������Ͽ�ʼ��˳ʱ�����У�     
// ����ptoldist�������ڵ�4ҳ     
bool ALGOLIB_API CircleRecRelation(POINT pc, double r, POINT pr1, POINT pr2, POINT pr3, POINT pr4);

//�㵽ƽ��ľ��룺     
//�㵽ƽ��ľ���,ƽ����һ��ʽ��ʾax+by+cz+d=0     
double ALGOLIB_API P2planeDist(double x, double y, double z, double a, double b, double c, double d);

//���Ƿ���ֱ��ͬ�ࣺ    
//�������Ƿ���ֱ��ͬ�࣬���򷵻�true     
bool ALGOLIB_API SameSide(POINT p1, POINT p2, LINE line);

//���淴���ߣ�    
// ��֪�����ߡ����棬�����ߡ�     
// a1,b1,c1Ϊ����ֱ�߷���(a1 x + b1 y + c1 = 0 ,��ͬ)ϵ��;      
//a2,b2,c2Ϊ�����ֱ�߷���ϵ��;      
//a,b,cΪ�����ֱ�߷���ϵ��.     
// �����з���ģ�ʹ��ʱע�⣺���������:<-b2,a2>�����������:<b,-a>.     
// ��Ҫ���ǽ���п��ܻ���"negative zeros"     
void ALGOLIB_API reflect(double a1,double b1,double c1,double a2,double b2,double c2,double &a,double &b,double &c);

//���ΰ�����     
// ����2��C��D���Ƿ���1��A��B����    
bool ALGOLIB_API r2inr1(double A,double B,double C,double D);

//��Բ���㣺     
// ��Բ�Ѿ��ཻ�����У�     
void ALGOLIB_API c2point(POINT p1,double r1,POINT p2,double r2,POINT &rp1,POINT &rp2);

//��Բ���������    
//���뱣֤�ཻ     
double ALGOLIB_API c2area(POINT p1,double r1,POINT p2,double r2);

//Բ��ֱ�߹�ϵ��     
//0----���� 1----���� 2----�ཻ     
int ALGOLIB_API clpoint(POINT p,double r,double a,double b,double c,POINT &rp1,POINT &rp2);

//����Բ��     
void ALGOLIB_API incircle(POINT p1,POINT p2,POINT p3,POINT &rp,double &r);

//���е㣺     
// p---Բ�����꣬ r---Բ�뾶�� sp---Բ��һ�㣬 rp1,rp2---�е�����     
void ALGOLIB_API cutpoint(POINT p,double r,POINT sp,POINT &rp1,POINT &rp2);


//�߶ε���������     
/* l2��l1����/�ҷ���l1Ϊ��׼�ߣ�   
����  0   �� �غϣ�   
����  1   �� ������   
����  �C1 �� ������   
*/    
int ALGOLIB_API rotat(LINESEG l1,LINESEG l2);
    
/*  
��ʽ��   
  
�����깫ʽ��   
ֱ������Ϊ P(x, y, z) ʱ����Ӧ����������(rsin��cos��, rsin��sin��, rcos��),���Ц�������OP��Z��ļнǣ���Χ[0����]����OP��XOY���ϵ�ͶӰ��X������ǣ���Χ[0��2��]    
  
ֱ�ߵ�һ�㷽��ת�����������̣�   
ax+by+c=0   
x-x0     y-y0   
   ------ = ------- // (x0,y0)Ϊֱ����һ�㣬m,nΪ����   
m        n   
ת����ϵ��   
a=n��b=-m��c=m��y0-n��x0��   
m=-b; n=a;   
  
����ƽ�淽�̣�   
����ΪP1��P2��P3   
������  M1=P2-P1; M2=P3-P1;   
ƽ�淨������  M=M1 x M2 ����   
ƽ�淽�̣�    M.i(x-P1.x)+M.j(y-P1.y)+M.k(z-P1.z)=0   
*/   
