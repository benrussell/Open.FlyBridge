#include "NamedPipe.h"

#include <XPLMUtilities.h>



NamedPipe::NamedPipe()
{
}


NamedPipe::~NamedPipe()
{
	close_pipe();
}

bool NamedPipe::open_pipe()
{

	//char caDbg[1024];


	// This function is called blind. It must check to see if the pipe handle is already valid.
	if (m_hPipe != INVALID_HANDLE_VALUE) {
		return true; //we already have a valid pipe, do nothing and exit.
	}


	// Try to open a named pipe; wait for it, if necessary. 
	m_hPipe = CreateFile(
		TEXT("\\\\.\\pipe\\projectfly"),   // pipe name 
		GENERIC_READ |  // read and write access 
		GENERIC_WRITE,
		0,              // no sharing 
		NULL,           // default security attributes
		OPEN_EXISTING,  // opens existing pipe 
		0,              // default attributes 
		NULL);          // no template file 

						// Break if the pipe handle is valid. 


	if(INVALID_HANDLE_VALUE == m_hPipe) {

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 

		if(GetLastError() != ERROR_PIPE_BUSY){
			
			// This error will fire repeatedly if the Fly Client App is not running.

			//snprintf(caDbg, 1024, "ofb: Could not open pipe. GLE=%d\n", GetLastError());
			//XPLMDebugString(caDbg);

			return false;
		
		} else {
			// All pipe instances are busy, so wait for 20 seconds. 
			// We can't use WaitNamedPipe because it will block X-P.
			// FIXME: pipes were tagged as busy, put an expiry timer in..

			XPLMDebugString("ofb: Pipe busy. FIXME: 20s delay.\n");
			return false;

		}

		
	} //end: if( valid_pipe_handle )
	

	XPLMDebugString("ofb: Pipe opened.\n");

	return true;
}



void NamedPipe::close_pipe()
{

	CloseHandle(m_hPipe);
	XPLMDebugString("ofb: Pipe closed.\n");

}










int NamedPipe::write_packet(BridgePacket &packet){

	//char caDbg[1024];
	//XPLMDebugString("ofb: NamedPipe::write_packet()\n");
		
	// Call for open_pipe() - handles all "open once" logic itself.
	if (open_pipe()) {

		DWORD cbWriten; //how much data was actually written.

		int tmp_pkt_size = sizeof(BridgePacket);
		// Send a message to the pipe server. 

		m_fSuccess = WriteFile(
			m_hPipe,                  // pipe handle 
			&packet,					//message blob
			(DWORD)tmp_pkt_size,     // message length 
			&cbWriten,             // bytes written 
			NULL);                  // not overlapped 


									//snprintf(caDbg, 1024, "ofb: wrote: %d bytes\n", cbWriten);
									//XPLMDebugString(caDbg);

		if (!m_fSuccess){
			m_hPipe = INVALID_HANDLE_VALUE;
			//snprintf(caDbg, 1024, "ofb: WriteFile to pipe failed. GLE=%d\n", GetLastError());
			//XPLMDebugString(caDbg);

			return -1;
		}


		return 1;

	} //code will only run if the IPC pipe is or could be opened.


} //end: write_packet(..)


