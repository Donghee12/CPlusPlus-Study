
// AR_TESTDlg.h: 헤더 파일
//

#pragma once
#include "SerialPort.h"
#include <mysql/jdbc.h> // MySQL Connector/C++

// CARTESTDlg 대화 상자
class CARTESTDlg : public CDialogEx
{
// 생성입니다.
public:
	CARTESTDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	~CARTESTDlg(); // 소멸자 추가

	CSerialPort m_serialPort; // 시리얼 포트 객체

	// MySQL
	sql::Driver* m_pMySqlDriver;
	sql::Connection* m_pMySqlConnection;
	sql::PreparedStatement* m_pMySqlPstmt;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AR_TEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	CComboBox* m_pComboPort;
	CComboBox* m_pComboBaud;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedButtonDisconnect();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
};
