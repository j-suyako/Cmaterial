
// CMaterial command target
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATERIAL_H__2B918EF0_6664_432A_8918_1D3BDB58D7AD__INCLUDED_)
#define AFX_MATERIAL_H__2B918EF0_6664_432A_8918_1D3BDB58D7AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMaterial : public CObject
{
	DECLARE_SERIAL(CMaterial)
private:
	//宏观材料参数，其值有Model中材料参数定义对话框进行函数调用后赋值
	double m_dMortarE;
	double m_dMortarV;
	double m_dMortarFt;
	double m_dMortarFc;

	double m_dInterfaceFt;
	double m_dInterfaceFc;

	double m_dMortarDensity;   //砂浆密度（和骨料密度均是体积密度，与二维程序不同，注意差别）(Juny 2014.8.27)
	double m_dAggDensity;    //骨料密度
	double m_dBondRatio;// 加载端摩擦约束系数（切向砂浆弹簧刚度）

	double m_dInterfaceE;	
	double m_dInterfaceV;

	double m_dAggE;
	double m_dAggV;	

	//单元的参数,目前为直接让其等于输出的宏观材料参数
	double m_dMorEleFt;    //砂浆单元抗拉强度; 最大拉应力
	double m_dMorEleFc;    //砂浆单元抗压强度
	double m_dMorEleE;     //砂浆单元弹性模量;
	double m_dMorEleV;     //砂浆单元泊松比;

	double m_dIntEleFt;    //界面单元抗拉强度; 最大拉应力
	double m_dIntEleFc;    //界面单元抗压强度

	double m_dIntEleE;     //界面单元弹性模量;
	double m_dIntEleV;     //界面单元泊松比;

	double m_dAggEleE;
	double m_dAggEleV;

	//弹簧刚度系数
	double m_dMorSprKn;    //砂浆弹簧法向刚度系数;
	double m_dMorSprKt;    //砂浆弹簧切向刚度系数;
	double m_dIntSprKn;    //界面弹簧法向刚度系数;
	double m_dIntSprKt;    //界面弹簧切向刚度系数;
	double m_dAggSprKn;    //骨料弹簧法向刚度系数;
	double m_dAggSprKt;    //骨料弹簧切向刚度系数;

	double m_dBondKn;
	double m_dBondKt;

	// 接触刚度 
	double m_dContactKn;  //法向接触刚度；
	double m_dContactKt;  //切向接触刚度；暂时在材料设定时，认为这两个刚度的值跟弹簧刚度一样；在判断具体的接触类型时，在乘以系数成为点接触或者线接触求接触力时的参数

public:
	CMaterial();
	virtual ~CMaterial();

	void Serialize(CArchive &ar);

	void SetElePara();//宏观微观材料参数转化关系,暂时取微观参数等于对话框直接输入的参数，
	                  //待得出规律后在此处完善转化关系，界时对话框直接输入宏观材料参数不必在乘系数之类
	void SetSprStiff(double MorEleE, double MorEleV, double BondRatio);   //砂浆弹簧刚度系数
	void SetIntStiff(double IntEleE, double IntEleV);  //界面弹簧刚度系数
	void SetAggStiff(double AggEleE, double AggEleV);  //骨料弹簧刚度系数


	void SetMortarFt(double MortarFt);
	void SetMortarFc(double MortarFc);
	void SetMortarE(double MortarE);
	void SetMortarV(double MortarV);

	void SetInterfaceFt(double InterfaceFt);
	void SetInterfaceFc(double InterfaceFc);
	void SetInterfaceE(double InterfaceE);
	void SetInterfaceV(double InterfaceV);	

	void SetAggE(double AggE);
	void SetAggV(double AggV);

	void SetMortarDensity(double MortarDensity);
	void SetAggDensity(double AggDensity);
	void SetBondRatio(double BondRatio);	

	double GetMortarFt();
	double GetMortarFc();
	double GetMortarE();
	double GetMortarV();

	double GetInterfaceFt();
	double GetInterfaceFc();
	double GetInterfaceE();
	double GetInterfaceV();	

	double GetAggE() { return m_dAggE; }
	double GetAggV() { return m_dAggV; }	

	double GetMortarDensity();
	double GetAggDensity();
	double GetBondRatio() { return m_dBondRatio; }

	double GetMorEleFt();
	double GetMorEleFc();
	double GetMorEleE();
	double GetMorEleV();

	double GetIntEleFt();
	double GetIntEleFc();
	double GetIntEleE();
	double GetIntEleV();

	double GetAggEleE();
	double GetAggEleV();

	double GetMorSprKn();
	double GetMorSprKt();
	double GetIntSprKn();
	double GetIntSprKt();
	double GetAggSprKn();
	double GetAggSprKt();

	double GetContactKn();
	double GetContactKt();

	double GetBondKn();
	double GetBondKt();
};

#endif // !defined(AFX_MATERIAL_H__2B918EF0_6664_432A_8918_1D3BDB58D7AD__INCLUDED_)
