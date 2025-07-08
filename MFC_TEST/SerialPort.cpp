#include "pch.h"
#include "SerialPort.h"

CSerialPort::CSerialPort()
{
    m_hComm = INVALID_HANDLE_VALUE;
}

CSerialPort::~CSerialPort()
{
    Close();
}

BOOL CSerialPort::Open(int nPort, int nBaud)
{
    if (IsOpened())
        return FALSE;

    CString strPort;
    strPort.Format(_T("\\\\.\\COM%d"), nPort);

    m_hComm = CreateFile(strPort,
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         NULL,
                         OPEN_EXISTING,
                         0,
                         NULL);

    if (m_hComm == INVALID_HANDLE_VALUE)
        return FALSE;

    DCB dcb;
    GetCommState(m_hComm, &dcb);
    dcb.BaudRate = nBaud;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    SetCommState(m_hComm, &dcb);

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = MAXDWORD;
    // timeouts.ReadTotalTimeoutConstant = 0;
    // timeouts.ReadTotalTimeoutMultiplier = 0;
    // timeouts.WriteTotalTimeoutConstant = 50;
    // timeouts.WriteTotalTimeoutMultiplier = 10;
    SetCommTimeouts(m_hComm, &timeouts);

    return TRUE;
}

void CSerialPort::Close()
{
    if (IsOpened())
    {
        CloseHandle(m_hComm);
        m_hComm = INVALID_HANDLE_VALUE;
    }
}

int CSerialPort::Read(char* pBuffer, int nLength)
{
    if (!IsOpened())
        return 0;

    DWORD dwBytesRead = 0;
    ReadFile(m_hComm, pBuffer, nLength, &dwBytesRead, NULL);
    return dwBytesRead;
}

int CSerialPort::Write(char* pBuffer, int nLength)
{
    if (!IsOpened())
        return 0;

    DWORD dwBytesWritten = 0;
    WriteFile(m_hComm, pBuffer, nLength, &dwBytesWritten, NULL);
    return dwBytesWritten;
}
