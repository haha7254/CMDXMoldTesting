// MDXProcessPage.cpp : implementation file
//

#include "stdafx.h"
#include "CMDXMoldTesting.h"
#include "MDXProcessPage.h"
#include "afxdialogex.h"
#include "DataCenter.h"


// CMDXProcessPage dialog

IMPLEMENT_DYNAMIC(CMDXProcessPage, CDialog)

CMDXProcessPage::CMDXProcessPage(CWnd* pParent /*=NULL*/)
	: CDialog(CMDXProcessPage::IDD, pParent)
, m_iFillSel(0)
, m_iPackSel(0)
, m_pParent(pParent)
{

}

CMDXProcessPage::~CMDXProcessPage()
{
}

void CMDXProcessPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMDXProcessPage, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CMDXProcessPage::OnBnClickedButton1)
	ON_CBN_SELCHANGE(IDC_COMBO_FILLING_SECTION, &CMDXProcessPage::OnCbnSelchangeComboFillingSection)
	ON_CBN_SELCHANGE(IDC_COMBO_PACKING_SECTION, &CMDXProcessPage::OnCbnSelchangeComboPackingSection)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_VP, &CMDXProcessPage::OnDeltaposSpinVP)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INJECTION_PRESSURE, &CMDXProcessPage::OnDeltaposSpinInjectionPressure)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_FILLINT_TIME, &CMDXProcessPage::OnDeltaposSpinFillTime)
END_MESSAGE_MAP()


// CMDXProcessPage message handlers
BOOL CMDXProcessPage::OnInitDialog()
{
	InitVP();
	InitInjectionPressure();

	InitFillTime();
	InitVolumeExpansion();

	InitComboProcessFill();
	InitComboProcessPack();

	SetProfileFill();
	SetProfilePack();

	InitCoolTime();
	InitMoldOpenTime();
	InitCycleTime();
	InitResidenceTime();

	return TRUE;
}

void CMDXProcessPage::InitCoolTime()
{
	double max_part = DataCenter::getInstance().GetMaxPartThickness();
	m_dCoolTime = max_part * (1+2*max_part);

	CString strTemp("");
	strTemp.Format("%.1f", m_dCoolTime);
	GetDlgItem(IDC_EDIT_COOLING_TIME)->SetWindowText(strTemp);
}

void CMDXProcessPage::InitMoldOpenTime()
{
	m_dMoldOpenTime = 5.0;

	CString strTemp("");
	strTemp.Format("%.1f", m_dMoldOpenTime);
	GetDlgItem(IDC_EDIT_MOLDOPEN_TIME)->SetWindowText(strTemp);
}
	
void CMDXProcessPage::InitCycleTime()
{
	m_dCycleTime = m_dInjectionTime + m_dCoolTime + m_dMoldOpenTime;

	CString strTemp("");
	strTemp.Format("%.1f", m_dCycleTime);
	GetDlgItem(IDC_EDIT_CYCLE_TIME)->SetWindowText(strTemp);
}

void CMDXProcessPage::InitResidenceTime()
{
	double hrV = DataCenter::getInstance().GetHotRunnerVolume();
	m_dResidenceTime = (hrV+m_dPartVolume+m_dColdRunnerVolume)
						/(m_dPartVolume+m_dColdRunnerVolume)
						/m_dVolumeExpansion*m_dCycleTime/60;
	
	CString strTemp("");
	strTemp.Format("%.1f", m_dResidenceTime);
	GetDlgItem(IDC_EDIT_RESIDENCE_TIME)->SetWindowText(strTemp);
}

void CMDXProcessPage::InitVP()
{
	//從資料中心拿資料
	m_dDiaScrew = DataCenter::getInstance().GetDiaScrew();

	//更新顯示(VP切換位置)
	m_dVP = m_dDiaScrew * 0.25;
	m_dVP_step = m_dVP * 0.01;
	m_dVP_max = m_dVP * 1.1;
	m_dVP_min = m_dVP * 0.9;

	CString strTemp("");
	strTemp.Format("%.1f", m_dVP);
	GetDlgItem(IDC_EDIT_VP)->SetWindowText(strTemp);

	//計算螺桿截面積 (cm^2)
	m_dAreaScrew = 3.1415926 * m_dDiaScrew * m_dDiaScrew / 4 / 100;
}

void CMDXProcessPage::OnDeltaposSpinVP(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	//按一次箭頭可以+-預設值的1% 
	//至多調整+-10%

	//向上箭頭
	if(pNMUpDown->iDelta == -1 && m_dVP < m_dVP_max)  
    {
        m_dVP += m_dVP_step;
    }
	//向下箭頭
    else if(pNMUpDown->iDelta == 1 && m_dVP > m_dVP_min)  
    {
        m_dVP -= m_dVP_step;
    }

    CString strTemp("");
	strTemp.Format("%.1f", m_dVP);
	GetDlgItem(IDC_EDIT_VP)->SetWindowText(strTemp); 
	SetProfileFill();
	SetProfilePack();
}

void CMDXProcessPage::InitInjectionPressure()
{
	//從資料中心拿資料
	m_dMachinePressure = DataCenter::getInstance().GetMaxInjectionPressure();

	//更新顯示(射出壓力)
	m_dInjectionPressure = m_dMachinePressure*0.9;
	m_dInjectionPressure_step = m_dInjectionPressure * 0.01;
	m_dInjectionPressure_max = m_dInjectionPressure * 1.1;
	m_dInjectionPressure_min = m_dInjectionPressure * 0.9;

	CString strTemp("");
	strTemp.Format("%.1f", m_dInjectionPressure);
	GetDlgItem(IDC_EDIT_INJECTION_PRESSURE)->SetWindowText(strTemp); 
}

void CMDXProcessPage::OnDeltaposSpinInjectionPressure(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	//按一次箭頭可以+-預設值的1%
	//至多調整+-10%

	//向上箭頭
	if(pNMUpDown->iDelta == -1 && m_dInjectionPressure < m_dInjectionPressure_max)  
    {
        m_dInjectionPressure += m_dInjectionPressure_step;
    }
	//向下箭頭
    else if(pNMUpDown->iDelta == 1 && m_dInjectionPressure > m_dInjectionPressure_min)  
    {
        m_dInjectionPressure -= m_dInjectionPressure_step;
    }

    CString strTemp("");
	strTemp.Format("%.1f", m_dInjectionPressure);
	GetDlgItem(IDC_EDIT_INJECTION_PRESSURE)->SetWindowText(strTemp); 
	DataCenter::getInstance().SetMaxInjectionPressure(m_dInjectionPressure);
	SetProfilePack();
}

void CMDXProcessPage::InitVolumeExpansion()
{
	if (m_iMaterialSel != 2) //PC or PC+ABS:非結晶型 
	{
		m_dVolumeExpansion = 1.1;
	}
	else //PA:結晶型
	{
		m_dVolumeExpansion = 1.3;
	}
}

void CMDXProcessPage::InitFillTime()
{
	//從資料中心拿資料
	m_iMaterialSel = DataCenter::getInstance().GetMaterialSel();
	m_dPartVolume = DataCenter::getInstance().GetPartVolume();

	//更新顯示(充填時間)
	m_dInjectionTime = InjectionTimeLookUpTable();
	m_dInjectionTime_step = m_dInjectionTime * 0.01;
	m_dInjectionTime_max = m_dInjectionTime * 1.1;
	m_dInjectionTime_min = m_dInjectionTime * 0.9;

	CString strTemp("");
	strTemp.Format("%.1f", m_dInjectionTime);
	GetDlgItem(IDC_EDIT_FILLINT_TIME)->SetWindowText(strTemp); 
}

void CMDXProcessPage::OnDeltaposSpinFillTime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	//按一次箭頭可以+-0.1
	//至多調整+-10%

	//向上箭頭
	if(pNMUpDown->iDelta == -1 && m_dInjectionTime < m_dInjectionTime_max)  
    {
        m_dInjectionTime += 0.1;
    }
	//向下箭頭
    else if(pNMUpDown->iDelta == 1 && m_dInjectionTime > m_dInjectionTime_min)  
    {
        m_dInjectionTime -= 0.1;
    }

    CString strTemp("");
	strTemp.Format("%.1f", m_dInjectionTime);
	GetDlgItem(IDC_EDIT_FILLINT_TIME)->SetWindowText(strTemp); 
	
	SetProfileFill();
	InitCycleTime();
	InitResidenceTime();
}

double CMDXProcessPage::InjectionTimeLookUpTable()
{
	std::vector<double> LowVis  (10, 0);    // 10 double with value 0
	std::vector<double> MidVis  (LowVis);	// a copy of LowVis
	std::vector<double> HighVis (LowVis);   

	LowVis[0] = (0.2+0.4) / 2;
	LowVis[1] = (0.4+0.5) / 2;
	LowVis[2] = (0.5+0.6) / 2;
	LowVis[3] = (0.6+0.8) / 2;
	LowVis[4] = (0.8+1.2) / 2;
	LowVis[5] = (1.2+1.8) / 2;
	LowVis[6] = (1.8+2.6) / 2;
	LowVis[7] = (2.6+3.5) / 2;
	LowVis[8] = (3.5+4.6) / 2;
	LowVis[9] = (4.6+6.5) / 2;

	MidVis[0] = (0.25+0.5) / 2;
	MidVis[1] = (0.5+0.6) / 2;
	MidVis[2] = (0.6+0.75) / 2;
	MidVis[3] = (0.75+1.0) / 2;
	MidVis[4] = (1.0+1.5) / 2;
	MidVis[5] = (1.5+2.2) / 2;
	MidVis[6] = (2.2+3.2) / 2;
	MidVis[7] = (3.2+4.4) / 2;
	MidVis[8] = (4.4+6.0) / 2;
	MidVis[9] = (6.0+8.0) / 2;

	HighVis[0] = (0.3+0.6) / 2;
	HighVis[1] = (0.6+0.75) / 2;
	HighVis[2] = (0.75+0.9) / 2;
	HighVis[3] = (0.9+1.2) / 2;
	HighVis[4] = (1.2+1.8) / 2;
	HighVis[5] = (1.8+2.7) / 2;
	HighVis[6] = (2.7+4.0) / 2;
	HighVis[7] = (4.0+5.4) / 2;
	HighVis[8] = (5.4+7.2) / 2;
	HighVis[9] = (7.2+9.5) / 2;

	// part volume
	int idxV = 0;
	if (m_dPartVolume < 8)
	{
		idxV = 0;
	}
	else if (m_dPartVolume < 15)
	{
		idxV = 1;
	}
	else if (m_dPartVolume < 30)
	{
		idxV = 2;
	}
	else if (m_dPartVolume < 50)
	{
		idxV = 3;
	}
	else if (m_dPartVolume < 80)
	{
		idxV = 4;
	}
	else if (m_dPartVolume < 120)
	{
		idxV = 5;
	}
	else if (m_dPartVolume < 180)
	{
		idxV = 6;
	}
	else if (m_dPartVolume < 250)
	{
		idxV = 7;
	}
	else if (m_dPartVolume < 350)
	{
		idxV = 8;
	}
	else //part volume >= 350
	{
		idxV = 9;
	}

	// material type
	if (m_iMaterialSel == 0) //PC:high viscosity
	{
		return HighVis[idxV];
	}
	else if (m_iMaterialSel == 1) //PC+ABS:midium viscosity
	{
		return MidVis[idxV];
	}
	else //PA:low viscosity
	{
		return LowVis[idxV];
	}
}

void CMDXProcessPage::InitComboProcessFill()
{
	CString strFillType("");
	((CComboBox*)GetDlgItem(IDC_COMBO_FILLING_SECTION))->ResetContent();
	strFillType = "1";
	((CComboBox*)GetDlgItem(IDC_COMBO_FILLING_SECTION))->AddString(strFillType); 
	strFillType = "3";
	((CComboBox*)GetDlgItem(IDC_COMBO_FILLING_SECTION))->AddString(strFillType); 

	((CComboBox*)GetDlgItem(IDC_COMBO_FILLING_SECTION))->SetCurSel(0);
	m_iFillSel = 0; //單段充填
	/*m_iMachineSel = 0;
	SetMachineData();*/
}

void CMDXProcessPage::InitComboProcessPack()
{
	CString strPackType("");
	((CComboBox*)GetDlgItem(IDC_COMBO_PACKING_SECTION))->ResetContent();
	strPackType = "1";
	((CComboBox*)GetDlgItem(IDC_COMBO_PACKING_SECTION))->AddString(strPackType); 
	strPackType = "2";
	((CComboBox*)GetDlgItem(IDC_COMBO_PACKING_SECTION))->AddString(strPackType); 

	((CComboBox*)GetDlgItem(IDC_COMBO_PACKING_SECTION))->SetCurSel(0);
	m_iPackSel = 0; //單段保壓
	/*m_iMachineSel = 0;
	SetMachineData();*/
}

void CMDXProcessPage::DrawFillCurve()
{

}

void CMDXProcessPage::DrawPackCurve()
{

}

void CMDXProcessPage::OnPaint()
{
	CClientDC dc(this);

	dc.MoveTo(0,0);
	dc.LineTo(10,10);

}

void CMDXProcessPage::CalculateFillSpeed()
{
	//從資料中心拿資料
	m_dColdRunnerVolume = DataCenter::getInstance().GetColdRunnerVolume();
	m_dMaxScrewStroke = DataCenter::getInstance().GetMaxScrewStroke();

	//Y軸繪圖上下限需使用(暫時位置)、其實該放在"IF"中
	m_dFillSpeed_single = 10*(m_dPartVolume+m_dColdRunnerVolume)*m_dVolumeExpansion
								/m_dInjectionTime/m_dAreaScrew;

	if (m_iFillSel == 0) //單段充填
	{
		/*m_dFillSpeed_single = 10*(m_dPartVolume+m_dColdRunnerVolume)*m_dVolumeExpansion
								/m_dInjectionTime/m_dAreaScrew;*/
		m_dScrewStroke_single = m_dFillSpeed_single*m_dInjectionTime+m_dVP;
	}
	else //三段充填 2:4:1
	{
		int iSpeedRatio_1 = 2;
		int iSpeedRatio_2 = 4;
		int iSpeedRatio_3 = 1;

		//第一.二.三段螺桿行程距離
		double dScrew_1 = 10 * m_dColdRunnerVolume*m_dVolumeExpansion/m_dAreaScrew;
		double dScrew_2 = 10 * 0.95*m_dPartVolume*m_dVolumeExpansion/m_dAreaScrew;
		double dScrew_3 = 10 * 0.05*m_dPartVolume*m_dVolumeExpansion/m_dAreaScrew;

		//第一.二.三段速度
		m_dFillSpeed_multi_1 = iSpeedRatio_1/m_dInjectionTime
								*((dScrew_1/iSpeedRatio_1)+(dScrew_2/iSpeedRatio_2)+(dScrew_3/iSpeedRatio_3));
		m_dFillSpeed_multi_2 = iSpeedRatio_2/m_dInjectionTime
								*((dScrew_1/iSpeedRatio_1)+(dScrew_2/iSpeedRatio_2)+(dScrew_3/iSpeedRatio_3));
		m_dFillSpeed_multi_3 = iSpeedRatio_3/m_dInjectionTime
								*((dScrew_1/iSpeedRatio_1)+(dScrew_2/iSpeedRatio_2)+(dScrew_3/iSpeedRatio_3));

		//第一.二.三段螺桿位置
		m_dScrewStroke_1 = m_dVP + dScrew_1 + dScrew_2 + dScrew_3;
		m_dScrewStroke_2 = m_dVP + dScrew_2 + dScrew_3;
		m_dScrewStroke_3 = m_dVP + dScrew_3;
	}
}

void CMDXProcessPage::SetProfileFill()
{
	CRect rect;
	GetDlgItem(IDC_STATIC_FILLING)->GetWindowRect(&rect);
	GetDlgItem(IDC_STATIC_FILLING)->ScreenToClient(&rect) ;
	
	m_profileF.CreateChart(rect,GetDlgItem(IDC_STATIC_FILLING));
	m_profileF.SetProfileType(1);	//0:純線段 1:填滿
	m_profileF.SetXAxisDrawingMode(1);	//0:左小右大 1:左大右小
	m_profileF.SetXAxisCaption(_T("螺桿位置 (mm)"));
	m_profileF.SetYAxisCaption(_T("充填速度 (mm/s)"));
	m_profileF.SetReadOnly(true);
	m_profileF.SetColorForBk(RGB( 255,255,255 ) );  //背景顏色
	m_profileF.SetVPSwitchLocation(m_dVP); //VP位置
	m_profileF.SetProfileColor( RGB(0,255,0));  //多段線的顏色

	m_profileF.RemoveAllData();
	CalculateFillSpeed();

	//TRACE("%lf", m_dVP);

	//加入資料
	if (m_iFillSel == 0) //單段充填
	{
		m_profileF.SetXMinMaxValue(m_dVP, m_dScrewStroke_single);  //X軸上下限
		m_profileF.SetYMinMaxValue(0,m_dFillSpeed_single*1.5);  //Y軸上下限

		m_profileF.AddXYData(m_dScrewStroke_single,m_dFillSpeed_single);
		m_profileF.AddXYData(m_dVP,m_dFillSpeed_single);
	}
	else //三段充填 2:4:1
	{
		m_profileF.SetXMinMaxValue(m_dVP, m_dScrewStroke_1);  //X軸上下限
		m_profileF.SetYMinMaxValue(0,m_dFillSpeed_single*1.5);  //Y軸上下限

		m_profileF.AddXYData(m_dScrewStroke_1,m_dFillSpeed_multi_1);
		m_profileF.AddXYData(m_dScrewStroke_2,m_dFillSpeed_multi_1); //SetProfileType(1)
		m_profileF.AddXYData(m_dScrewStroke_2,m_dFillSpeed_multi_2);
		m_profileF.AddXYData(m_dScrewStroke_3,m_dFillSpeed_multi_2); //SetProfileType(1)
		m_profileF.AddXYData(m_dScrewStroke_3,m_dFillSpeed_multi_3);
		m_profileF.AddXYData(m_dVP,m_dFillSpeed_multi_3);
	}

	m_profileF.Invalidate(TRUE);
}

void CMDXProcessPage::SetProfilePack()
{
	CRect rect;
	GetDlgItem(IDC_STATIC_PACKING)->GetWindowRect(&rect);
	GetDlgItem(IDC_STATIC_PACKING)->ScreenToClient(&rect) ;
	
	m_profileP.CreateChart(rect,GetDlgItem(IDC_STATIC_PACKING) );
	m_profileP.SetProfileType(1);	//0:純線段 1:填滿
	m_profileP.SetXAxisDrawingMode(0);	//0:右畫到左 1:左畫到右
	m_profileP.SetXAxisCaption(_T("時間 (sec)"));
	m_profileP.SetYAxisCaption(_T("保壓壓力 (MPa)"));
	m_profileP.SetReadOnly(true);
	m_profileP.SetColorForBk(RGB( 255,255,255 ) );  //背景顏色
	m_profileP.SetVPSwitchLocation(m_dVP); //VP位置
	m_profileP.SetProfileColor( RGB(0,0,255));  //多段線的顏色
	
	m_profileP.RemoveAllData();

	//從資料中心拿資料
	double gate_thickness = DataCenter::getInstance().GetGateThickness();
	m_dPackTime = gate_thickness * (1+2*gate_thickness);

	m_profileP.SetXMinMaxValue(0,m_dPackTime);
	m_profileP.SetYMinMaxValue(0,m_dInjectionPressure);

	//加入資料
	if (m_iPackSel == 0) //單段保壓 0.75(機器壓力)
	{
		m_profileP.AddXYData(m_dPackTime,0.75*m_dInjectionPressure);
		m_profileP.AddXYData(0,0.75*m_dInjectionPressure);
	}
	else //二段保壓 0.75:0.4(機器壓力)
	{
		m_profileP.AddXYData(m_dPackTime,0.4*m_dInjectionPressure);
		m_profileP.AddXYData(0.5*m_dPackTime,0.4*m_dInjectionPressure);
		m_profileP.AddXYData(0.5*m_dPackTime,0.75*m_dInjectionPressure);
		m_profileP.AddXYData(0,0.75*m_dInjectionPressure);
	}

	m_profileP.Invalidate(TRUE);
}

// test button
void CMDXProcessPage::OnBnClickedButton1()
{
	m_profileP.RemoveAllData();
	m_profileP.AddXYData(150,40);
	m_profileP.AddXYData(30,20);
	m_profileP.AddXYData(15,90);
	m_profileP.AddXYData(0,0);
	m_profileP.SetXMinMaxValue(0,250);
	m_profileP.SetYMinMaxValue(0,123);
	m_profileP.Invalidate(TRUE);
}

void CMDXProcessPage::OnCbnSelchangeComboFillingSection()
{
	if (m_iFillSel == 0)
	{
		m_iFillSel = 1;
	}
	else
	{
		m_iFillSel = 0;
	}

	m_profileF.RemoveAllData();
	SetProfileFill();
}

void CMDXProcessPage::OnCbnSelchangeComboPackingSection()
{
	if (m_iPackSel == 0)
	{
		m_iPackSel = 1;
	}
	else
	{
		m_iPackSel = 0;
	}

	m_profileP.RemoveAllData();
	SetProfilePack();
}
