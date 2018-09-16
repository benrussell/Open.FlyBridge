#pragma once

#include "BridgePacket.h"

#include <windows.h> 
#include <stdio.h>
#include <conio.h>
#include <tchar.h>


class NamedPipe
{

private:
	HANDLE m_hPipe = INVALID_HANDLE_VALUE;
	BOOL   m_fSuccess = FALSE;
	
	bool open_pipe();
	void close_pipe();

public:
	NamedPipe();
	~NamedPipe();
	
	int write_packet(BridgePacket &packet);


};

