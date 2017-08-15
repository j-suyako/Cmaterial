#pragma once

// CSpring command target
#include <afxtempl.h>
#include "Material.h"
#include "SprNode.h"

class CElement;
class CEleList;

class CSpring : public CObject
{
	DECLARE_SERIAL(CSpring)
private:
	int m_iNum;    //弹簧编号同时也是弹簧所在单元的弹簧点的编号  ---弹簧0节点所在单元上的弹簧节点的编号
	int m_iType;   //弹簧类型号,0表示砂浆弹簧，1表示界面弹簧, 2 表示加载边上的
	int m_iOfEle;  //所属的单元号
	int m_iTNum;   //弹簧在整体弹簧列表中的编号

	CMaterial * m_pMaterial;
	CElement * m_pEle;     //本弹簧起点所在的单元
	CEleList * m_pSprEleList; //本弹簧起点和终点所在的单元链表的指针  //未定义具体set
	CSprNoList * m_pSprNoList;//指向两个端点组成的链表的指针，表中第一个点在正在放松的单元上，第二点在另一个单元上;

	C3DVec m_UniVecN;    //弹簧的法向矢量  // 如在弹簧组中则认为是 法向弹簧的方向矢量 
	C3DVec m_UniVecT;    // 弹簧的切向矢量 //

	double m_dLPre;  //弹簧点两端的原来距离(也即两个单元上各自弹簧点之间的距离)
	double m_dLPreN, m_dLPreT; //法向和切向弹簧原长;在GetCalcuSprForce()利用
	double m_dLN;   //法向弹簧变形
	double m_dLT;   //切向弹簧变形 
	double m_dStrainN;  //法向弹簧正应变(LC)
	double m_dStrainT;  //法向弹簧剪应变(LC)

	double m_dSprFn;  //法向弹簧力(局部坐标系下的)  N/mm
	double m_dSprFt;  //切向弹簧力(LC)              N/mm
	double m_dSprStressN;  //法向弹簧应力   N/mm2  //
	double m_dSprStressT;  //切向弹簧应力   N/mm2  //
	C3DVec m_VecSprF;   //弹簧合力在整体坐标系的值，用向量表示   N/mm
	C3DVec m_VecSprStress;  //切向弹簧力在整体坐标系的值，用向量表示 N/mm2   ////

	double m_dh1, m_dh2;  //弹簧连接的两个单元的质心到梁单元的公共面（不是接触判断的共公面）的垂直距离；
	double m_darea;   //弹簧所在公共面的面积（弹簧代表面积） 

	BOOL m_bIntact;  //标识弹簧是否完好，初值为true
	enum _FailMode
	{
		NEITHER = 0,                // 0为两个弹簧都未破坏 （未破坏则全为普通弹簧本构）
		TESION = 1,					// 1为拉坏，即法向弹簧发生破坏 (破坏后法向的本构转为接触本构)
		PRESSURE = 2,				// 2为压坏，即法向弹簧发生破坏 (破坏后法向的本构转为接触本构)	
		SHEAR = 3,					// 3为剪坏，即切向弹簧发生破坏 （破坏后切向的本构转为contact constitute）
		BOTH = 4,                   // 4为两个弹簧都发生了破坏  （破坏后法向和切向的本构均为接触本构）		
	};

	int m_iFailMode;  //破坏方式 0为完好， 1为拉坏，2为压坏 , 3为剪坏， 4为同时拉坏和剪坏

	BOOL m_bMaterAttribute;//弹簧是否已经定义材料属性 true为已经定义 

	double m_dIniDelta;//自重作用下初始变形量；

	double m_dContactKn;   //弹簧断开以后的法向接触刚度，可以理解为虚拟的弹簧
	double m_dContactKt;   //弹簧断开以后的切向接触刚度

	double m_dSprKn;//弹簧法向刚度
	double m_dSprKt;//弹簧切向刚度
	double m_dEleFt;//单元抗拉强度
	double m_dEleFc;//单元抗压强度，主要值为砂浆的轴心抗压强度，主要用于压剪耦合的破坏准则中
	double m_dLMax; //最大裂缝宽度，破坏准则中
	double m_dLfc;  //破坏准则中，与最大抗压强度对应的弹簧变形（由应变值求出的）
	double m_dLft;  //破坏准则中，与最大抗拉强度对应的弹簧变形
	double m_dLfs;  //破坏准则中，与最大抗剪强度对应的弹簧变形
	double m_dShearStressMax;  //最大剪应力

	int m_iSprStressLevel;         //弹簧应力等级
public:
	void Serialize(CArchive &ar);
	CSpring();
	CSpring(CElement * pELe,CMaterial *pMaterial, int iNum, int iType,CSprNoList *pSprNoList, CEleList *pEleList);
	void Initial();
	CSpring(int iTNum, int iNum, CSprNoList *pSprNoList, CMaterial *pMaterial);
	CSpring(int iTNum, CSprNoList *pSprNoList, CEleList *pEleList);
	virtual ~CSpring();

	double AverageRandom(double min, double max);   //生成随机数
	double Weibull(double a, double b); //生成weibull分布的随机数 
	double Normal(double a, double b); //生成正太分布，a为均质， b为均方差double AverageRandom(double min, double max);   //生成随机数

	int GetNum();
	
	C3DVec GetSprUN();
	C3DVec GetSprUT();

	void SetState(BOOL State);
	BOOL GetState();//返回弹簧的当前状态（是否完好）
	void SetSprAttribute();
	void DistributionType(); //根据分布类型给弹簧赋值

	void SetMaterialState(BOOL State); //弹簧是否已经定义材料属性 true为已经定义 
	BOOL GetMaterialState();    
	void SetRSprMaterial(double SprKn, double SprKt, double EleFt, double EleFc, double LMax, double Lfc, double Lft); //给远端单元的同一根弹簧赋以同样的属性

	void SetFailMode(int FailMode);
	int GetFailMode();

	CSprNoList * GetSprNoList();
	CEleList * GetEleList();

	void SetSprType(int iType);
	int GetSprType();

	double GetFaceArea();
	double GetH1();
	double GetH2();

	double GetEleFt();
	double GetEleFc();

	double GetSprKn();
	double GetSprKt();

	C3DVec GetCalcuSprForce(CElement *pEle);//计算并返回弹簧力

	//分别讨论不同类别弹簧的弹簧力计算
	C3DVec CalcuForceOfType2();   //加载板弹簧
	C3DVec CalcuForceOfType0();   //砂浆弹簧
	C3DVec CalcuForceOfType1();   //界面弹簧

	void NormalSpringConstribute();
	void ShearSpringConstribute0();
	void ShearSpringConstribute1();
	void ContactConstribute();

	void NormalSpringConstribute_Old();//法向弹簧本构关系
	void ShearSpringConstribute_Old();//切向弹簧本构关系

	void FailMode(int iFailMode);
	void FailMode0(int iFailMode); //没有发生破坏
	void FailMode4(int iFailMode);

	void UpdateSprDir();
	void UpdateSprState();
	void UpdateSprStateAndForce();

	double GetSprStressN();//返回弹簧应力(单位为应力单位)
	double GetSprStressT();//返回弹簧应力(单位为应力单位)
	C3DVec GetSprStress(); //返回弹簧应力(单位为应力单位)
	C3DVec GetSprForce();  //返回弹簧力

	double GetSprN();      //返回弹簧上一步的变形值
	double GetSprT();

	void SetSprStressLevel(int Leveln);      //弹簧应力等级
	int GetSprStressLevel();
	
};

typedef CTypedPtrArray<CObArray, CSpring *> CSprListBase;   //弹簧列表

class CSprList: public CSprListBase
{
public:
	//	int DeepAppend(const CSprList& src);
	//	void DeepCopy(const CSprList& src);
	void DeepRemoveAll();
	//	void OutputToText(std::fstream &cout);
};
