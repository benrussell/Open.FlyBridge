#pragma once

/*
This structure defines the packet sent over the Named Pipe IPC connection to the Fly Client app.
*/


#pragma pack(show)
#pragma pack(push, r1, 2)
#pragma pack(show)
struct BridgePacket {
	int alt_msl;
	int alt_agl;
	int gs;
	int ias;
	int htrue;
	int hmag;

	double lat;
	double lon;

	int vs;
	int vs_landing;

	int g_force; // (int)9.1 * 1000; Multiply by 1000 and convert to an int.

	short reserved;

	int fuel_loaded;

	short trans_code;

	char bridge_type; //1: "fsuipc", 2: "if", 3: "xplane"
	char on_ground;
	char is_slew;
	char is_paused;

	int pitch;
	int roll;

	int fps;
};
#pragma pack(pop, r1)
#pragma pack(show)



