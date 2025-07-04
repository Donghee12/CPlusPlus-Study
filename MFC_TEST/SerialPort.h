#pragma once

#include <Windows.h>

class CSerialPort
{
public:
    CSerialPort();
    ~CSerialPort();

    BOOL Open(int nPort, int nBaud);
    void Close();
    int Read(char* pBuffer, int nLength);
    int Write(char* pBuffer, int nLength);
    BOOL IsOpened() { return m_hComm != INVALID_HANDLE_VALUE; }

private:
    HANDLE m_hComm;
};
