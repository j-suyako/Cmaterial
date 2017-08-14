void CModel::MaterialDefine() 
{
	CMaterDlg dlg;
	dlg.m_dConcretF = 30.0;
	dlg.m_dMortarF = 25.0;

	dlg.m_dMortarD=2142.0e-9;   //!!!  1350e-9 kg/mm3  原来为10
	dlg.m_dAggD=2800.0e-9;

	dlg.m_dMortarE = 26300;
	dlg.m_dMortarFc = 29.9;
	dlg.m_dMortarFt= 3.55;
	dlg.m_dMortarV = 0.213;

	dlg.m_dInterfaceFc = 29.9;
	dlg.m_dInterfaceFt= 1.78;

	dlg.m_dInterfaceE = 21040;
	dlg.m_dInterfaceV = 0.213;

	dlg.m_dBondRatio = 0.0;

	dlg.m_dAggE = 50000;
	dlg.m_dAggV = 0.25;
	dlg.m_iAggRigid = m_iAggRigid;

	if (dlg.DoModal() == IDOK)
	{
		m_bMatVariation = dlg.m_bMatVariation;
		m_iAggRigid = dlg.m_iAggRigid;		

		m_iDistributionType = dlg.m_iDistributionType;
		m_dNormal_d = dlg.m_dNormal;
		m_dWeibull_a = dlg.m_dWeibull;

		m_pMaterial->SetMortarE(dlg.m_dMortarE);
		m_pMaterial->SetMortarFc(dlg.m_dMortarFc);
		m_pMaterial->SetMortarFt(dlg.m_dMortarFt);
		m_pMaterial->SetMortarV(dlg.m_dMortarV);

		m_pMaterial->SetInterfaceFc(dlg.m_dInterfaceFc);
		m_pMaterial->SetInterfaceFt(dlg.m_dInterfaceFt);	
		m_pMaterial->SetInterfaceE(dlg.m_dInterfaceE);
		m_pMaterial->SetInterfaceV(dlg.m_dInterfaceV);

		m_pMaterial->SetAggE(dlg.m_dAggE);
		m_pMaterial->SetAggV(dlg.m_dAggV);

		m_pMaterial->SetAggDensity(dlg.m_dAggD);
		m_pMaterial->SetMortarDensity(dlg.m_dMortarD);
		m_pMaterial->SetBondRatio(dlg.m_dBondRatio);        

		m_pMaterial->SetElePara();
	}

	for(int f=0;f<m_pEleList->GetSize();f++)
	{			
		m_pEleList->GetAt(f)->SetEleMass();	
	}
}
