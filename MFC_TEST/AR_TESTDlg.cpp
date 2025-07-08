// AR_TESTDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "AR_TEST.h"
#include "AR_TESTDlg.h"
#include "afxdialogex.h"
#include "SerialPort.h" // CSerialPort 클래스 포함
#include <afxwin.h> // CComboBox, CEdit 등 MFC 컨트롤 클래스 포함
#include <atlbase.h> // CT2A 등 ATL/MFC 문자열 변환 매크로 포함
#include <atlconv.h> // CT2A 등 ATL/MFC 문자열 변환 매크로 포함

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CARTESTDlg 대화 상자



CARTESTDlg::CARTESTDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AR_TEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pComboPort = nullptr; // 멤버 변수 초기화
	m_pComboBaud = nullptr; // 멤버 변수 초기화

	// MySQL 멤버 변수 초기화
	m_pMySqlDriver = nullptr;
	m_pMySqlConnection = nullptr;
	m_pMySqlPstmt = nullptr;
}

CARTESTDlg::~CARTESTDlg()
{
	// 데이터베이스 연결 해제
	delete m_pMySqlPstmt;
	delete m_pMySqlConnection;
}

void CARTESTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CARTESTDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CARTESTDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CARTESTDlg::OnBnClickedButtonDisconnect)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CARTESTDlg::OnBnClickedButtonSend)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CARTESTDlg 메시지 처리기

BOOL CARTESTDlg::OnInitDialog()
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

	// COM 포트 콤보 박스 초기화
	m_pComboPort = (CComboBox*)GetDlgItem(IDC_COMBO_PORT);
	for (int i = 1; i <= 10; i++) // COM1부터 COM20까지
	{
		CString strPort;
		strPort.Format(_T("COM%d"), i);
		m_pComboPort->AddString(strPort);
	}
	m_pComboPort->SetCurSel(0); // 기본값 COM1

	// 전송 속도 콤보 박스 초기화
	m_pComboBaud = (CComboBox*)GetDlgItem(IDC_COMBO_BAUD);
	m_pComboBaud->AddString(_T("9600"));
	m_pComboBaud->AddString(_T("19200"));
	m_pComboBaud->AddString(_T("38400"));
	m_pComboBaud->AddString(_T("57600"));
	m_pComboBaud->AddString(_T("115200"));
	m_pComboBaud->SetCurSel(4); // 기본값 115200

	SetTimer(1, 100, NULL);

	try {
		// MySQL 드라이버 가져오기
		m_pMySqlDriver = get_driver_instance();
		// MySQL 서버에 연결
		m_pMySqlConnection = m_pMySqlDriver->connect("tcp://127.0.0.1:3306", "root", "1234");
		m_pMySqlConnection->setClientOption("characterSetResults", "utf8mb4");
		m_pMySqlConnection->setClientOption("characterSetClient", "utf8mb4");
		m_pMySqlConnection->setClientOption("characterSetConnection", "utf8mb4");
		// 사용할 데이터베이스 선택
		m_pMySqlConnection->setSchema("ar_test_db");
		// Auto Commit 활성화
		m_pMySqlConnection->setAutoCommit(true);

		// 테이블이 없으면 생성
		std::unique_ptr<sql::Statement> stmt(m_pMySqlConnection->createStatement());
		stmt->execute("CREATE TABLE IF NOT EXISTS serial_logs (id INT AUTO_INCREMENT PRIMARY KEY, timestamp DATETIME, message VARCHAR(1024))");

		// INSERT를 위한 PreparedStatement 준비
		m_pMySqlPstmt = m_pMySqlConnection->prepareStatement("INSERT INTO serial_logs(timestamp, message) VALUES (?, ?)");

		AfxMessageBox(_T("DB 초기화 성공!")); // <--- 디버깅용 메시지 추가

	} catch (sql::SQLException &e) {
		CString strError;
		strError.Format(_T("MySQL Error: %s"), CString(e.what()));
		MessageBox(strError, _T("DB Connection Error"), MB_OK | MB_ICONERROR);
		return FALSE; // DB 연결 실패 시 다이얼로그 종료
	}

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CARTESTDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CARTESTDlg::OnPaint()
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
HCURSOR CARTESTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CARTESTDlg::OnBnClickedButtonConnect()
{
	int nPortIndex = m_pComboPort->GetCurSel();
	int nBaudIndex = m_pComboBaud->GetCurSel();

	CString strPort;
	m_pComboPort->GetLBText(nPortIndex, strPort);
	int nPort = _ttoi(strPort.Mid(3)); // "COM" 다음 숫자만 추출

	CString strBaud;
	m_pComboBaud->GetLBText(nBaudIndex, strBaud);
	int nBaud = _ttoi(strBaud);

	if (m_serialPort.Open(nPort, nBaud))
	{
		MessageBox(_T("COM 포트가 성공적으로 연결되었습니다."), _T("성공"), MB_OK | MB_ICONINFORMATION);
		// 연결 성공 시 UI 업데이트 (예: 연결 버튼 비활성화, 해제 버튼 활성화)
		GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(TRUE);
	}
	else
	{
		MessageBox(_T("COM 포트 연결에 실패했습니다."), _T("오류"), MB_OK | MB_ICONERROR);
	}
}

void CARTESTDlg::OnBnClickedButtonDisconnect()
{
	m_serialPort.Close();
	MessageBox(_T("COM 포트 연결이 해제되었습니다."), _T("성공"), MB_OK | MB_ICONINFORMATION);
	// 연결 해제 시 UI 업데이트
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(FALSE);
}

void CARTESTDlg::OnBnClickedButtonSend()
{
	CEdit* pEditSend = (CEdit*)GetDlgItem(IDC_EDIT_SEND);

	if (pEditSend == nullptr) // 널 포인터 체크 추가
	{
		MessageBox(_T("전송 에디트 컨트롤을 찾을 수 없습니다. 리소스 ID를 확인해주세요."), _T("오류"), MB_OK | MB_ICONERROR);
		return;
	}

	CString strSend;
	pEditSend->GetWindowText(strSend);

	// 아두이노가 메시지 끝을 인식하도록 개행 문자 추가
	strSend += _T("\n"); 

	if (m_serialPort.IsOpened())
	{
		// CString을 char*로 변환
		CT2A utf8(strSend, CP_UTF8);
		int nBytesWritten = m_serialPort.Write(utf8.m_psz, strlen(utf8.m_psz));
		
		CString strMessage;
		strMessage.Format(_T("%d 바이트를 전송했습니다."), nBytesWritten);
		MessageBox(strMessage, _T("전송"), MB_OK | MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(_T("COM 포트가 연결되지 않았습니다."), _T("오류"), MB_OK | MB_ICONERROR);
	}
}

void CARTESTDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1 && m_serialPort.IsOpened())
	{
		char data[1024];
		int len = m_serialPort.Read(data, sizeof(data) - 1);
		if (len > 0)
		{
			data[len] = '\0';

			// Convert received char* to CString
			USES_CONVERSION; // ATL 문자열 변환 매크로 활성화
			CString strReceived(CA2W(data, CP_UTF8)); // ← UTF-8 → 유니코드 변환

			// Get current time
			CTime time = CTime::GetCurrentTime();
			CString strTime = time.Format(_T("[%Y-%m-%d %H:%M:%S] "));

			// Format the log message
			CString strLog;
			strLog.Format(_T("%s%s\r\n"), strTime, strReceived);

			// Append to the edit control
			CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_RECEIVE);
			if (pEdit)
			{
				int nLength = pEdit->GetWindowTextLength();
				pEdit->SetSel(nLength, nLength);
				pEdit->ReplaceSel(strLog);
			}

			// Save to MySQL Database
			if (m_pMySqlPstmt) {
				try {
					// CString을 std::string으로 변환하기 위해 CT2A 사용
					CString strTimestamp = time.Format(_T("%Y-%m-%d %H:%M:%S"));
					CT2A ts_ascii(strTimestamp);
					CT2A msg_ascii(strReceived, CP_UTF8);  // ✅ UTF-8로 변환 지정


					// PreparedStatement에 변환된 값 설정
					m_pMySqlPstmt->setString(1, ts_ascii.m_psz);
					m_pMySqlPstmt->setString(2, msg_ascii.m_psz);

					// 쿼리 실행
					m_pMySqlPstmt->execute();
				}
				catch (sql::SQLException& e) {
					// DB 저장 실패 시 에러를 출력합니다.
					CString strError;
					strError.Format(_T("MySQL Save Error: %s"), CString(e.what()));
					AfxMessageBox(strError);
				}
			}
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}
