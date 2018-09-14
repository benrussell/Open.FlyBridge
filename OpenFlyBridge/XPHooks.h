#pragma once


#include <XPLMDefs.h>

#include <XPLMUtilities.h>
#include <XPLMDataAccess.h>

#include <XPLMProcessing.h>


//Standard X-Plane plugin function hooks.
PLUGIN_API int XPluginStart(
	char *		outName,
	char *		outSig,
	char *		outDesc);

PLUGIN_API void XPluginStop(void);

PLUGIN_API int XPluginEnable(void);

PLUGIN_API void XPluginDisable(void);

PLUGIN_API void XPluginReceiveMessage(
	XPLMPluginID   inFromWho,
	intptr_t        inMessage,
	void *      inParam);




