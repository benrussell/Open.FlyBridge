#include "XPHooks.h"

#include "OpenFlyBridge.h"

#include <XPLMUtilities.h>


//Standard X-Plane plugin function hooks.
PLUGIN_API int XPluginStart(
	char *		outName,
	char *		outSig,
	char *		outDesc) {

	strcpy_s(outName, 128, "Open.FlyBridge");
	strcpy_s(outSig, 128, "open.flybridge");
	strcpy_s(outDesc, 128, "Open.FlyBridge v18.09.17.1347 beta, (C) br@x-plugins.com");

	// Confirm we're running by echoing the outDesc string to the log file.
	char caDbg[1024];
	snprintf(caDbg, 1024, "%s\n", outDesc);
	XPLMDebugString(caDbg);


	OpenFlyBridge::init_drefs();


	return 1;
}




PLUGIN_API void XPluginStop(void) {
	
	XPLMDebugString("ofb: Stopped.");

}




PLUGIN_API int XPluginEnable(void) {

	// Init resources.
	
	XPLMRegisterFlightLoopCallback(
		OpenFlyBridge::cb_FlightLoop_1hz,	// Callback
		1.0f,					// Interval
		NULL);

	XPLMRegisterFlightLoopCallback(
		OpenFlyBridge::cb_FlightLoop_HighSpeedTriggers,	// Callback
		-1.0f,					// Interval
		NULL);


	XPLMDebugString("ofb: Enabled.\n");

	return 1;
}

PLUGIN_API void XPluginDisable(void) {

	// Cleanup resources.
	XPLMUnregisterFlightLoopCallback(OpenFlyBridge::cb_FlightLoop_1hz, NULL);
	
	XPLMUnregisterFlightLoopCallback(OpenFlyBridge::cb_FlightLoop_HighSpeedTriggers, NULL);

	XPLMDebugString("ofb: Disabled.\n");
	
}




PLUGIN_API void XPluginReceiveMessage(
	XPLMPluginID   inFromWho,
	intptr_t        inMessage,
	void *      inParam) {


}



