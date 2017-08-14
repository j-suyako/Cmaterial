// Material.cpp : implementation file
//
#include "stdafx.h"
#include "3D-SCFDEM.h"
#include "Material.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_SERIAL(CMaterial,CObject,1)

CMaterial::CMaterial()
{
	m_dMortarE =0.0;
	m_dMortarFc =0.0;
	m_dMortarFt =0.0;
	m_dMortarV =0.0;

	m_dInterfaceE =0.0;
	m_dInterfaceFc =0.0;
	m_dInterfaceFt =0.0;
	m_dInterfaceV =0.0;

	m_dMortarDensity =0.0;   //砂浆密度
	m_dAggDensity =0.0;    //骨料密度

	m_dAggE =0.0;
	m_dAggV =0.0;	
	m_dBondRatio =3.0;// 加载端摩擦约束系数（切向砂浆弹簧刚度）

	m_dMorEleFt =0.0;    //砂浆单元抗拉强度; 最大拉应力
	m_dMorEleFc =0.0;    //砂浆单元抗压强度
	m_dMorEleE =0.0;     //砂浆单元弹性模量;
	m_dMorEleV =0.0;     //砂浆单元泊松比;

	m_dIntEleFt =0.0;    //界面单元抗拉强度; 最大拉应力
	m_dIntEleFc =0.0;    //界面单元抗压强度
	m_dIntEleE =0.0;     //界面单元弹性模量;
	m_dIntEleV =0.0;     //界面单元泊松比;

	m_dAggEleE =0.0;
	m_dAggEleV =0.0;

	m_dMorSprKn=0.;    //砂浆弹簧法向刚度系数;
	m_dMorSprKt=0.;    //砂浆弹簧切向刚度系数;
	m_dIntSprKn=0.;    //界面弹簧法向刚度系数;
	m_dIntSprKt=0.;    //界面弹簧切向刚度系数;
	m_dAggSprKn =0.0;    //骨料弹簧法向刚度系数;
	m_dAggSprKt =0.0;    //骨料弹簧切向刚度系数;

	m_dBondKn =0.0;
	m_dBondKt =0.0;
}

CMaterial::~CMaterial()
{
}

void CMaterial::Serialize(CArchive &ar)
{
	CObject::Serialize(ar);

	if(ar.IsStoring())
	{    
		ar<<m_dMortarE<<m_dMortarV<<m_dMortarFt<<m_dMortarFc<<m_dInterfaceFt<<m_dInterfaceFc<<m_dMortarDensity
			<<m_dAggDensity<<m_dBondRatio<<m_dInterfaceE<<m_dInterfaceV<<m_dAggE<<m_dAggV
			<<m_dMorEleFt<<m_dMorEleFc<<m_dMorEleE<<m_dMorEleV<<m_dIntEleFt<<m_dIntEleFc<<m_dIntEleE<<m_dIntEleV
			<<m_dAggEleE<<m_dAggEleV<<m_dMorSprKn<<m_dMorSprKt<<m_dIntSprKn<<m_dIntSprKt<<m_dAggSprKn<<m_dAggSprKt
			<<m_dBondKn<<m_dBondKt<<m_dContactKn<<m_dContactKt;
	}
	else
	{
		ar>>m_dMortarE>>m_dMortarV>>m_dMortarFt>>m_dMortarFc>>m_dInterfaceFt>>m_dInterfaceFc>>m_dMortarDensity
			>>m_dAggDensity>>m_dBondRatio>>m_dInterfaceE>>m_dInterfaceV>>m_dAggE>>m_dAggV
			>>m_dMorEleFt>>m_dMorEleFc>>m_dMorEleE>>m_dMorEleV>>m_dIntEleFt>>m_dIntEleFc>>m_dIntEleE>>m_dIntEleV
			>>m_dAggEleE>>m_dAggEleV>>m_dMorSprKn>>m_dMorSprKt>>m_dIntSprKn>>m_dIntSprKt>>m_dAggSprKn>>m_dAggSprKt
			>>m_dBondKn>>m_dBondKt>>m_dContactKn>>m_dContactKt;
	}
}

void CMaterial::SetMortarFt(double MortarFt)
{ 
	m_dMortarFt = MortarFt;
}

void CMaterial::SetMortarFc(double MortarFc)
{
	m_dMortarFc = MortarFc;
}

void CMaterial::SetMortarE(double MortarE)
{
	m_dMortarE = MortarE;
}

void CMaterial::SetMortarV(double MortarV)
{
	m_dMortarV = MortarV;
}

void CMaterial::SetInterfaceFt(double InterfaceFt)
{
	m_dInterfaceFt = InterfaceFt;
}

void CMaterial::SetInterfaceFc(double InterfaceFc)
{
	m_dInterfaceFc = InterfaceFc;
}

void CMaterial::SetInterfaceE(double InterfaceE)
{
	m_dInterfaceE = InterfaceE;
}

void CMaterial::SetInterfaceV(double InterfaceV)	
{
	m_dInterfaceV = InterfaceV;
}

void CMaterial::SetAggE(double AggE)
{
	m_dAggE = AggE;
}

void CMaterial::SetAggV(double AggV)
{
	m_dAggV = AggV;
}

void CMaterial::SetMortarDensity(double MortarDensity)
{
	m_dMortarDensity = MortarDensity;
}

void CMaterial::SetAggDensity(double AggDensity)
{
	m_dAggDensity = AggDensity;
}

void CMaterial::SetBondRatio(double BondRatio)
{
	m_dBondRatio = BondRatio;
}

void CMaterial::SetElePara()
{	
	  //m_dMorEleFt = m_dMortarFt;    //砂浆单元抗拉强度; 最大拉应力
	  //m_dMorEleFc = m_dMortarFc;    //砂浆单元抗压强度
	  //m_dMorEleE = m_dMortarE;     //砂浆单元弹性模量;
	  //m_dMorEleV = m_dMortarV;     //砂浆单元泊松比;

	//数值模拟得到的宏微观转化关系放入到程序内部，运行程序时只需在材料参数中输入宏观材性试验的结果即可。
	m_dMorEleV = 0.5*pow(m_dMortarV,3)+11.56*pow(m_dMortarV,2)-1.03*m_dMortarV;
	m_dMorEleE = m_dMortarE*(-28.78*pow(m_dMortarV,3)-12.88*pow(m_dMortarV,2)+4.52*m_dMortarV+1.0);

	//m_dMorEleFc = 0.0003*pow(m_dMortarFc,3)+0.0136*m_dMortarFc*m_dMortarFc+3.2519*m_dMortarFc;   //(Juny->)暂时不采用转换关系2014.11.28
	m_dMorEleFc = 1.817* pow(m_dMortarFc,1.17);
	m_dIntEleFc = 2.498* pow( m_dInterfaceFc,1.008);       //界面单元抗压强度,以砂浆的抗压强度为无量纲的准则

	//以下三行适用于混凝土受拉、受压、水泥砂浆受压
	double scale = m_dMorEleFc / m_dMortarFc;
	m_dMorEleFt = scale * m_dMortarFt;
	m_dIntEleFt = scale * m_dInterfaceFt;    //界面单元抗拉强度; 最大拉应力  (Juny->)暂时去掉2014.11.28	

	//以下两行仅在水泥砂浆受拉模拟时采用比较合适
		//m_dMorEleFt = 1.92 * m_dMortarFt;
		//m_dIntEleFt = 1.65 * m_dInterfaceFt;    //界面单元抗拉强度; 最大拉应力   

	m_dIntEleV = 0.5*pow(m_dInterfaceV,3)+11.56*pow(m_dInterfaceV,2)-1.03*m_dInterfaceV; //界面单元泊松比;
	m_dIntEleE = m_dInterfaceE*(-28.78*pow(m_dInterfaceV,3)-12.88*pow(m_dInterfaceV,2)+4.52*m_dInterfaceV+1);   //界面单元弹性模量;    (Juny->)暂时去掉

	m_dAggEleE = m_dAggE;    //骨料单元弹性模量;
	m_dAggEleV = m_dAggV;    //骨料单元泊松比;

	SetSprStiff(m_dMorEleE, m_dMorEleV, m_dBondRatio);    
	SetIntStiff(m_dIntEleE, m_dIntEleV);
	SetAggStiff(m_dAggEleE, m_dAggEleV);
}
void CMaterial::SetSprStiff(double MorEleE, double MorEleV, double BondRatio)  //砂浆弹簧刚度系数
{
	m_dMorSprKn = MorEleE * (1.0-MorEleV) / (1.0+MorEleV) /(1.0-2.0*MorEleV)  ;
	m_dMorSprKt = MorEleE / (1.0+MorEleV) /2.0;

	m_dContactKn = MorEleE;
	m_dContactKt = m_dContactKn /(1+MorEleV)*0.5;

	m_dBondKn = 1.0 * m_dMorSprKn;    //!!!
	m_dBondKt = BondRatio * m_dMorSprKt;    //!!!!
}

void CMaterial::SetIntStiff(double IntEleE, double IntEleV) //界面弹簧刚度系数
{
	m_dIntSprKn = IntEleE * (1.0-IntEleV) / (1.0+IntEleV) /(1.0-2.0*IntEleV) ;
	m_dIntSprKt = IntEleE / (1+IntEleV) /2.0 ;
}

void CMaterial::SetAggStiff(double AggEleE, double AggEleV)   //骨料弹簧刚度系数
{
	m_dAggSprKn = AggEleE * (1.0-AggEleV) / (1.0+AggEleV) /(1.0-2.0*AggEleV)  ;
	m_dAggSprKt = AggEleE / (1+AggEleV) / 2.0;
}


double CMaterial::GetMortarDensity()
{
	return m_dMortarDensity;
}
double CMaterial::GetAggDensity()
{
	return m_dAggDensity;
}

double CMaterial::GetMortarFt()
{
	return m_dMortarFt;
}
double CMaterial::GetMortarFc()
{
	return m_dMortarFc;
}

double CMaterial::GetMortarE()
{
	return m_dMortarE;
}
double CMaterial::GetMortarV()
{
	return m_dMortarV;
}

double CMaterial::GetInterfaceFt()
{
	return m_dInterfaceFt;
}
double CMaterial::GetInterfaceFc()
{
	return m_dInterfaceFc;
}

double CMaterial::GetInterfaceV()
{
	return m_dInterfaceV;
}
double CMaterial::GetInterfaceE()
{
	return m_dInterfaceE;
}


double CMaterial::GetMorEleFt()
{
	return m_dMorEleFt;
}
double CMaterial::GetMorEleFc()
{
	return m_dMorEleFc;
}

double CMaterial::GetMorEleE()
{
	return m_dMorEleE;
}
double CMaterial::GetMorEleV()
{
	return m_dMorEleV;
}

double CMaterial::GetIntEleFt()
{
	return m_dIntEleFt;
}
double CMaterial::GetIntEleFc()
{
	return m_dIntEleFc;
}

double CMaterial::GetIntEleE()
{
	return m_dIntEleE;
}
double CMaterial::GetIntEleV()
{
	return m_dIntEleV;
}

double CMaterial::GetAggEleE()
{
	return m_dAggEleE;
}
double CMaterial::GetAggEleV()
{
	return m_dAggEleV;
}


//弹簧参数
double CMaterial::GetMorSprKn()
{
	return m_dMorSprKn;
}
double CMaterial::GetMorSprKt()
{
	return m_dMorSprKt;
}

double CMaterial::GetContactKn()
{
	return m_dContactKn;
}
double CMaterial::GetContactKt()
{
	return m_dContactKt;
}

double CMaterial::GetBondKn()
{
	return m_dBondKn;
}
double CMaterial::GetBondKt()
{
	return m_dBondKt;
}

double CMaterial::GetIntSprKn()
{
	return m_dIntSprKn;
}
double CMaterial::GetIntSprKt()
{
	return m_dIntSprKt;
}

double CMaterial::GetAggSprKn()
{
	return m_dAggSprKn;
}
double CMaterial::GetAggSprKt()
{
	return m_dAggSprKt;
}



// CMaterial member functions
