// OpenFlyBridge.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"

#include "OpenFlyBridge.h"

#include "BridgePacket.h"

#include <XPLMProcessing.h>


// Last WoW flag value. Used for edge detection in HighSpeed FLCB.
int OpenFlyBridge::m_iLastWoWFlag = 0;

float OpenFlyBridge::m_fLastWowFlag_TimeStamp=0.f;
const float OpenFlyBridge::m_fLastWowFlag_TimeDelay=5.f;

int OpenFlyBridge::m_iVerticalLandingSpeed = 0;



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
//XPLMDataRef OpenFlyBridge::m_drIsSlew;
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

	//m_drIsSlew = XPLMFindDataRef(""); //slew <------------------------------------------ FIXME: Camera mode?

	m_drIsPaused = XPLMFindDataRef("sim/time/paused");

	m_drPitch = XPLMFindDataRef("sim/flightmodel/position/true_theta");

	m_drRoll = XPLMFindDataRef("sim/flightmodel/position/true_phi");

	m_drFRP = XPLMFindDataRef("sim/operation/misc/frame_rate_period");
		
}



float OpenFlyBridge::cb_FlightLoop_1hz(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void * inRefcon)
{

	//XPLMDebugString("ofb: cb_FlightLoop_1hz()\n");

#if 1
	if (XPLMGetDatai(m_drIsPaused)) {
		// If paused, send no packets?
		return 1.0f;
	}
#endif
		
	// Packet handler functions will pull in the packet var by-ref.
	BridgePacket packet;
	build_packet( packet );
	m_IPCPipe.write_packet( packet );
	

	return 1.0f;
}

float OpenFlyBridge::cb_FlightLoop_HighSpeedTriggers(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void * inRefcon)
{

	char caDbg[1024];

	// Watch the WoW flag and the landing speed. If the WoW flag changes we take action.

	int iWoWAny = XPLMGetDatai(m_drOnGround);
	if (m_iLastWoWFlag != iWoWAny) {

		//Calculate and enforce a minimum time window for WoW detection.
		//This is essentially a crude bump-landing filter.
		const float fNow = XPLMGetElapsedTime();
		const float fTsDelta = fNow - m_fLastWowFlag_TimeStamp;

		const float fTsRemains = m_fLastWowFlag_TimeDelay - fTsDelta;
		if (fTsDelta >= m_fLastWowFlag_TimeDelay) {

			if (iWoWAny) {
				// Rising edge. Weight is newly on wheels.
				XPLMDebugString("ofb: WoW rising-edge.\n");

				//Save the vertical speed when we hit the deck.
				OpenFlyBridge::m_iVerticalLandingSpeed = (int)XPLMGetDataf(m_drVerticalSpeed);


				//Set a timestamp to use for filtering out bump events.
				XPLMDebugString("ofb: Set LandingVS WindowTS.\n");
				m_fLastWowFlag_TimeStamp = fNow;

			} else {
				// Falling edge. Weight is now off wheels.
				XPLMDebugString("ofb: WoW falling-edge.\n");

				//Reset the vertical landing speed to 0.
				OpenFlyBridge::m_iVerticalLandingSpeed = 0;

			}


			// Save value for next frame.
			// This is time-filter protected.
			m_iLastWoWFlag = iWoWAny;

		} else {
			// Filter out bump events.
			snprintf(caDbg, 1024, "ofb: WoW BUMP!: remaining:%0.3f\n", fTsRemains);
			XPLMDebugString(caDbg);

		} //time filter.

	} // WoW flag change filter.


	// Slew mode logic?...


	return -1.0f; //every frame thanks.
}



void OpenFlyBridge::build_packet( BridgePacket &packet ) {

	//char caDbg[1024];
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
	packet.vs_landing = OpenFlyBridge::m_iVerticalLandingSpeed; // This is calculated in the HighSpeed FLCB

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

