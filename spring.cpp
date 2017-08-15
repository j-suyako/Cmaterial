// Spring.cpp : implementation file
//

#include "stdafx.h"
#include "3D-SCFDEM.h"
#include "Spring.h"
#include "Element.h"
#include "Vertex.h"
#include "Side.h"
#include "Model.h"
#include <fstream>

#include "..\inc\Miscellaneous\Macros.h"
#include <cmath>


using namespace std;  //加上此语句#include <fstream>才起作用
// CSpring
/////////////////////////////////////////////////////////////////////
// CSpring Class
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CSpring,CObject,1)

CSpring::CSpring()
: m_bIntact(true),m_bMaterAttribute(false),
m_iNum(0),m_iType(0),m_iOfEle(0),m_iTNum(0),m_iFailMode(0),
m_dLPre(0.0),
m_dLPreN(0.0),m_dLPreT(0.0),m_dLT(0.0),m_dStrainN(0.0),m_dStrainT(0.0),
m_dSprFn(0.0),m_dSprFt(0.0),m_dSprStressN(0.0),m_dSprStressT(0.0),m_dh1(0.0), m_dh2(0.0),m_darea(0.0),
m_dIniDelta(0.0),m_dSprKn(0.0),m_dSprKt(0.0),m_dEleFt(0.0),m_dEleFc(0.0),m_dContactKn(0.0),m_dContactKt(0.0),
m_dLMax(0.0),m_dLfc(0.0),m_dLft(0.0),m_dLfs(0.0),m_dShearStressMax(0.0),
m_pMaterial(new CMaterial),m_pEle(new CElement),m_pSprEleList(new CEleList),m_pSprNoList(new CSprNoList)
{

}

CSpring::~CSpring()
{
	delete m_pSprEleList;m_pSprEleList = NULL;
	delete m_pSprNoList;m_pSprNoList = NULL;
	delete m_pMaterial;m_pMaterial = NULL;
	delete m_pEle;m_pEle = NULL;
}
double CSpring::AverageRandom(double min, double max)       // need improvement?
{
	int minInteger = (int)(min*10000);                      // %min,max are double; why int?
	int maxInteger  = (int)(max*10000);
	int randInteger = 1;
	do{
		randInteger = rand()*rand();                        // %randInteger 生成随机数?
	}
	while(randInteger%10000==0); 
	int diffInteger = maxInteger - minInteger;
	int resultInteger = randInteger % diffInteger + minInteger;
	return resultInteger/10000.0;         
}

double CSpring::Weibull(double a, double b)     //生成weibull分布的随机数
{
	double u = AverageRandom(0.0,1.0);          //先生成0-1的均匀分布的随机数
	u = - log(u);
	double x = b * pow(u,1.0/a);
	return x;
}

double CSpring::Normal(double a, double b)     //生成正太分布，a为均质， b为均方差
{
	double x,y;
	x = 0;
	for(int i=0; i<12; i++)
	{
		x += AverageRandom(0.0,1.0);
	}
	x = x-6.0;
	y = a+ b * x;
	return y;
}
CSpring::CSpring(CElement * pEle, CMaterial *pMaterial, int iNum, int iType, CSprNoList *pSprNoList, CEleList *pEleList)
: m_bIntact(true),m_bMaterAttribute(false),
m_iNum(iNum),m_iType(iType),m_iOfEle(0),m_iTNum(0),m_iFailMode(0),
m_dLPre(0.0),
m_dLPreN(0.0),m_dLPreT(0.0),m_dLT(0.0),m_dStrainN(0.0),m_dStrainT(0.0),
m_dSprFn(0.0),m_dSprFt(0.0),m_dSprStressN(0.0),m_dSprStressT(0.0),m_dh1(0.0), m_dh2(0.0),m_darea(0.0),
m_dIniDelta(0.0),m_dSprKn(0.0),m_dSprKt(0.0),m_dEleFt(0.0),m_dEleFc(0.0),m_dContactKn(0.0),m_dContactKt(0.0),
m_dLMax(0.0),m_dLfc(0.0),m_dLft(0.0),m_dLfs(0.0),m_dShearStressMax(0.0),
m_pMaterial(pMaterial),m_pEle(pEle),m_pSprEleList(pEleList),m_pSprNoList(pSprNoList)
{
	Initial();
}
void CSpring::Initial()
{
	/*ofstream outdata0( "Initialspr_Data.txt " );*/

    m_iOfEle = m_pEle->GetEleNum();
	double cos0;
	C3DVec VecT,VecN,Vec0,Vec1,Vec2,Vec3;

	//弹簧方向矢量初始化
	if (m_iType!=2)    //普通弹簧
	{
		Vec0 = (m_pEle->GetFaceList()->GetAt(m_iNum)->GetfVertList()->GetAt(0)->GetPos() - m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr()).GetNormal();
		Vec1 = (m_pEle->GetFaceList()->GetAt(m_iNum)->GetfVertList()->GetAt(1)->GetPos() - m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr()).GetNormal();
		Vec2 = (m_pEle->GetCentr() - m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr()).GetNormal();
		VecN = (Vec0 % Vec1).GetNormal();
		cos0 = (VecN * Vec2)/(VecN.GetModule()*Vec2.GetModule());
		if (cos0<0.0 && cos0>=-1.0)
		{
			m_UniVecN = C3DVec(VecN.m_dX, VecN.m_dY, VecN.m_dZ);     //保证法向量指向面外
		}
		else if (cos0>0.0 && cos0<=1.0)
		{
			m_UniVecN = C3DVec(-VecN.m_dX, -VecN.m_dY, -VecN.m_dZ);
		}	
		//else if (cos0 == 0.0)   //单元的质心在其内部，与面的形心连线必然不可能再面上，因此与面的法线不可能有垂直的状况
		//{
		//    ASSERT(false); 
		//}
		
		else
		{
			ASSERT(false); 
		}
	
	}
	else if(m_iType == 2)          //连接加载板和底板的弹簧
	{
		if(m_pEle->GetType() == 4 )       //上加载板单元
		{
            m_UniVecN = C3DVec(0.0, -1.0, 0.0);
		}
		else if(m_pEle->GetType() == 5 )       //下加载板单元
		{
            m_UniVecN = C3DVec(0.0, 1.0, 0.0);
		}
		else if(m_pEle->GetType() == 9 )       //左加载板单元
		{
			m_UniVecN = C3DVec(1.0, 0.0, 0.0);
		}
		else if(m_pEle->GetType() == 10 )       //右加载板单元
		{
			m_UniVecN = C3DVec(-1.0, 0.0, 0.0);
		}
		else if(m_pEle->GetType() == 13 )       //前加载板单元
		{
			m_UniVecN = C3DVec(0.0, 0.0, -1.0);
		}
		else if(m_pEle->GetType() == 14 )       //后加载板单元
		{
			m_UniVecN = C3DVec(0.0, 0.0, 1.0);
		}
		
		else if(m_pEle->GetType() == 2 || m_pEle->GetType() == 3 || m_pEle->GetType() == 7 || m_pEle->GetType() == 8 || m_pEle->GetType() ==11 || m_pEle->GetType() ==12)
		{
			Vec0 = (m_pEle->GetFaceList()->GetAt(m_iNum)->GetfVertList()->GetAt(0)->GetPos() - m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr()).GetNormal();
			Vec1 = (m_pEle->GetFaceList()->GetAt(m_iNum)->GetfVertList()->GetAt(1)->GetPos() - m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr()).GetNormal();
			Vec2 = (m_pEle->GetCentr() - m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr()).GetNormal();
			VecN = (Vec0 % Vec1).GetNormal();			
			cos0 = (VecN * Vec2)/(VecN.GetModule()*Vec2.GetModule());
			if (cos0<0.0 && cos0>=-1.0)
			{
				m_UniVecN = C3DVec(VecN.m_dX, VecN.m_dY, VecN.m_dZ);
			}
			else if (cos0>0.0 && cos0<=1.0)
			{
				m_UniVecN = C3DVec(-VecN.m_dX, -VecN.m_dY, -VecN.m_dZ);
			}
			else
			{
				ASSERT(false); 
			}
		}
		
	}
	else
	{
		ASSERT(false); 
	}
	m_dLPreN = (m_pSprNoList->GetAt(1)->GetPos() - m_pSprNoList->GetAt(0)->GetPos()) * m_UniVecN;  // 点积求弹簧 法向长度
	Vec3 = (m_pSprNoList->GetAt(1)->GetPos() - m_pSprNoList->GetAt(0)->GetPos()) % m_UniVecN;
	m_UniVecT = m_UniVecN % Vec3; //切向弹簧矢量
	m_UniVecT = m_UniVecT.GetNormal();
	m_dLPreT = (m_pSprNoList->GetAt(1)->GetPos() - m_pSprNoList->GetAt(0)->GetPos()) * m_UniVecT;  // 点积求弹簧 切向长度

   /* outdata0<<"m_Centr"<<"   "<<"X=  "<<m_pEle->GetCentr().m_dX<<"   "<<"Y=  "<<m_pEle->GetCentr().m_dY<<"   "<<"Z=  "<<m_pEle->GetCentr().m_dZ<<endl;
	outdata0<<"m_fCentr"<<"   "<<"X=  "<< m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr().m_dX<<"   "<<"Y=  "<< m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr().m_dY<<"   "<<"Z=  "<< m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr().m_dZ<<endl;

	outdata0<<"Vec0"<<"   "<<"X=  "<<Vec0.m_dX<<"   "<<"Y=  "<<Vec0.m_dY<<"   "<<"Z=  "<<Vec0.m_dZ<<endl;
	outdata0<<"Vec1"<<"   "<<"X=  "<<Vec1.m_dX<<"   "<<"Y=  "<<Vec1.m_dY<<"   "<<"Z=  "<<Vec1.m_dZ<<endl;
	outdata0<<"Vec2"<<"   "<<"X=  "<<Vec2.m_dX<<"   "<<"Y=  "<<Vec2.m_dY<<"   "<<"Z=  "<<Vec2.m_dZ<<endl;
	outdata0<<"VecN"<<"   "<<"X=  "<<VecN.m_dX<<"   "<<"Y=  "<<VecN.m_dY<<"   "<<"Z=  "<<VecN.m_dZ<<endl;
	outdata0<<"cos0="<<"   "<<cos0<<endl;

	outdata0<<"m_UniVecN"<<"   "<<"X=  "<<m_UniVecN.m_dX<<"   "<<"Y=  "<<m_UniVecN.m_dY<<"   "<<"Z=  "<<m_UniVecN.m_dZ<<endl;
	outdata0<<"m_UniVecT"<<"   "<<"X=  "<<m_UniVecT.m_dX<<"   "<<"Y=  "<<m_UniVecT.m_dY<<"   "<<"Z=  "<<m_UniVecT.m_dZ<<endl;*/

	//在我的模型中则不仅仅是法向弹簧，而是法向和切向的总体变形是竖直向下的(wzl)
	//如果考虑自重影响下的初始变形之后，应当对m_dLPreN、m_dLPreT进行修正(wzl)

	//求代表厚度
	// 处理骨料完全刚性和加载边完全刚性，部分骨料周围单元，以及加载板附近单元，m_dh2 = 0；
	if(m_iType == 1)                    //界面弹簧
	{  
		if(m_pEle->GetType() == 1)      //弹簧本单元为骨料单元
		{
			if(GetModel()->m_iAggRigid==0)   //骨料为完全刚性，the solving method is to 折减砂浆弹性模量
			{
				m_dh1 = 0;              //骨料代表长度为0
				m_dh2 = fabs( (m_pSprEleList->GetAt(1)->GetCentr() - m_pSprNoList->GetAt(1)->GetPos()) * m_UniVecN );
			}
			else                        //骨料非完全刚性
			{
				m_dh1 = fabs( (m_pSprEleList->GetAt(0)->GetCentr() - m_pSprNoList->GetAt(0)->GetPos()) * m_UniVecN );
				m_dh2 = fabs( (m_pSprEleList->GetAt(1)->GetCentr() - m_pSprNoList->GetAt(1)->GetPos()) * m_UniVecN );
			}
		}
		else                            //弹簧本单元为砂浆单元
		{
			if(GetModel()->m_iAggRigid==0)   //骨料为完全刚性，采用折减砂浆弹性模量的求解方法，其代表长度为0
			{
				m_dh1 = fabs( (m_pSprEleList->GetAt(0)->GetCentr() - m_pSprNoList->GetAt(0)->GetPos()) * m_UniVecN );
				m_dh2 = 0;
			}
			else                        //骨料非完全刚性
			{	
				m_dh1 = fabs( (m_pSprEleList->GetAt(0)->GetCentr() - m_pSprNoList->GetAt(0)->GetPos()) * m_UniVecN );
				m_dh2 = fabs( (m_pSprEleList->GetAt(1)->GetCentr() - m_pSprNoList->GetAt(1)->GetPos()) * m_UniVecN );
			}
		}
	}
	else if(m_iType == 2)               //加载板和底板连接弹簧
	{
		if(m_pEle->GetType()==4 || m_pEle->GetType()==5 || m_pEle->GetType()==9 || m_pEle->GetType()==10 || m_pEle->GetType()==13 || m_pEle->GetType()==14)      //本单元为加载板单元
		{
			m_dh1 = 0;
			m_dh2 = fabs( (m_pSprEleList->GetAt(1)->GetCentr() - m_pSprNoList->GetAt(1)->GetPos()) * m_UniVecN );
		}
		else                            //本单元为砂浆单元
		{
			m_dh1 = fabs( (m_pSprEleList->GetAt(0)->GetCentr() - m_pSprNoList->GetAt(0)->GetPos()) * m_UniVecN );
			m_dh2 = 0;
		}
	}
	else if(m_iType == 0)                              //普通砂浆弹簧m_iType == 0
	{
		m_dh1 = fabs( (m_pSprEleList->GetAt(0)->GetCentr() - m_pSprNoList->GetAt(0)->GetPos()) * m_UniVecN );
		m_dh2 = fabs( (m_pSprEleList->GetAt(1)->GetCentr() - m_pSprNoList->GetAt(1)->GetPos()) * m_UniVecN );
	}
	else
	{
		ASSERT(false); 
	}

	//求弹簧的代表宽度（指面积）
	if(m_pEle->GetType() !=4 && m_pEle->GetType() !=5 && m_pEle->GetType() !=9 && m_pEle->GetType() !=10 &&m_pEle->GetType() !=13 && m_pEle->GetType() !=14)     //普通单元求边长，弹簧的代表宽度
	{		
		m_darea =(m_pEle->GetFaceList()->GetAt(m_iNum)->GetFaceArea());
	}
	else											       //加载端的代表面积
	{
		int RLSideNum = m_pSprNoList->GetAt(1)->GetNum();  //对面弹簧节点所在其单元的弹簧点的编号
		m_darea = (m_pSprEleList->GetAt(1)->GetFaceList()->GetAt(RLSideNum)->GetFaceArea());   //代表宽度
	}

}

//**************************
//单元强度与力学属性分配
void CSpring::SetSprAttribute()
{
	// i、j、n均是表示弹簧远端单元的编号，弹簧节点编号以及单元的弹簧总数（Juny 2014.7.23）
	int i = m_pSprNoList->GetAt(1)->GetOfEle();   //本单元本弹簧的另一端的R单元在单元总体链表中的编号
	int j = m_pSprNoList->GetAt(1)->GetNum();     //R单元弹簧在R单元弹簧列表中的编号
	int n = GetModel()->m_pEleList->GetAt(i)->GetSprList()->GetSize();    //R单元的弹簧总数

	if (m_iType==2) //加载面弹簧
	{
		m_bMaterAttribute = true;               //确保同一根弹簧不会被两次赋值
		m_dSprKn = m_pMaterial->GetBondKn();
		m_dSprKt = m_pMaterial->GetBondKt();
		m_dEleFt = m_pMaterial->GetMorEleFt();
		m_dEleFc = m_pMaterial->GetMorEleFc();
	} 
	else  //两大类区分
	{
		m_dContactKn = m_pMaterial->GetContactKn();   //暂时假定砂浆单元之间以及砂浆与骨料单元之间的接触刚度相同
		m_dContactKt = m_pMaterial->GetContactKt();
		if (m_iType==0 && !m_bMaterAttribute)      // 砂浆弹簧
		{
			m_bMaterAttribute = true;
			m_dSprKn = m_pMaterial->GetMorSprKn();
			m_dSprKt = m_pMaterial->GetMorSprKt();
			m_dEleFt = m_pMaterial->GetMorEleFt();
			m_dEleFc = m_pMaterial->GetMorEleFc();
			if(GetModel()->m_bMatVariation)           //考虑材料变异性
			{
				DistributionType() ;
			}
			m_dLft = m_dEleFt / m_pMaterial->GetMorEleE() * (m_dh1 + m_dh2);
			m_dLMax = 0.03;                         //最大裂缝宽度
			m_dLfc = m_dEleFc / m_pMaterial->GetMorEleE() * (m_dh1 + m_dh2);       //极限压应变对应的变形位移，此处取砂浆试验值0.003大约
		}
		else if (m_iType==1 && !m_bMaterAttribute)  // 界面弹簧
		{
			if(GetModel()->m_iAggRigid==0)   //骨料为完全刚性，Using mortar elastic modulus reduction method for solving the kn and kt of springs
			{
				m_bMaterAttribute = true;
				m_dSprKn = m_pMaterial->GetIntSprKn();
				m_dSprKt = m_pMaterial->GetIntSprKt();
				m_dEleFt = m_pMaterial->GetIntEleFt();
				m_dEleFc = m_pMaterial->GetMorEleFc();   //界面压剪破坏准则的衡量仍以砂浆的轴心抗压强度为准
				if(GetModel()->m_bMatVariation)  //考虑材料变异性
				{
					DistributionType();
				}
				m_dLft = m_dEleFt / m_pMaterial->GetIntEleE() * (m_dh1+m_dh2);
				m_dLMax = 0.03;
				m_dLfc = m_dEleFc / m_pMaterial->GetIntEleE() * (m_dh1+m_dh2);       //极限压应变对应的变形位移，此处取砂浆试验值0.003大约
			}
			else //骨料不完全刚性，有弹性模量，采用加权平均的求解方法
			{
				m_bMaterAttribute = true;
				double Kn1, Kn2, Kt1, Kt2;
				if(m_pEle->GetType() == 1)  //本单元为骨料单元
				{
					Kn1 = m_pMaterial->GetAggSprKn();
					Kt1 = m_pMaterial->GetAggSprKt();
					Kn2 = m_pMaterial->GetMorSprKn();
					Kt2 = m_pMaterial->GetMorSprKt();
				}
				else   //如果本单元为砂浆单元
				{
					Kn2 = m_pMaterial->GetAggSprKn();
					Kt2 = m_pMaterial->GetAggSprKt();
					Kn1 = m_pMaterial->GetMorSprKn();
					Kt1 = m_pMaterial->GetMorSprKt();
				}

				m_dSprKn =(m_dh1*Kn1 + m_dh2*Kn2) / (m_dh1 + m_dh2);
				m_dSprKt = (m_dh1*Kt1 + m_dh2*Kt2) / (m_dh1 + m_dh2);
				m_dEleFt = m_pMaterial->GetIntEleFt();
				m_dEleFc = m_pMaterial->GetMorEleFc();
				if(GetModel()->m_bMatVariation)  //考虑材料变异性
				{
					DistributionType();
				}
				m_dLMax = 0.03;
				m_dLft = m_dEleFt / m_dSprKn * (m_dh1+m_dh2);
				m_dLfc = m_dEleFc/m_dSprKn  * (m_dh1+m_dh2);       //极限压应变对应的变形位移，此处取砂浆试验值0.003大约
			}
		} 
		for(int k=0; k<n; k++)
		{
			if(GetModel()->m_pEleList->GetAt(i)->GetSprList()->GetAt(k)->GetNum() == j)
			{
				GetModel()->m_pEleList->GetAt(i)->GetSprList()->GetAt(k)->SetMaterialState(true);
				GetModel()->m_pEleList->GetAt(i)->GetSprList()->GetAt(k)->SetRSprMaterial(m_dSprKn, m_dSprKt, m_dEleFt, m_dEleFc, m_dLMax, m_dLfc, m_dLft); //给该弹簧(远端弹簧)赋同样属性
			}
		}	
	}
}
void CSpring::DistributionType()    //根据不同的分布进行材料参数取值
{
	int temp = GetModel()->m_iDistributionType;
	if(GetModel()->m_iDistributionType==2)       //weibull 分布
	{
		double temp = Weibull(GetModel()->m_dWeibull_a,1);
		m_dEleFt = m_dEleFt * temp;                       //weibull 分布函数的参数，前一个参数为均质度
		m_dEleFc = m_dEleFc * temp;              //认为同一单元抗拉、抗压强度的不均匀性一致，因此采用同一个权重系数（临时生成的随机数）temp。
	}
	else if(GetModel()->m_iDistributionType==1)  //正态分布
	{
		double temp = Normal(1, GetModel()->m_dNormal_d);
		m_dEleFt = m_dEleFt * temp;
		m_dEleFc = m_dEleFc * temp;
	}	
}
double CSpring::GetEleFt()
{
	return m_dEleFt;
}

double CSpring::GetEleFc()
{
	return m_dEleFc;
}
void CSpring::SetMaterialState(BOOL State)
{
	m_bMaterAttribute = State;
}
BOOL CSpring::GetMaterialState()
{
	return m_bMaterAttribute;
}

void CSpring::SetRSprMaterial(double SprKn, double SprKt, double EleFt, double EleFc, double LMax, double Lfc, double Lft) //给R单元同一弹簧赋以同样的属性
{
	m_dSprKn = SprKn;
	m_dSprKt = SprKt;
	m_dEleFt = EleFt;
	m_dEleFc = EleFc;
	m_dLMax = LMax;
	m_dLfc = Lfc;   //极限压应变(最大压应力)对应的变形位移
	m_dLft = Lft;
}
//*********************************
//单元产生位移后，定期更新弹簧的矢量方向（法向和切向）

void CSpring::UpdateSprDir()
{
	double cos0;
	C3DVec VecT,VecN,Vec0,Vec1,Vec2,Vec3;

	//弹簧方向矢量初始化
	if (m_iType!=2)    //普通弹簧
	{
		Vec0 = (m_pEle->GetFaceList()->GetAt(m_iNum)->GetfVertList()->GetAt(0)->GetPos() - m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr()).GetNormal();
		Vec1 = (m_pEle->GetFaceList()->GetAt(m_iNum)->GetfVertList()->GetAt(1)->GetPos() - m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr()).GetNormal();
		Vec2 = (m_pEle->GetCentr() - m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr()).GetNormal();
		VecN = (Vec0 % Vec1).GetNormal();
		cos0 = (VecN * Vec2)/(VecN.GetModule()*Vec2.GetModule());
		if (cos0<0.0 && cos0>=-1.0)
		{
			m_UniVecN = C3DVec(VecN.m_dX, VecN.m_dY, VecN.m_dZ);
		}
		else if (cos0>0.0 && cos0<=1.0)
		{
			m_UniVecN = C3DVec(-VecN.m_dX, -VecN.m_dY, -VecN.m_dZ);
		}
		else    
		{
			ASSERT(false); 
		}		
	}
	else if(m_iType == 2)          //连接加载板和底板的弹簧
	{
		if(m_pEle->GetType() == 4 )       //上加载板单元
		{
			m_UniVecN = C3DVec(0.0, -1.0, 0.0);
		}
		else if(m_pEle->GetType() == 5 )       //下加载板单元
		{
			m_UniVecN = C3DVec(0.0, 1.0, 0.0);
		}
		else if(m_pEle->GetType() == 9 )       //左加载板单元
		{
			m_UniVecN = C3DVec(1.0, 0.0, 0.0);
		}
		else if(m_pEle->GetType() == 10)       //右加载板单元
		{
			m_UniVecN = C3DVec(-1.0, 0.0, 0.0);
		}
		else if(m_pEle->GetType() == 13 )       //前加载板单元
		{
			m_UniVecN = C3DVec(0.0, 0.0, -1.0);
		}
		else if(m_pEle->GetType() == 14 )       //后加载板单元
		{
			m_UniVecN = C3DVec(0.0, 0.0, 1.0);
		}

		else if(m_pEle->GetType() == 2 || m_pEle->GetType() == 3 || m_pEle->GetType() == 7 || m_pEle->GetType() == 8 || m_pEle->GetType() == 11 || m_pEle->GetType() == 12)
		{
			Vec0 = (m_pEle->GetFaceList()->GetAt(m_iNum)->GetfVertList()->GetAt(0)->GetPos() - m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr()).GetNormal();
			Vec1 = (m_pEle->GetFaceList()->GetAt(m_iNum)->GetfVertList()->GetAt(1)->GetPos() - m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr()).GetNormal();
			Vec2 = (m_pEle->GetCentr() - m_pEle->GetFaceList()->GetAt(m_iNum)->GetfCentr()).GetNormal();
			VecN = (Vec0 % Vec1).GetNormal();
			cos0 = (VecN * Vec2)/(VecN.GetModule()*Vec2.GetModule());
			if (cos0<0.0 && cos0>=-1.0)
			{
				m_UniVecN = C3DVec(VecN.m_dX, VecN.m_dY, VecN.m_dZ);
			}
			else if (cos0>0.0 && cos0<=1.0)
			{
				m_UniVecN = C3DVec(-VecN.m_dX, -VecN.m_dY, -VecN.m_dZ);
			}
			else
			{
				ASSERT(false); 
			}
		}

	}
	else
	{
		ASSERT(false); 
	}	
	Vec3 = (m_pSprNoList->GetAt(1)->GetPos() - m_pSprNoList->GetAt(0)->GetPos())%m_UniVecN; //切向弹簧矢量
	m_UniVecT = m_UniVecN % Vec3;
	m_UniVecT = m_UniVecT.GetNormal();	
}
CSpring::CSpring(int iTNum, int iNum, CSprNoList *pSprNoList, CMaterial *pMaterial)   //弹簧整体列表中弹簧的构造函数
{
	m_bIntact=true;
	m_iTNum = iTNum;     //整体列表编号
	m_iNum=iNum;         //弹簧0节点所在单元上的弹簧节点的编号
	m_pSprNoList=pSprNoList;
	m_pMaterial=pMaterial;

	m_dLPre=0.;
	m_dLPreN=0.;
	m_dLPreT=0.;
	m_dIniDelta=0.;
}

CSpring::CSpring(int iTNum, CSprNoList *pSprNoList, CEleList *pEleList)
{
	m_iTNum = iTNum;     //整体列表编号
	m_pSprNoList = pSprNoList;
	m_pSprEleList= pEleList;
}

//计算并返回弹簧力
C3DVec CSpring::GetCalcuSprForce(CElement *pEle)  
{
	//	if(!m_bIntact)
	//		return C2DVec(0.0, 0.0);
	//弹簧方向矢量的更新，放在每一子步加载完毕后集中更新，此处不再更新
	double dLNexN,dLNexT;
	C3DVec vecTemp = m_pSprNoList->GetAt(1)->GetPos()-m_pSprNoList->GetAt(0)->GetPos(); //相减后矢量由点0指向点1
	dLNexN = vecTemp * m_UniVecN;  // 点积，弹簧变形在该弹簧点法向上的分量
	dLNexT = vecTemp * m_UniVecT;  // 点积，弹簧变形在该弹簧点切向上的分量
	m_dLN = dLNexN-m_dLPreN;       //法向弹簧变形受压为负，受拉为正     
	m_dLT = dLNexT-m_dLPreT;       //切向弹簧变形 ,与正方向一致为正值，相反为负值   
	ASSERT(fabs(m_dh1+m_dh2)>1E-6);
	m_dStrainN = (m_dLN/*+m_dIniDelta*/)/(m_dh1+m_dh2);
	m_dStrainT = (m_dLT/*+m_dIniDelta*/)/(m_dh1+m_dh2);

	C3DVec vec;
	if(m_iType==2)       //加载端链接弹簧-弹簧力
	{
		vec = CalcuForceOfType2();
	}	
	else if(m_iType==0)  //普通砂浆弹簧 弹簧力
	{
		vec = CalcuForceOfType0();
	}
	else if(m_iType==1)  //界面弹簧 弹簧力
	{
		vec = CalcuForceOfType1();
	}
	else
	{
		ASSERT(false);
	}
	//test
	if(!m_bIntact)
	{
		int b=2;
	}
	if( m_bIntact && m_iFailMode==NEITHER )    //对没有坏的弹簧根据弹簧变形和力，进行状态更新，已经坏的，就按之前的状态不再重复更新
	{
		UpdateSprState();   
	}	
	return vec;
}
C3DVec CSpring::CalcuForceOfType2()   //加载端链接弹簧-弹簧力
{
	m_dSprStressN = - m_dStrainN * m_dSprKn;    //受拉为负，受压为正
	m_dSprStressT = m_dStrainT * m_dSprKt;      //切向仍然按照实际求出的值的正负 
	m_dSprFn = -m_dSprStressN * m_darea;           //弹簧力受压为负，受拉为正
	m_dSprFt = m_dSprStressT * m_darea;
	m_VecSprF = m_UniVecN * m_dSprFn + m_UniVecT * m_dSprFt ;      //弹簧合力在整体坐标系的值，矢量
	m_VecSprStress = m_VecSprF/m_darea;         //弹簧合应力矢量 	
	m_bIntact = true;
	return m_VecSprF;                   
}
C3DVec CSpring::CalcuForceOfType0()      //普通砂浆弹簧 弹簧力//!!!!!!!!
{
	switch(m_iFailMode)
	{
	case NEITHER:
		NormalSpringConstribute();           //法向弹簧本构关系
		ShearSpringConstribute0();           //切向弹簧本构关系
		break;
	case TESION:        
		ContactConstribute();           //弹簧破坏后，转入接触本构
		break;
	case PRESSURE:
		ContactConstribute();           //
		break;
	case SHEAR:
		ContactConstribute();           //弹簧破坏后，转入接触本构
		break;
	case BOTH:
		ContactConstribute();           //弹簧破坏后，转入接触本构
		break;
	}
	m_dSprFn = -m_dSprStressN * m_darea;    //弹簧力受压为负，受拉为正
	m_dSprFt = m_dSprStressT * m_darea;
	m_VecSprF = m_UniVecN * m_dSprFn + m_UniVecT * m_dSprFt ;      //弹簧合力在整体坐标系的值，用向量表示
	m_VecSprStress = m_VecSprF/m_darea;  // 弹簧应力 矢量
	return m_VecSprF;	
}

C3DVec CSpring::CalcuForceOfType1()      //界面弹簧 弹簧力/
{		
	switch(m_iFailMode)
	{
	case NEITHER:
		NormalSpringConstribute();           //法向弹簧本构关系
		ShearSpringConstribute1();           //切向弹簧本构关系
		break;
	case TESION:
		ContactConstribute();           //
		break;
	case PRESSURE:
		ContactConstribute();           //
		break;
	case SHEAR:
		ContactConstribute();           //弹簧破坏后，转入接触本构
		break;
	case BOTH:
		ContactConstribute();           //弹簧破坏后，转入接触本构
		break;
	}
	m_dSprFn = -m_dSprStressN * m_darea;    //弹簧力受压为负，受拉为正
	m_dSprFt = m_dSprStressT * m_darea;
	m_VecSprF = m_UniVecN * m_dSprFn + m_UniVecT * m_dSprFt ;      //弹簧合力在整体坐标系的值，用向量表示
	m_VecSprStress = m_VecSprF/m_darea;  // 弹簧应力 受压为正，受拉为负
	return m_VecSprF;
}
void CSpring::ContactConstribute()
{
	BOOL bContact = GetModel()->m_bContact;    //判断是否考虑接触作用

	if( m_dLN < 0.0 && m_dLN > -1*0.003 && bContact)   //法向变形 负值表明单元之间又有接触，有叠合量
	{
		m_dSprStressN = - m_dStrainN * m_dContactKn;    //法向应力受压为正
		m_dSprStressT = m_dStrainT * m_dContactKt; // 
		if( m_dSprStressT>= m_dSprStressN * 0.70)    //接触面的塑性剪切破坏准则Fs<=Fn*tan35.
		{
			m_dSprStressT = m_dSprStressN * 0.70;
		}
		else if(m_dSprStressT<= -1.0*m_dSprStressN * 0.70)
		{
			m_dSprStressT = -1.0* m_dSprStressN * 0.70;
		}
		else
		{
			m_dSprStressT = m_dStrainT * m_dContactKt; 
		}
	}
	else       //如果 m_dLN 为正值，则说明单元没有接触，认为没有产生接触力，法向和切向均为零 或者在不考虑接触的情况下
	{
		m_dSprStressN = 0.0;
		m_dSprStressT = 0.0; 
	}
}
void CSpring::NormalSpringConstribute()  //法向弹簧本构关系
{
	if(m_dLN <= - m_dLfc)                //受压水平段（理想塑性）   
	{
		m_dSprStressN = m_dEleFc; 
	}
	else if(m_dLN <= m_dLft)             //线性段（从0.003压应变至抗拉强度对应的拉应变）
	{
		m_dSprStressN = -m_dStrainN * m_dSprKn; 
	}
	
	else if(m_dLN <= m_dLMax)            //软化段（从抗拉强度对应的拉应变至最大裂缝宽度对应的应变）          
	{
		m_dSprStressN = - m_dEleFt + m_dEleFt * (m_dLN-m_dLft)/(m_dLMax-m_dLft);
	}
	
	else                                 //破坏段，超过了最大裂缝宽度
	{
		m_dSprStressN = 0.0;   //且法向弹簧破坏了
	}
}
void CSpring::ShearSpringConstribute0()  //砂浆弹簧压剪耦合本构，以试验报告中拟合曲线的结果为准
{
	if(m_dSprStressN > 0.6 * m_dEleFc)    //下降段，拟合公式为直线(已经改为水平线)
	{
		m_dShearStressMax = 0.454 * m_dEleFc;
	}	
	else                                 //上升段，拟合公式为二次式(已经改为直线）
	{
		ASSERT(!IS_NCZERO(m_dEleFc));   
		m_dShearStressMax = 0.102 * m_dEleFc + 0.7002 * m_dSprStressN ;
		//		m_dShearStressMax = 0.092 * m_dEleFc + 1.181 * m_dSprStressN -0.964 * m_dSprStressN * m_dSprStressN / m_dEleFc;
	}
	m_dLfs = m_dShearStressMax / m_dSprKt * (m_dh1+m_dh2); 

	if(m_dLT <= - m_dLfs)
	{
		m_dSprStressT = -m_dShearStressMax;
	}
	else if(m_dLT <= m_dLfs)
	{
		m_dSprStressT = m_dStrainT * m_dSprKt;
	}
	else
	{
		m_dSprStressT = m_dShearStressMax;
	}
}
void CSpring::ShearSpringConstribute1()  //界面弹簧压剪耦合本构，以试验报告中拟合曲线的结果为准
{
	if(m_dSprStressN> 0.663 * m_dEleFc)  //下降段，拟合公式为圆弧
	{
		m_dShearStressMax = sqrt( (m_dSprStressN / m_dEleFc) *( 1 - m_dSprStressN / m_dEleFc)) * m_dEleFc;   
	}
	else                                 //上升段，拟合公式为二次式
	{
		m_dShearStressMax = 0.049 * m_dEleFc + 1.150 * m_dSprStressN -0.770 * m_dSprStressN * m_dSprStressN / m_dEleFc;
	}
	m_dLfs = m_dShearStressMax / m_dSprKt * (m_dh1+m_dh2); 

	if(m_dLT <= - m_dLfs)
	{
		m_dSprStressT = -m_dShearStressMax;
	}
	else if(m_dLT <= m_dLfs)
	{
		m_dSprStressT = m_dStrainT * m_dSprKt;
	}
	else
	{
		m_dSprStressT = m_dShearStressMax;
	}
}

void CSpring::ShearSpringConstribute_Old()    //旧版本本构关系//切向弹簧本构关系。可用于测试
{
	m_dShearStressMax  = 0.11 * pow(m_dEleFt, 3.0)* pow((m_dSprStressN+m_dEleFt),0.6) + m_dEleFt; //与压应力相关的最大剪应力
	m_dLfs = m_dShearStressMax / m_dSprKt * (m_dh1+m_dh2); 
	if(m_dLT <= - m_dLfs)
	{
		m_dSprStressT =  -m_dShearStressMax;
	}
	else if(m_dLT <= m_dLfs)
	{
		m_dSprStressT = m_dStrainT * m_dSprKt;
	}
	else
	{
		m_dSprStressT = m_dShearStressMax;
	}
}
void CSpring::NormalSpringConstribute_Old()  //旧版本本构关系/可用于测试//法向弹簧本构关系
{
	if(m_dLN <= - m_dLfc)
	{
		m_dSprStressN = 0.002 * m_dSprKn; 
	}
	else if(m_dLN <= m_dLft)
	{
		m_dSprStressN = -m_dStrainN * m_dSprKn; 
	}
	else if(m_dLN <= m_dLMax)
	{
		m_dSprStressN = - m_dEleFt + m_dEleFt * (m_dLN-m_dLft)/(m_dLMax-m_dLft);
	}
	else
	{
		m_dSprStressN = 0.0;   //且法向弹簧破坏了
	}
}
void CSpring::FailMode(int iFailMode)    //弹簧发生破坏后的一系列处理
{
	int i,j,n;
	i = m_pSprNoList->GetAt(1)->GetOfEle();   //本单元本弹簧的另一端的R单元在单元总体链表中的编号
	j = m_pSprNoList->GetAt(1)->GetNum();     //R单元弹簧在R单元弹簧列表中的编号

	CElement *pElement = GetModel()->m_pEleList->GetAt(i);
	n = pElement->GetSprList()->GetSize();    //R单元的弹簧总数

	m_bIntact = false;
	m_pEle->GetSprNoList()->GetAt(m_iNum)->SetState(false); //标定L单元对应弹簧点上的弹簧断开了，已经没有弹簧
	m_iFailMode = iFailMode;

	BOOL bCheck = FALSE;
	for(int k=0; k<n; k++)
	{
		CSpring *pSpring = pElement->GetSprList()->GetAt(k);
		int RK = pSpring->GetNum();
		if( RK == j)
		{
			pSpring->SetState(false);
			pElement->GetSprNoList()->GetAt(j)->SetState(false);   //标定R单元对应弹簧点上的弹簧断开了，已经没有弹簧
			pSpring->SetFailMode(m_iFailMode);
			bCheck = TRUE;
		}
	}
	if(!bCheck)
	{
		AfxMessageBox("模型错误！");
	}
}
void CSpring::FailMode0(int iFailMode)
{
	m_bIntact = true;
	m_iFailMode = iFailMode;
}

void CSpring::UpdateSprState()    //此步，只更新弹簧状态，不更新力
{
	if(m_iType==2)  //加载板和底板的连接弹簧（一直不会坏）
	{
		FailMode0(CSpring::NEITHER);
		return;
	}	
	else            //砂浆间弹簧和界面弹簧  弹簧强度不一样，但是表达形式一样 
	{
		if( ( m_dLN >= 0) && (fabs(fabs(m_dSprStressT)-m_dShearStressMax) < 1e-3 ) ) //法向受拉和切向受剪同时破坏 拉剪
		{
			FailMode(CSpring::BOTH);
			return;
		}
		else if( m_dLN >= m_dLft)  //法向弹簧受拉破坏,超过荷载对应的变形，力马上变为零   受拉
		{
			FailMode(CSpring::TESION);
			return;
		}
		else if( m_dLN <= -m_dLfc)  //   受压
		{
			FailMode(CSpring::PRESSURE);
			return;
		}
		//		else if( fabs(fabs(m_dLT)-2*m_dLfs) < 1e-10)
		//		{
		//			FailMode(CSpring::SHEAR);
		//		    return;
		//		}
		else if( fabs(fabs(m_dSprStressT)-m_dShearStressMax) < 1e-3 )    //压剪
		{
			FailMode(CSpring::SHEAR);
			return;
		}
		else
		{
			FailMode0(CSpring::NEITHER);
			return;
		}			
	}
}
CEleList * CSpring::GetEleList()   //弹簧的两个端点所在的单元的列表
{
	return m_pSprEleList;
}

CSprNoList * CSpring::GetSprNoList()   //弹簧的两个端点/弹簧点列表
{
	return m_pSprNoList;
}
BOOL CSpring::GetState()
{
	return m_bIntact;
}

void CSpring::SetState(BOOL State)
{
	m_bIntact=State;
}
int CSpring::GetFailMode()
{
	return m_iFailMode;
}
void CSpring::SetFailMode(int FailMode)
{
	m_iFailMode = FailMode;
}
double CSpring::GetSprStressN()   //返回弹簧力
{
	return m_dSprStressN;
}
double CSpring::GetSprStressT()   //返回弹簧力
{
	return m_dSprStressT;
}

C3DVec CSpring::GetSprStress()   //返回弹簧应力
{
	return m_VecSprStress;
}

void CSpring::SetSprStressLevel(int Leveln)      //弹簧应力等级
{
	m_iSprStressLevel = Leveln;
}

int CSpring::GetSprStressLevel()
{
	return m_iSprStressLevel;
}

C3DVec CSpring::GetSprForce()   //返回弹簧应力
{
	return m_VecSprF;
}
int CSpring::GetNum()
{
	return m_iNum;
}
void CSpring::SetSprType(int iType)
{
	m_iType=iType;
}
int CSpring::GetSprType()
{
	return m_iType;
}
double CSpring::GetFaceArea()
{
	return m_darea;
}

double CSpring::GetSprKn()
{
	return m_dSprKn;
}
double CSpring::GetSprKt()
{
	return m_dSprKt;
}

double CSpring::GetH1()
{
	return m_dh1;
}
double CSpring::GetH2()
{
	return m_dh2;
}
double CSpring::GetSprN()   //获得弹簧变形长度
{
	return m_dLPreN;
}
double CSpring::GetSprT()   //获得弹簧变形长度
{
	return m_dLPreT;
}
C3DVec CSpring::GetSprUN()
{
	return m_UniVecN;
}
C3DVec CSpring::GetSprUT()
{
	return m_UniVecT;
}


void CSpring::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	if(ar.IsStoring())
	{    
		ar<<m_iNum<<m_iType<<m_iOfEle<<m_iTNum<<m_iFailMode<<m_iSprStressLevel
			<<m_dLPre<<m_dLPreN<<m_dLPreT<<m_dLN<<m_dLT<<m_dStrainN<<m_dStrainT<<m_dSprFn<<m_dSprFt
			<<m_dSprStressN<<m_dSprStressT<<m_dh1<<m_dh2<<m_darea<<m_dIniDelta<<m_dContactKn<<m_dContactKt
			<<m_dSprKn<<m_dSprKt<<m_dEleFt<<m_dEleFc<<m_dLMax<<m_dLfc<<m_dLft<<m_dLfs<<m_dShearStressMax
			<<m_bIntact<<m_bMaterAttribute;
	}
	else
	{
		ar>>m_iNum>>m_iType>>m_iOfEle>>m_iTNum>>m_iFailMode>>m_iSprStressLevel
			>>m_dLPre>>m_dLPreN>>m_dLPreT>>m_dLN>>m_dLT>>m_dStrainN>>m_dStrainT>>m_dSprFn>>m_dSprFt
			>>m_dSprStressN>>m_dSprStressT>>m_dh1>>m_dh2>>m_darea>>m_dIniDelta>>m_dContactKn>>m_dContactKt
			>>m_dSprKn>>m_dSprKt>>m_dEleFt>>m_dEleFc>>m_dLMax>>m_dLfc>>m_dLft>>m_dLfs>>m_dShearStressMax
			>>m_bIntact>>m_bMaterAttribute;
	}
	m_pSprEleList->Serialize(ar); 
	m_pSprNoList->Serialize(ar);
	m_pMaterial->Serialize(ar);
	m_pEle->Serialize(ar);
	m_UniVecN.Serialize(ar);
	m_UniVecT.Serialize(ar);
	m_VecSprF.Serialize(ar);
	m_VecSprStress.Serialize(ar);
}



// CSpring member functions


///////////////////////////////////////////////////////////////////////////////////////////
void CSprList::DeepRemoveAll()
{
	ASSERT_VALID(this);

	int index = m_nSize;
	while (index--) {
		delete this->ElementAt(index);
	}
	CSprListBase::RemoveAll();
}
