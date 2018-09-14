#include "XPHooks.h"

#include "OpenFlyBridge.h"


//Standard X-Plane plugin function hooks.
PLUGIN_API int XPluginStart(
	char *		outName,
	char *		outSig,
	char *		outDesc) {

	strcpy_s(outName, 128, "OpenFlyBridge");
	strcpy_s(outSig, 128, "open.fly.bridge");
	strcpy_s(outDesc, 128, "v18.09.14 beta");

	// Confirm we're running.
	XPLMDebugString("OpenFlyBridge v18.09.14 beta\n");

	OpenFlyBridge::init_drefs();


	return 1;
}




PLUGIN_API void XPluginStop(void) {
	
}




PLUGIN_API int XPluginEnable(void) {

	// Init resources.
	OpenFlyBridge::open_ipc_pipe(); //Move this to the 1hz loop. Client app might not be available and will hang in a loop.

	XPLMRegisterFlightLoopCallback(
		OpenFlyBridge::cb_FlightLoop_1hz,	// Callback
		1.0f,					// Interval
		NULL);


	XPLMDebugString("ofb: Enabled.\n");

	return 1;
}

PLUGIN_API void XPluginDisable(void) {

	// Cleanup resources.
	XPLMUnregisterFlightLoopCallback(OpenFlyBridge::cb_FlightLoop_1hz, NULL);
	
	OpenFlyBridge::close_ipc_pipe();


	XPLMDebugString("ofb: Disabled.\n");
	
}




PLUGIN_API void XPluginReceiveMessage(
	XPLMPluginID   inFromWho,
	intptr_t        inMessage,
	void *      inParam) {


}



