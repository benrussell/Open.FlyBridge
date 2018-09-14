#pragma once

#include <XPLMDefs.h>

#include <XPLMUtilities.h>
#include <XPLMDataAccess.h>





#include "NamedPipe.h"


class OpenFlyBridge {
private:
	// IPC Pipe
	static NamedPipe m_IPCPipe;


	// X-Plane Datarefs
	static XPLMDataRef m_drAltMsl;
	static XPLMDataRef m_drAltAgl;

	static XPLMDataRef m_drGroundSpeed;
	static XPLMDataRef m_drIndicatedAirSpeed;

	static XPLMDataRef m_drHdgTrue;
	static XPLMDataRef m_drHdgMagnetic;

	static XPLMDataRef m_drLat;
	static XPLMDataRef m_drLon;

	static XPLMDataRef m_drVerticalSpeed;
	
	static XPLMDataRef m_drGForce;

	static XPLMDataRef m_drFuelTotal;

	static XPLMDataRef m_drTransponderCode;

	static XPLMDataRef m_drOnGround;
	static XPLMDataRef m_drIsSlew;
	static XPLMDataRef m_drIsPaused;
	
	static XPLMDataRef m_drPitch;
	static XPLMDataRef m_drRoll;

	static XPLMDataRef m_drFRP;



	static void build_packet( BridgePacket &packet );



public:

	static void init_drefs();

	static void open_ipc_pipe();
	static void close_ipc_pipe();


	static float cb_FlightLoop_1hz(
		float                inElapsedSinceLastCall,
		float                inElapsedTimeSinceLastFlightLoop,
		int                  inCounter,
		void *               inRefcon
	);


};


