#include <psp2/kernel/processmgr.h>
#include <psp2/vshbridge.h> 
#include <psp2/ctrl.h>

#include <string.h>

#include "print/pspdebug.h"


#define VERSION "v1.00"

#define RED     0xFF0000FF
#define GREEN   0xFF00FF00
#define BLUE    0xFFFF0000
#define CYAN    0xFFFFFF00
#define MAGENTA 0xFFFF00FF
#define YELLOW  0xFF00FFFF
#define AZURE   0xFFFF7F00
#define VIOLET  0xFFFF007F
#define ORANGE	0xFF007FFF
#define GREY    0xFF808080
#define WHITE   0xFFFFFFFF
#define BLACK   0xFF000000


typedef struct ConsoleId {
	uint16_t unknown;
	uint16_t company_code;
	uint16_t product_code;
	uint16_t product_sub_code;
	uint16_t chassis_check;
	char unique_data[6];
} ConsoleId;



char *getProductCode(uint16_t x) {
	switch( x ) {
		case 0x100: return "TEST";
		case 0x101: return "TOOL (Development Kit)";
		case 0x102: return "DEX (Testing Kit)";
		case 0x103: return "CEX - Japan";
		case 0x104: return "CEX - North America";
		case 0x105: return "CEX - Europe";
		case 0x106: return "CEX - South Korea";
		case 0x107: return "CEX - United kingdom";
		case 0x108: return "CEX - Mexico";
		case 0x109: return "CEX - Australia / NZ";
		case 0x10A: return "CEX - Asia";
		case 0x10B: return "CEX - Taiwan";
		case 0x10C: return "CEX - Russia";
		case 0x10D: return "CEX - China";
	}	
	return "Unknown?!";
}

char *getProductSubCode(uint16_t x) {
	switch( x ) {
		case 0x05: return "DEM-3000";  //H
		case 0x0B: return "DEM-3000";  //L
		case 0x0A: return "CEM-3000";  //VP1
		case 0x0F: return "CEM-3000";  //NP1
		case 0x10: return "Vita Fat";        //IRS-002 ? / IRT-002 ?
		case 0x11: return "Vita Fat";        //IRS-1001 ?
		case 0x12: return "Vita Fat";        //IRS-1001 ?
		case 0x14: return "Vita Slim";       //USS-1001 ?
		case 0x18: return "Vita Slim";       //USS-1002 ?
		case 0x201: return "PlayStation TV"; //DOL-1001 ?
		case 0x202: return "PlayStation TV"; //DOL-1002 ?
	}	
	return "Unknown?!";
}


int main(int argc, char *argv[]) {
	
	SceCtrlData pad;
	ConsoleId data;
	int i, ret;
	char CID[32];
	
	psvDebugScreenInit();
	psvDebugScreenSetXY(1, 1);
	
	/// Get the ConsoleID
	ret = _vshSblAimgrGetConsoleId(CID);
	if( ret != 0 ) { //error
		psvDebugScreenSetTextColor(RED);
		psvDebugScreenPrintf("An Error occured while getting the ConsoleID! 0x%08X\n", ret);
		psvDebugScreenPrintf("Exiting in 5 seconds..");
		
		sceKernelDelayThread(5000 * 1000); //5 seconds
		sceKernelExitProcess(0);
		return -1;
	}


	data.unknown          = CID[0] * 0x100 + CID[1];
	data.company_code     = CID[2] * 0x100 + CID[3];
	data.product_code     = CID[4] * 0x100 + CID[5];
	data.product_sub_code = CID[6] * 0x100 + CID[7];
	data.chassis_check    = CID[8] * 0x100 + CID[9];


	/// Draw
	psvDebugScreenSetTextColor(WHITE);
	psvDebugScreenPrintf("Vita ConsoleID %s\n", VERSION);	
	psvDebugScreenSetXY(11, 8);
	for(i = 0; i < 16; i++) {
		switch( i ) {
			case 0: case 1: //
				psvDebugScreenSetTextColor(MAGENTA);
				break;
				
			case 2: case 3: //Company Code
				psvDebugScreenSetTextColor(AZURE);
				break;
				
			case 4: case 5: //Product Code
				psvDebugScreenSetTextColor(YELLOW);
				break;
			
			case 6: case 7: //Product Sub Code
				psvDebugScreenSetTextColor(RED);
				break;
			
			case 8: case 9: //Chassis Check
				psvDebugScreenSetTextColor(GREEN);
				break;
				
			case 10: case 11: case 12: case 13: case 14: case 15: //unique data
				psvDebugScreenSetTextColor(ORANGE);
				break;
			
			default: psvDebugScreenSetTextColor(WHITE);
		}
			
		psvDebugScreenPrintf("%02X ", CID[i]);
	}
	
	
	psvDebugScreenSetXY(16, 14);
	psvDebugScreenSetTextColor(MAGENTA);
	psvDebugScreenPrintf("Unknown           -> ");
	
	psvDebugScreenSetXY(16, 17);
	psvDebugScreenSetTextColor(AZURE);
	psvDebugScreenPrintf("Company Code      -> ");
	
	psvDebugScreenSetXY(16, 20);
	psvDebugScreenSetTextColor(YELLOW);
	psvDebugScreenPrintf("Product Code      ->   %s", getProductCode(data.product_code) );
	
	psvDebugScreenSetXY(16, 23);
	psvDebugScreenSetTextColor(RED);
	psvDebugScreenPrintf("Product Sub Code  ->   %s", getProductSubCode(data.product_sub_code) );
	
	psvDebugScreenSetXY(16, 26);
	psvDebugScreenSetTextColor(GREEN);
	psvDebugScreenPrintf("Chassis Check     ->    ");
	
	psvDebugScreenSetXY(16, 29);
	psvDebugScreenSetTextColor(ORANGE);
	psvDebugScreenPrintf("Unique Data       -> ");
	
	
	
	while(1) {
		memset(&pad, 0, sizeof(pad));
		sceCtrlPeekBufferPositive(0, &pad, 1);
		
		/// hide unique part
		if( pad.buttons & SCE_CTRL_CROSS ) {
			psvDebugScreenSetXY(41, 8);
			psvDebugScreenSetTextColor(ORANGE);
			psvDebugScreenPrintf("XX XX XX XX XX XX");
		}
		
		/// rewrite unique part
		if( pad.buttons & SCE_CTRL_CIRCLE ) {
			psvDebugScreenSetXY(41, 8);
			psvDebugScreenSetTextColor(ORANGE);
			for(i = 10; i < 16; i++) {
				psvDebugScreenPrintf("%02X ", CID[i]);
			}
		}
		
		/// exit combo
		if( (pad.buttons & SCE_CTRL_START) && (pad.buttons & SCE_CTRL_SELECT) )
			break;
		
	}
	
	sceKernelExitProcess(0);
	return 0;
}
