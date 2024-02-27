
// RFIDDlg.cpp: 구현 파일
//
#include "is_d2xx.h"
#include "pch.h"
#include "framework.h"
#include "RFID.h"
#include "RFIDDlg.h"
#include "afxdialogex.h"

// sound 출력용
#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include "mmsystem.h"
#pragma comment(lib,"winmm.lib") // 라이브러리 불러오기

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Maria DB
#include <iostream>
#include <mysql.h>

//thread
CWinThread* p1;
CString m_staticDisp;
static UINT TimeThread(LPVOID _mothod);

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx) 

END_MESSAGE_MAP()


// CRFIDDlg 대화 상자



CRFIDDlg::CRFIDDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RFID_DIALOG, pParent)
	, m_strRfid(_T(""))
	, m_strRfid2(_T(""))

	, m_strRfid3(_T(""))
	, m_strRfid4(_T(""))
	, m_strRfid5(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	//maira db
	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);
	conn_result = mysql_real_connect(conn, "127.0.0.1", "root", "0000", "samplesys", 3306, NULL, 0);

}
CRFIDDlg::~CRFIDDlg()
{
	//maria db 
	mysql_free_result(result);
	mysql_close(conn);
	//
	OnDisconnect();
}

LRESULT CRFIDDlg::updateMessage(WPARAM wParam, LPARAM lParam)
{
	UpdateDataStruct* updateData = (UpdateDataStruct*)lParam;
	m_strRfid = updateData->strRfid;
	printf("STR TEST: %s", m_strRfid);
	UpdateData(FALSE);

	delete updateData;
	return 0;
}

void CRFIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strRfid);
	DDX_Text(pDX, IDC_EDIT2, m_strRfid2);
	DDX_Text(pDX, IDC_EDIT3, m_strRfid3);
	DDX_Text(pDX, IDC_EDIT4, m_strRfid4);
	DDX_Text(pDX, IDC_EDIT5, m_strRfid5);
}
static const UINT WM_UPDATE_MESSAGE = WM_USER + 1;

BEGIN_MESSAGE_MAP(CRFIDDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CRFIDDlg::OnConnect)
	ON_BN_CLICKED(IDC_BUTTON3, &CRFIDDlg::OnReadOnce)
	ON_BN_CLICKED(IDC_BUTTON4, &CRFIDDlg::OnReadContinue)
	ON_BN_CLICKED(IDC_BUTTON2, &CRFIDDlg::OnDisconnect)
	ON_MESSAGE(WM_UPDATE_MESSAGE, &CRFIDDlg::updateMessage)
	ON_EN_CHANGE(IDC_EDIT2, &CRFIDDlg::OnEnChangeEdit2)
	ON_EN_CHANGE(IDC_EDIT1, &CRFIDDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDOK, &CRFIDDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CRFIDDlg 메시지 처리기

BOOL CRFIDDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	p1 = NULL;

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CRFIDDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CRFIDDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CRFIDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRFIDDlg::OnConnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		//열린 포트번호 찾기
	if (is_GetDeviceNumber(&usbnumber) == IS_OK)
	{
		printf("FTDI USB To Serial 연결된 개수 : %d\n", usbnumber);
		if (is_GetSerialNumber(0, readSerialNumber) == IS_OK)
		{
			if (memcmp(changeSerialNumber, readSerialNumber, sizeof(changeSerialNumber) != 0))
			{
				if (is_SetSerialNumber(0, changeSerialNumber) == IS_OK)
				{
					printf(" USB To Serial Number 를 변경 하였습니다.\n");
					printf(" FTDI SerialNumber :  %s \n", changeSerialNumber);
				}
			}
			else
				printf(" FTDI SerialNumber :  %s \n", changeSerialNumber);
		}
	}

	//열린 포트번호로 연결
	unsigned long portNumber;
	if (is_GetCOMPort_NoConnect(0, &portNumber) == IS_OK)
	{
		printf("COM Port : %d\n", portNumber);
	}
	if (is_OpenSerialNumber(&ftHandle, readSerialNumber, 115200) != IS_OK)
	{
		printf("USB To Serial과 통신 연결 실패\n");
		//return -1;
	}
	else {
		printf("Serial포트 %d와 통신 연결성공!! \n", portNumber);
	}
	Sleep(100);
}


void CRFIDDlg::OnReadOnce()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	readonce();
	/////사진출력/////

	if (m_strRfid == card1)
	{
		floatingPhoto(card1);
		int UID = 1;
		mariadb(UID);
	}

	if (m_strRfid == card2)
	{
		floatingPhoto(card2);
		int UID = 2;
		mariadb(UID);
	}

	if (m_strRfid == card3)
	{
		floatingPhoto(card3);
		int UID = 3;
		mariadb(UID);
	}

	if (m_strRfid == card4)
	{
		floatingPhoto(card4);
		int UID = 4;
		mariadb(UID);
	}
	
	//Thread 종료
	if (NULL != p1) {
		::SuspendThread(p1->m_hThread);
	}
	
}

void CRFIDDlg::OnReadContinue()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	p1 = AfxBeginThread(TimeThread, this);

	if (p1 == nullptr) {
		AfxMessageBox(_T("Error!"));
	}

}


// 쓰레드실행시 반복 호출 될 코드
static UINT TimeThread(LPVOID _mothod)
{
	CRFIDDlg* fir = (CRFIDDlg*)_mothod;

	printf("thread in");
	while (1) 
	{
		if (!fir)
		{
			continue;
		}
		printf("thread");
		CString temp = _T("");
		CString temp1 = _T("");
		// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		//ISO15693모드로 읽기( 싱글모드 읽기 )
		if (fir->flag_r == 0)
		{
			if (is_WriteReadCommand(fir->ftHandle, CM1_ISO15693, CM2_ISO15693_ACTIVE + BUZZER_OFF,
				fir->writeLength, fir->writeData, &fir->readLength, fir->readData) == IS_OK)
			{
				printf("ISO 15693 UID : ");
				for (int i = 0; i < fir->readLength; i++)
				{
					temp.Format(_T("%02x "), fir->readData[i]);
					temp1 += temp;
					printf("%02x ", fir->readData[i]);
				}
				UpdateDataStruct* updateData = new UpdateDataStruct();
				updateData->strRfid = temp1;
				fir->PostMessageW(WM_UPDATE_MESSAGE, 0, (LPARAM)updateData);

				printf("\n");
				continue;
			}

			Sleep(100);
		}
	}
	return 0;
}

void CRFIDDlg::OnDisconnect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
		// 무선파워를 끊어요.
	is_RfOff(ftHandle);
	//USB 포트를 Close
	if (is_Close(ftHandle) == IS_OK)
	{
		printf("연결을 닫습니다. ");
	}

}

void CRFIDDlg::OnEnChangeEdit2()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CRFIDDlg::OnEnChangeEdit1()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.

	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

//mariadb
void CRFIDDlg::mariadb(int uid)
{
	conn = mysql_init(NULL);
	mysql_options(conn, MYSQL_OPT_CONNECT_TIMEOUT, &timeout_sec);

	conn_result = mysql_real_connect(conn, "127.0.0.1", "root", "0000", "samplesys", 3306, NULL, 0);

	if (conn_result==nullptr)
	{
		std::cout << "DB Connection Fail" << std::endl;
	}

	char query[100];
	MYSQL_ROW row;

	sprintf_s(query, 100, "SELECT * FROM missing_pet_prevention_ WHERE UID = %d",uid);

	// Get Response

	// Send Query
	if (mysql_query(conn, query))
	{
		std::cout << "SELECT Fail" << std::endl;
	}

	result = mysql_store_result(conn);

	int fields = mysql_num_fields(result);    // 필드 갯수 구함

	//객체에 스트링 출력
	CString temp, temp1 = _T("");
	CString stringbuff[10];

	while (row = mysql_fetch_row(result))     // 모든 레코드 탐색
	{
		for (int i = 0; i < fields; i++)    // 각각의 레코드에서 모든 필드 값 출력
		{
			stringbuff[i] = row[i];
			temp += row[i];

			printf("%02s", row[i]);
		}
		std::cout << std::endl;

	}

	//stringbuff = temp;
	//m_strRfid2 = stringbuff;
	m_strRfid3 = stringbuff[1];
	m_strRfid4 = stringbuff[2];
	m_strRfid5 = stringbuff[3];

	UpdateData(FALSE);

	return;
}

void CRFIDDlg::readonce()
{
		//ISO15693모드로 읽기
		if (is_WriteReadCommand(ftHandle, CM1_ISO15693, CM2_ISO15693_ACTIVE + BUZZER_OFF,
			writeLength, writeData, &readLength, readData) == IS_OK)
		{
			int i;

			printf("ISO 15693 UID : ");
			for (i = 0; i < readLength; i++)
			{
				temp.Format(_T("%02x "), readData[i]);

				temp1 += temp;
				printf("%02x ", readData[i]);

			}

		}


		//ISO14443A모드로 읽기
		if (is_WriteReadCommand(ftHandle, CM1_ISO14443AB, CM2_ISO14443A_ACTIVE + BUZZER_OFF,
			writeLength, writeData, &readLength, readData) == IS_OK)
		{
			int i;
			printf("ISO 14443AB UID : ");
			for (i = 0; i < readLength; i++)
			{
				temp.Format(_T("%02x "), readData[i]);
				temp1 += temp;
				printf("%02x ", readData[i]);

			}
		}
		m_strRfid.Empty();
		m_strRfid = temp1;
		temp1.Empty();
		UpdateData(FALSE);
		printf("\n");
		
		//시간 출력
		SYSTEMTIME st;
		GetLocalTime(&st);

		int year = st.wYear;
		int month = st.wMonth;
		int day = st.wDay;
		int hour = st.wHour;
		int minute = st.wMinute;
		int second = st.wSecond;

		CString currentTime;
		currentTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), year, month, day, hour, minute, second);
		m_strRfid2 = currentTime;




}

void CRFIDDlg::floatingPhoto(CString card)
{
		CBitmap m_bitmap;

		CString Path_img1 = _T("img\\mark.bmp");
		CString Path_img2 = _T("img\\tony.bmp");
		CString Path_img3 = _T("img\\steve.bmp");
		CString Path_img4 = _T("img\\bill.bmp");
		
		m_bitmap.DeleteObject();
		if (card==card1)
		{
			m_bitmap.Attach((HBITMAP)LoadImage(NULL, Path_img1, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
		}
		if (card == card2)
		{
			m_bitmap.Attach((HBITMAP)LoadImage(NULL, Path_img2, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
		}
		if (card == card3)
		{
			m_bitmap.Attach((HBITMAP)LoadImage(NULL, Path_img3, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
		}
		if (card == card4)
		{
			m_bitmap.Attach((HBITMAP)LoadImage(NULL, Path_img4, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE));
		}

		CDC* pDC = GetDC();
		CDC memDC;
		memDC.CreateCompatibleDC(pDC);//////////////////////////
		CRect m_rect;
		memDC.SelectObject(m_bitmap);
		((CStatic*)GetDlgItem(IDC_STATIC))->GetWindowRect(m_rect);
		ScreenToClient(&m_rect);
		CClientDC   dc(this);

		dc.BitBlt(30, 10, 260, 240, &memDC, 0, 0, SRCCOPY);

		ReleaseDC(pDC); // 출력 명령
		DeleteDC(memDC);
}


void CRFIDDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}
