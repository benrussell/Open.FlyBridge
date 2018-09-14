#include "NamedPipe.h"

#include <XPLMUtilities.h>



NamedPipe::NamedPipe()
{
}


NamedPipe::~NamedPipe()
{
}

int NamedPipe::open_pipe()
{

	// Try to open a named pipe; wait for it, if necessary. 
	while (true) //FIXME
	{
		m_hPipe = CreateFile(
			m_lpszPipename,   // pipe name 
			GENERIC_READ |  // read and write access 
			GENERIC_WRITE,
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 

							// Break if the pipe handle is valid. 

		if (m_hPipe != INVALID_HANDLE_VALUE)
			break;

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if (GetLastError() != ERROR_PIPE_BUSY)
		{
			_tprintf(TEXT("Could not open pipe. GLE=%d\n"), GetLastError());
			return -1;
		}

		// All pipe instances are busy, so wait for 20 seconds. 

		if (!WaitNamedPipe(m_lpszPipename, 20000))
		{
			printf("Could not open pipe: 20 second wait timed out.");
			return -1;
		}
	}


	//if we make it this far the pipe has been opened.

	// The pipe connected; change to message-read mode. 

	m_dwMode = PIPE_READMODE_MESSAGE;
	m_fSuccess = SetNamedPipeHandleState(
		m_hPipe,    // pipe handle 
		&m_dwMode,  // new pipe mode 
		NULL,     // don't set maximum bytes 
		NULL);    // don't set maximum time 
	if (!m_fSuccess)
	{
		_tprintf(TEXT("SetNamedPipeHandleState failed. GLE=%d\n"), GetLastError());
		_tprintf(TEXT("* If you're using the real Fly Client this error message is expected.\n"));
		//return -1;
	} else {
		_tprintf(TEXT("Strange Pipe Server config detected. Allows read.\n"));
	}
	_tprintf(TEXT("\n"));



	return true;
}



void NamedPipe::close_pipe()
{

	CloseHandle(m_hPipe);

}










int NamedPipe::write_packet(BridgePacket &packet)
{

	XPLMDebugString("ofb: NamedPipe::write_packet()\n");

	DWORD cbWriten;

	int tmp_pkt_size = sizeof(BridgePacket);
	// Send a message to the pipe server. 
	
		m_fSuccess = WriteFile(
			m_hPipe,                  // pipe handle 
			&packet,					//message blob
			(DWORD)tmp_pkt_size,     // message length 
			&cbWriten,             // bytes written 
			NULL);                  // not overlapped 

		if (!m_fSuccess)
		{
			_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError());
			return -1;
		}

		return 1;

}


