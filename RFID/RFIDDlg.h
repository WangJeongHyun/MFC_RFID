// RFIDDlg.h: 헤더 파일
#pragma once
#include "is_d2xx.h"

// Maria DB
#include <iostream>
#include <mysql.h>

struct UpdateDataStruct
{
	CString strRfid;
};
// CRFIDDlg 대화 상자
class CRFIDDlg : public CDialogEx
{
// 생성입니다.
public:
	CRFIDDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	~CRFIDDlg();
	LRESULT CRFIDDlg::updateMessage(WPARAM wParam, LPARAM lParam);
	IS_HANDLE ftHandle = 0;
	char readSerialNumber[100] = "COM07";
	char changeSerialNumber[100] = "RFID01";
	short usbnumber;
	unsigned char writeData[1024];
	unsigned short writeLength = 0;
	unsigned char readData[1024];
	unsigned short readLength = 0;
	BOOL flag_r=0;
	int UID = 0;
	CString temp, temp1 = _T("");
	//maria db
	MYSQL_RES* result;
	MYSQL* conn;
	MYSQL* conn_result;
	unsigned int timeout_sec = 1;



// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RFID_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	CString card1 = _T("03 27 68 cb 50 01 04 e0 ");
	CString card2 = _T("d5 0d 68 cb 50 01 04 e0 ");
	CString card3 = _T("51 ab 1b c9 ");
	CString card4 = _T("21 37 cb 5b ");

// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnConnect();
	afx_msg void OnDisconnect();
	afx_msg void OnReadOnce();
	afx_msg void OnReadContinue();
	CString m_strRfid;
	
//	afx_msg void OnBnClickedButton5();
//	afx_msg void OnBnClickedButton6();
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnEnChangeEdit1();
	CString m_strRfid2;

	//maria db
	void mariadb(int uid); // 헤더에 선언 후 함수생성
	CString m_strRfid3;
	CString m_strRfid4;
	CString m_strRfid5;
	//
	void readonce();
	//
	void floatingPhoto(CString);
	//

	afx_msg void OnBnClickedOk();
};
