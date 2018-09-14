#pragma once

#include "BridgePacket.h"

#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#define BUFSIZE 512


class NamedPipe
{

private:
	HANDLE m_hPipe;
	TCHAR  m_chBuf[BUFSIZE];
	BOOL   m_fSuccess = FALSE;
	DWORD  m_cbRead, m_dwMode;

	LPTSTR m_lpszPipename = TEXT("\\\\.\\pipe\\projectfly");


public:
	NamedPipe();
	~NamedPipe();

	int open_pipe();
	void close_pipe();

	int write_packet(BridgePacket &packet);


};

