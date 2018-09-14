// OpenFlyBridge.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "OpenFlyBridge.h"

#include "BridgePacket.h"



NamedPipe OpenFlyBridge::m_IPCPipe;


XPLMDataRef OpenFlyBridge::m_drAltMsl;
XPLMDataRef OpenFlyBridge::m_drAltAgl;

XPLMDataRef OpenFlyBridge::m_drGroundSpeed;
XPLMDataRef OpenFlyBridge::m_drIndicatedAirSpeed;

XPLMDataRef OpenFlyBridge::m_drHdgTrue;
XPLMDataRef OpenFlyBridge::m_drHdgMagnetic;

XPLMDataRef OpenFlyBridge::m_drLat;
XPLMDataRef OpenFlyBridge::m_drLon;

XPLMDataRef OpenFlyBridge::m_drVerticalSpeed;

XPLMDataRef OpenFlyBridge::m_drGForce;

XPLMDataRef OpenFlyBridge::m_drFuelTotal;

XPLMDataRef OpenFlyBridge::m_drTransponderCode;

XPLMDataRef OpenFlyBridge::m_drOnGround;
XPLMDataRef OpenFlyBridge::m_drIsSlew;
XPLMDataRef OpenFlyBridge::m_drIsPaused;

XPLMDataRef OpenFlyBridge::m_drPitch;
XPLMDataRef OpenFlyBridge::m_drRoll;

XPLMDataRef OpenFlyBridge::m_drFRP;




void OpenFlyBridge::init_drefs(){

	XPLMDebugString("ofb: init_drefs()\n");

	/*
	These datarefs were extracted from the win.xpl that METTAR ships.

	sim/flightmodel/position/elevation
	sim/flightmodel/position/latitude
	sim/flightmodel/position/longitude
	sim/flightmodel/position/theta <------ wrong.
	sim/flightmodel/position/phi <------ wrong
	sim/flightmodel/position/psi
	sim/flightmodel/position/true_psi
	sim/flightmodel/position/mag_psi
	sim/flightmodel/position/groundspeed
	sim/flightmodel/position/indicated_airspeed
	sim/flightmodel/position/y_agl
	sim/cockpit/radios/transponder_code
	sim/flightmodel/failures/onground_any
	sim/time/paused
	sim/flightmodel/position/vh_ind_fpm
	sim/flightmodel/weight/m_fuel_total
	sim/operation/misc/frame_rate_period
	
	*/


	m_drAltMsl = XPLMFindDataRef("sim/flightmodel/position/elevation");
	m_drAltAgl = XPLMFindDataRef("sim/flightmodel/position/y_agl");
	
	m_drGroundSpeed = XPLMFindDataRef("sim/flightmodel/position/groundspeed");
	m_drIndicatedAirSpeed = XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed");

	m_drHdgTrue = XPLMFindDataRef("sim/flightmodel/position/true_psi");
	m_drHdgMagnetic = XPLMFindDataRef("sim/flightmodel/position/mag_psi");

	m_drLat = XPLMFindDataRef("sim/flightmodel/position/latitude");
	m_drLon = XPLMFindDataRef("sim/flightmodel/position/longitude");
	
	m_drVerticalSpeed = XPLMFindDataRef("sim/flightmodel/position/vh_ind_fpm");
	
	m_drGForce = XPLMFindDataRef("sim/flightmodel/forces/g_nrml");

	m_drFuelTotal = XPLMFindDataRef("sim/flightmodel/weight/m_fuel_total");

	m_drTransponderCode = XPLMFindDataRef("sim/cockpit/radios/transponder_code");

	m_drOnGround = XPLMFindDataRef("sim/flightmodel/failures/onground_any");

	m_drIsSlew = XPLMFindDataRef(""); //slew <------------------------------------------ FIXME: Camera mode?

	m_drIsPaused = XPLMFindDataRef("sim/time/paused");

	m_drPitch = XPLMFindDataRef("sim/flightmodel/position/true_theta");

	m_drRoll = XPLMFindDataRef("sim/flightmodel/position/true_phi");

	m_drFRP = XPLMFindDataRef("sim/operation/misc/frame_rate_period");
		
}


void OpenFlyBridge::open_ipc_pipe()
{

	XPLMDebugString("ofb: Opening IPC Pipe..\n");
	m_IPCPipe.open_pipe(); //FIXME: error detection.

}

void OpenFlyBridge::close_ipc_pipe()
{
	XPLMDebugString("ofb: Closing IPC Pipe..\n");
	m_IPCPipe.close_pipe();
}

float OpenFlyBridge::cb_FlightLoop_1hz(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void * inRefcon)
{

	//XPLMDebugString("ofb: cb_FlightLoop_1hz()\n");
		
	BridgePacket packet;
	build_packet( packet );

	m_IPCPipe.write_packet( packet );
	

	return 1.0f;
}



void OpenFlyBridge::build_packet( BridgePacket &packet ) {

	char caDbg[1024];
	//XPLMDebugString("ofb: build_packet()\n");
	
	packet.alt_msl = (int)XPLMGetDataf(m_drAltMsl);
	packet.alt_agl = (int)XPLMGetDataf(m_drAltAgl);

	packet.gs = (int)XPLMGetDataf(m_drGroundSpeed);
	packet.ias = (int)XPLMGetDataf(m_drIndicatedAirSpeed);

	packet.htrue = (int)XPLMGetDataf(m_drHdgTrue);
	packet.hmag = (int)XPLMGetDataf(m_drHdgMagnetic);

	packet.lat = XPLMGetDatad(m_drLat);
	packet.lon = XPLMGetDatad(m_drLon);

	packet.vs = (int)XPLMGetDataf(m_drVerticalSpeed);
	packet.vs_landing = 11; //FIXME: Calculate when we get new WoW rising edge flag?

	packet.g_force = (int)(XPLMGetDataf(m_drGForce) * 1000.f); //scale by 1000 to pack into an int value.....

	packet.fuel_loaded = (int)XPLMGetDataf(m_drFuelTotal);

	packet.trans_code = XPLMGetDatai(m_drTransponderCode);

	packet.bridge_type = 3; //x-plane, leave hard coded.

	packet.on_ground = XPLMGetDatai(m_drOnGround);
	packet.is_slew = 0; //FIXME: ------------------------- How? ---------------
	packet.is_paused = XPLMGetDatai(m_drIsPaused);

	packet.pitch = (int)XPLMGetDataf(m_drPitch);
	packet.roll = (int)XPLMGetDataf(m_drRoll);

	packet.fps = (int)(1 / XPLMGetDataf(m_drFRP));
	if (packet.fps < 0) {
		// force to 0 when paused.
		packet.fps = 0;
	}
	
} //end build_packet(..)

