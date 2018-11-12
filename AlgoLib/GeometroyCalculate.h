/*  
计算几何  
  
  
  
目录   
  
  
㈠ 点的基本运算   
1. 平面上两点之间距离 1   
2. 判断两点是否重合 1   
3. 矢量叉乘 1   
4. 矢量点乘 2   
5. 判断点是否在线段上 2   
6. 求一点饶某点旋转后的坐标 2   
7. 求矢量夹角 2   
  
㈡ 线段及直线的基本运算   
1. 点与线段的关系 3   
2. 求点到线段所在直线垂线的垂足 4   
3. 点到线段的最近点 4   
4. 点到线段所在直线的距离 4   
5. 点到折线集的最近距离 4   
6. 判断圆是否在多边形内 5   
7. 求矢量夹角余弦 5   
8. 求线段之间的夹角 5   
9. 判断线段是否相交 6   
10.判断线段是否相交但不交在端点处 6   
11.求线段所在直线的方程 6   
12.求直线的斜率 7   
13.求直线的倾斜角 7   
14.求点关于某直线的对称点 7   
15.判断两条直线是否相交及求直线交点 7   
16.判断线段是否相交，如果相交返回交点 7   
  
㈢ 多边形常用算法模块   
1. 判断多边形是否简单多边形 8   
2. 检查多边形顶点的凸凹性 9   
3. 判断多边形是否凸多边形 9   
4. 求多边形面积 9   
5. 判断多边形顶点的排列方向，方法一 10   
6. 判断多边形顶点的排列方向，方法二 10   
7. 射线法判断点是否在多边形内 10   
8. 判断点是否在凸多边形内 11   
9. 寻找点集的graham算法 12   
10.寻找点集凸包的卷包裹法 13   
11.判断线段是否在多边形内 14   
12.求简单多边形的重心 15   
13.求凸多边形的重心 17   
14.求肯定在给定多边形内的一个点 17   
15.求从多边形外一点出发到该多边形的切线 18   
16.判断多边形的核是否存在 19   
  
㈣ 圆的基本运算   
1 .点是否在圆内 20   
2 .求不共线的三点所确定的圆 21   
  
㈤ 矩形的基本运算   
1.已知矩形三点坐标，求第4点坐标 22   
  
㈥ 常用算法的描述 22   
  
㈦ 补充   
1．两圆关系： 24   
2．判断圆是否在矩形内： 24   
3．点到平面的距离： 25   
4．点是否在直线同侧： 25   
5．镜面反射线： 25   
6．矩形包含： 26   
7．两圆交点： 27   
8．两圆公共面积： 28   
9. 圆和直线关系： 29   
10. 内切圆： 30   
11. 求切点： 31   
12. 线段的左右旋： 31   
13．公式： 32   
*/   
/* 需要包含的头文件 */    
#include <cmath>    
#include "Comlib.h"
   
/* 常用的常量定义 */    
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
   
/* 基本几何结构*/    
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
struct ALGOLIB_API LINE           // 直线的解析方程 a*x+b*y+c=0  为统一表示，约定 a >= 0     
{    
   double a;    
   double b;    
   double c;    
   LINE(double d1=1, double d2=-1, double d3=0) {a=d1; b=d2; c=d3;}    
};    
   
/**********************  
 *                    *   
 *   点的基本运算     *   
 *                    *   
 **********************/    
   
double ALGOLIB_API dist(POINT p1,POINT p2);
bool ALGOLIB_API equal_point(POINT p1,POINT p2);
/******************************************************************************   
r=multiply(sp,ep,op),得到(sp-op)和(ep-op)的叉积   
r>0：ep在矢量opsp的逆时针方向；   
r=0：opspep三点共线；   
r<0：ep在矢量opsp的顺时针方向   
*******************************************************************************/    
double ALGOLIB_API multiply(POINT sp,POINT ep,POINT op);
/*   
r=dotmultiply(p1,p2,op),得到矢量(p1-op)和(p2-op)的点积，如果两个矢量都非零矢量   
r<0：两矢量夹角为锐角；  
r=0：两矢量夹角为直角；  
r>0：两矢量夹角为钝角   
*******************************************************************************/    
double ALGOLIB_API dotmultiply(POINT p1,POINT p2,POINT p0);
/******************************************************************************   
判断点p是否在线段l上  
条件：(p在线段l所在的直线上) && (点p在以线段l为对角线的矩形内)  
*******************************************************************************/    
bool ALGOLIB_API  online(LINESEG l,POINT p);
// 返回点p以点o为圆心逆时针旋转alpha(单位：弧度)后所在的位置     
POINT ALGOLIB_API rotate(POINT o,double alpha,POINT p);

/* 返回顶角在o点，起始边为os，终止边为oe的夹角(单位：弧度)   
    角度小于pi，返回正值   
    角度大于pi，返回负值   
    可以用于求线段之间的夹角   
原理：  
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
*      线段及直线的基本运算   *   
*                             *   
\*****************************/    

/* 判断点与线段的关系,用途很广泛   
本函数是根据下面的公式写的，P是点C到线段AB所在直线的垂足   
  
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
  
// 求点C到线段AB所在直线的垂足 P     
POINT ALGOLIB_API perpendicular(POINT p,LINESEG l);

/* 求点p到线段l的最短距离,并返回线段上距该点最近的点np   
注意：np是线段l上到点p最近的点，不一定是垂足 */    
double ALGOLIB_API ptolinesegdist(POINT p,LINESEG l,POINT &np);

// 求点p到线段l所在直线的距离,请注意本函数与上个函数的区别      
double ALGOLIB_API ptoldist(POINT p,LINESEG l);

/* 计算点到折线集的最近距离,并返回最近点.   
注意：调用的是ptolineseg()函数 */    
double ALGOLIB_API ptopointset(int vcount,POINT pointset[],POINT p,POINT &q);

/* 判断圆是否在多边形内.ptolineseg()函数的应用2 */    
bool ALGOLIB_API CircleInsidePolygon(int vcount,POINT center,double radius,POINT polygon[]);

/* 返回两个矢量l1和l2的夹角的余弦(-1 --- 1)注意：如果想从余弦求夹角的话，注意反余弦函数的定义域是从 0到pi */    
double ALGOLIB_API cosine(LINESEG l1,LINESEG l2);

// 返回线段l1与l2之间的夹角 单位：弧度 范围(-pi，pi)     
double ALGOLIB_API lsangle(LINESEG l1,LINESEG l2);


// 如果线段u和v相交(包括相交在端点处)时，返回true     
//    
//判断P1P2跨立Q1Q2的依据是：( P1 - Q1 ) × ( Q2 - Q1 ) * ( Q2 - Q1 ) × ( P2 - Q1 ) >= 0。    
//判断Q1Q2跨立P1P2的依据是：( Q1 - P1 ) × ( P2 - P1 ) * ( P2 - P1 ) × ( Q2 - P1 ) >= 0。    
bool ALGOLIB_API intersect(LINESEG u,LINESEG v);


//  (线段u和v相交)&&(交点不是双方的端点) 时返回true        
bool ALGOLIB_API intersect_A(LINESEG u,LINESEG v);

// 线段v所在直线与线段u相交时返回true；方法：判断线段u是否跨立线段v      
bool ALGOLIB_API intersect_l(LINESEG u,LINESEG v);

// 根据已知两点坐标，求过这两点的直线解析方程： a*x+b*y+c = 0  (a >= 0)      
LINE ALGOLIB_API makeline(POINT p1,POINT p2);

// 根据直线解析方程返回直线的斜率k,水平线返回 0,竖直线返回 1e200     
double ALGOLIB_API slope(LINE l);

// 返回直线的倾斜角alpha ( 0 - pi)     
double ALGOLIB_API alpha(LINE l);

// 求点p关于直线l的对称点      
POINT ALGOLIB_API symmetry(LINE l,POINT p);

// 如果两条直线 l1(a1*x+b1*y+c1 = 0), l2(a2*x+b2*y+c2 = 0)相交，返回true，且返回交点p      
bool ALGOLIB_API lineintersect(LINE l1,LINE l2,POINT &p); // 是 L1，L2     

// 如果线段l1和l2相交，返回true且交点由(inter)返回，否则返回false     
bool ALGOLIB_API intersection(LINESEG l1,LINESEG l2,POINT &inter);    
   
/******************************\   
*                             *   
* 多边形常用算法模块       *   
*                             *   
\******************************/    
   
// 如果无特别说明，输入多边形顶点要求按逆时针排列     
   
/*   
返回值：输入的多边形是简单多边形，返回true   
要 求：输入顶点序列按逆时针排序   
说 明：简单多边形定义：   
1：循环排序中相邻线段对的交是他们之间共有的单个点   
2：不相邻的线段不相交   
本程序默认第一个条件已经满足   
*/    
bool ALGOLIB_API issimple(int vcount,POINT polygon[]);

// 返回值：按输入顺序返回多边形顶点的凸凹性判断，bc[i]=1,iff:第i个顶点是凸顶点     
void ALGOLIB_API checkconvex(int vcount,POINT polygon[],bool bc[]);

// 返回值：多边形polygon是凸多边形时，返回true      
bool ALGOLIB_API isconvex(int vcount,POINT polygon[]);

// 返回多边形面积(signed)；输入顶点按逆时针排列时，返回正值；否则返回负值     
double area_of_polygon(int vcount,POINT polygon[]);

// 如果输入顶点按逆时针排列，返回true     
bool ALGOLIB_API isconterclock(int vcount,POINT polygon[]);    

// 另一种判断多边形顶点排列方向的方法      
bool ALGOLIB_API isccwize(int vcount,POINT polygon[]);

/********************************************************************************************     
射线法判断点q与多边形polygon的位置关系，要求polygon为简单多边形，顶点逆时针排列   
   如果点在多边形内：   返回0   
   如果点在多边形边上： 返回1   
   如果点在多边形外：    返回2   
*********************************************************************************************/    
int ALGOLIB_API insidepolygon(int vcount,POINT Polygon[],POINT q);

//点q是凸多边形polygon内时，返回true；注意：多边形polygon一定要是凸多边形      
bool ALGOLIB_API InsideConvexPolygon(int vcount,POINT polygon[],POINT q); // 可用于三角形！     

/**********************************************   
寻找凸包的graham 扫描法   
PointSet为输入的点集；   
ch为输出的凸包上的点集，按照逆时针方向排列;   
n为PointSet中的点的数目   
len为输出的凸包上的点的个数   
**********************************************/    
void ALGOLIB_API Graham_scan(POINT PointSet[],POINT ch[],int n,int &len);

// 卷包裹法求点集凸壳，参数说明同graham算法        
void ALGOLIB_API ConvexClosure(POINT PointSet[],POINT ch[],int n,int &len);

/*********************************************************************************************    
    判断线段是否在简单多边形内(注意：如果多边形是凸多边形，下面的算法可以化简)   
    必要条件一：线段的两个端点都在多边形内；   
    必要条件二：线段和多边形的所有边都不内交；   
    用途： 1. 判断折线是否在简单多边形内   
            2. 判断简单多边形是否在另一个简单多边形内   
**********************************************************************************************/    
bool ALGOLIB_API LinesegInsidePolygon(int vcount,POINT polygon[],LINESEG l);

/*********************************************************************************************    
求任意简单多边形polygon的重心   
需要调用下面几个函数：   
    void AddPosPart(); 增加右边区域的面积   
    void AddNegPart(); 增加左边区域的面积   
    void AddRegion(); 增加区域面积   
在使用该程序时，如果把xtr,ytr,wtr,xtl,ytl,wtl设成全局变量就可以使这些函数的形式得到化简,  
但要注意函数的声明和调用要做相应变化   
**********************************************************************************************/    
void ALGOLIB_API AddPosPart(double x, double y, double w, double &xtr, double &ytr, double &wtr);

void ALGOLIB_API AddNegPart(double x, double y, double w, double &xtl, double &ytl, double &wtl);
    
void ALGOLIB_API AddRegion ( double x1, double y1, double x2, double y2, double &xtr, double &ytr,    
        double &wtr, double &xtl, double &ytl, double &wtl);

POINT ALGOLIB_API cg_simple(int vcount,POINT polygon[]);

// 求凸多边形的重心,要求输入多边形按逆时针排序     
POINT ALGOLIB_API gravitycenter(int vcount,POINT polygon[]);
   
/************************************************  
给定一简单多边形，找出一个肯定在该多边形内的点   
定理1 ：每个多边形至少有一个凸顶点   
定理2 ：顶点数>=4的简单多边形至少有一条对角线   
结论  ： x坐标最大，最小的点肯定是凸顶点   
    y坐标最大，最小的点肯定是凸顶点              
************************************************/    
POINT ALGOLIB_API a_point_insidepoly(int vcount,POINT polygon[]);

/***********************************************************************************************  
求从多边形外一点p出发到一个简单多边形的切线,如果存在返回切点,其中rp点是右切点,lp是左切点   
注意：p点一定要在多边形外 ,输入顶点序列是逆时针排列   
原 理：    如果点在多边形内肯定无切线;凸多边形有唯一的两个切点,凹多边形就可能有多于两个的切点;   
        如果polygon是凸多边形，切点只有两个只要找到就可以,可以化简此算法   
        如果是凹多边形还有一种算法可以求解:先求凹多边形的凸包,然后求凸包的切线   
/***********************************************************************************************/    
void ALGOLIB_API pointtangentpoly(int vcount,POINT polygon[],POINT p,POINT &rp,POINT &lp);

// 如果多边形polygon的核存在，返回true，返回核上的一点p.顶点按逆时针方向输入      
bool ALGOLIB_API core_exist(int vcount,POINT polygon[],POINT &p);

/*************************\   
*                        *   
* 圆的基本运算           *   
*                        *   
\*************************/    
/******************************************************************************  
返回值 ： 点p在圆内(包括边界)时，返回true   
用途  ： 因为圆为凸集，所以判断点集，折线，多边形是否在圆内时，  
    只需要逐一判断点是否在圆内即可。   
*******************************************************************************/    
bool ALGOLIB_API point_in_circle(POINT o,double r,POINT p);
    
/******************************************************************************  
用 途 ：求不共线的三点确定一个圆   
输 入 ：三个点p1,p2,p3   
返回值 ：如果三点共线，返回false；反之，返回true。圆心由q返回，半径由r返回   
*******************************************************************************/    
bool ALGOLIB_API cocircle(POINT p1,POINT p2,POINT p3,POINT &q,double &r);

int ALGOLIB_API line_circle(LINE l,POINT o,double r,POINT &p1,POINT &p2);    
   
/**************************\   
*                         *   
* 矩形的基本运算          *   
*                         *   
\**************************/    
/*   
说明：因为矩形的特殊性，常用算法可以化简：   
1.判断矩形是否包含点   
只要判断该点的横坐标和纵坐标是否夹在矩形的左右边和上下边之间。   
2.判断线段、折线、多边形是否在矩形中   
因为矩形是个凸集，所以只要判断所有端点是否都在矩形中就可以了。   
3.判断圆是否在矩形中   
圆在矩形中的充要条件是：圆心在矩形中且圆的半径小于等于圆心到矩形四边的距离的最小值。   
*/    
// 已知矩形的三个顶点(a,b,c)，计算第四个顶点d的坐标. 注意：已知的三个顶点可以是无序的     
POINT ALGOLIB_API rect4th(POINT a,POINT b,POINT c);


/*************************\   
*                       *   
* 常用算法的描述       *   
*                       *   
\*************************/    
/*   
尚未实现的算法：   
1. 求包含点集的最小圆   
2. 求多边形的交   
3. 简单多边形的三角剖分   
4. 寻找包含点集的最小矩形   
5. 折线的化简   
6. 判断矩形是否在矩形中   
7. 判断矩形能否放在矩形中   
8. 矩形并的面积与周长   
9. 矩形并的轮廓   
10.矩形并的闭包   
11.矩形的交   
12.点集中的最近点对   
13.多边形的并   
14.圆的交与并   
15.直线与圆的关系   
16.线段与圆的关系   
17.求多边形的核监视摄象机   
18.求点集中不相交点对 railwai   
*//*   
寻找包含点集的最小矩形   
原理：该矩形至少一条边与点集的凸壳的某条边共线   
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
折线的化简 伪码如下：   
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
* 补充                *   
*                   *   
\********************/    
   
//两圆关系：     
/* 两圆：   
相离： return 1；   
外切： return 2；   
相交： return 3；   
内切： return 4；   
内含： return 5；   
*/    
int ALGOLIB_API CircleRelation(POINT p1, double r1, POINT p2, double r2);
    
//判断圆是否在矩形内：    
// 判定圆是否在矩形内，是就返回true（设矩形水平，且其四个顶点由左上开始按顺时针排列）     
// 调用ptoldist函数，在第4页     
bool ALGOLIB_API CircleRecRelation(POINT pc, double r, POINT pr1, POINT pr2, POINT pr3, POINT pr4);

//点到平面的距离：     
//点到平面的距离,平面用一般式表示ax+by+cz+d=0     
double ALGOLIB_API P2planeDist(double x, double y, double z, double a, double b, double c, double d);

//点是否在直线同侧：    
//两个点是否在直线同侧，是则返回true     
bool ALGOLIB_API SameSide(POINT p1, POINT p2, LINE line);

//镜面反射线：    
// 已知入射线、镜面，求反射线。     
// a1,b1,c1为镜面直线方程(a1 x + b1 y + c1 = 0 ,下同)系数;      
//a2,b2,c2为入射光直线方程系数;      
//a,b,c为反射光直线方程系数.     
// 光是有方向的，使用时注意：入射光向量:<-b2,a2>；反射光向量:<b,-a>.     
// 不要忘记结果中可能会有"negative zeros"     
void ALGOLIB_API reflect(double a1,double b1,double c1,double a2,double b2,double c2,double &a,double &b,double &c);

//矩形包含：     
// 矩形2（C，D）是否在1（A，B）内    
bool ALGOLIB_API r2inr1(double A,double B,double C,double D);

//两圆交点：     
// 两圆已经相交（相切）     
void ALGOLIB_API c2point(POINT p1,double r1,POINT p2,double r2,POINT &rp1,POINT &rp2);

//两圆公共面积：    
//必须保证相交     
double ALGOLIB_API c2area(POINT p1,double r1,POINT p2,double r2);

//圆和直线关系：     
//0----相离 1----相切 2----相交     
int ALGOLIB_API clpoint(POINT p,double r,double a,double b,double c,POINT &rp1,POINT &rp2);

//内切圆：     
void ALGOLIB_API incircle(POINT p1,POINT p2,POINT p3,POINT &rp,double &r);

//求切点：     
// p---圆心坐标， r---圆半径， sp---圆外一点， rp1,rp2---切点坐标     
void ALGOLIB_API cutpoint(POINT p,double r,POINT sp,POINT &rp1,POINT &rp2);


//线段的左右旋：     
/* l2在l1的左/右方向（l1为基准线）   
返回  0   ： 重合；   
返回  1   ： 右旋；   
返回  –1 ： 左旋；   
*/    
int ALGOLIB_API rotat(LINESEG l1,LINESEG l2);
    
/*  
公式：   
  
球坐标公式：   
直角坐标为 P(x, y, z) 时，对应的球坐标是(rsinφcosθ, rsinφsinθ, rcosφ),其中φ是向量OP与Z轴的夹角，范围[0，π]；是OP在XOY面上的投影到X轴的旋角，范围[0，2π]    
  
直线的一般方程转化成向量方程：   
ax+by+c=0   
x-x0     y-y0   
   ------ = ------- // (x0,y0)为直线上一点，m,n为向量   
m        n   
转换关系：   
a=n；b=-m；c=m·y0-n·x0；   
m=-b; n=a;   
  
三点平面方程：   
三点为P1，P2，P3   
设向量  M1=P2-P1; M2=P3-P1;   
平面法向量：  M=M1 x M2 （）   
平面方程：    M.i(x-P1.x)+M.j(y-P1.y)+M.k(z-P1.z)=0   
*/   
