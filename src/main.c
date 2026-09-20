#include <psp2/kernel/processmgr.h>
#include <psp2/vshbridge.h> 
#include <psp2/ctrl.h>

#include <string.h>

#include "print/pspdebug.h"


#define VERSION "v1.01"

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

#define SCEGREEN   0xFF00C000
#define SCEYELLOW  0xFF00C0C0
#define SCECYAN    0xFFC0C000
#define SCEMAGENTA 0xFFC000C0


typedef struct ConsoleId {
	uint16_t unknown;
	uint16_t company_code;
	uint16_t product_code;
	uint16_t product_sub_code;
	uint8_t factory_code;
	uint32_t serial_data;
	uint32_t unique_data;
} ConsoleId;


char *getProductCode(uint16_t x) {
	switch( x ) {
		case 0x100: return "TEST";
		case 0x101: return "TOOL (Development Kit)";
		case 0x102: return "DEX (Testing Kit)";

		case 0x103: return "CEX (J1 / Japan)";
		case 0x104: return "CEX (UC2 / North America)";
		case 0x105: return "CEX (CEL / Europe)";
		case 0x106: return "CEX (KR2 / South Korea)";
		case 0x107: return "CEX (CEK / United Kingdom)";
		case 0x108: return "CEX (MX2 / Mexico)";
		case 0x109: return "CEX (AU3 / Australia & NZ)";
		case 0x10A: return "CEX (E12 / Asia)";
		case 0x10B: return "CEX (TW1 / Taiwan)";
		case 0x10C: return "CEX (RU3 / Russia)";
		case 0x10D: return "CEX (CN9 / China)";
		case 0x10E: return "CEX (HK5 / Hong Kong)";

		case 0x10F: return "CEX (RSV1)";
		case 0x110: return "CEX (RSV2)";
		case 0x111: return "CEX (RSV3)";
	}	
	return "Unknown?!";
}

char *getProductSubCode(uint16_t x) {
	switch( x ) {
		case 0x1: case 0x2:  case 0x3:
		case 0x4: case 0x5:  case 0x6:
		case 0x7: case 0x8:  case 0x9:
		case 0xA: case 0xB:  case 0xC:
		case 0xD: case 0xE:  case 0xF: 
			return "Vita 1000 (Prototype)";

		case 0x10: 
			return "Vita 1000";

		case 0x401: case 0x402: case 0x403:  
			return " ";
		
		case 0x11: case 0x12:  
			return "Vita 1000 (Variant)";

		case 0x404: case 0x406: case 0x407:  
			return "Vita 2000 (Prototype)";

		case 0x13: case 0x14:  
			return "Vita 2000";
			
		case 0x408: 
			return " ";
			
		case 0x601: case 0x602: 
			return "PlayStationTV (Prototype)";
	
		case 0x201:
			return "PlayStationTV";

		case 0x603: case 0x604: 
		case 0x605: case 0x606: 
			return " ";
		
		case 0x202:
			return "PlayStationTV (1.2g)";

		case 0x409: case 0x40A: 
		case 0x40D: case 0x40E: 
			return " ";
		
		case 0x17: case 0x18:  
			return "Vita 2000 (Variant)";
	}	
	return "Unknown?!";
}

char *getFactoryCode(uint8_t x) {
	switch(x) { // via https://github.com/TeamFAPS/PS-ConsoleId-wiki/blob/master/PS-ConsoleId-wiki.txt#L270
		case  0x1: return "Japan"; // eg PDEL
    	case  0x2: return "China #1";  
    	case  0x3: return "China #2";  // seen on most retail devices
    	case  0x4: return "China #3";  
    	case 0x23: return "Japan Diag #1"; 
    	case 0x24: return "Japan Diag #2";  // we found this on Rickys dolphin VP1 for example! (no serial yet) ->  0000 0001 0103 0010 9000 0000 C8XX XXXX   // https://wiki.henkaku.xyz/vita/SceSysmem#sceSblAIMgrIsDiagForDriver
    	case 0x3D: return "Service Center #2";
    	case 0x3E: return "Service Center #1";
    }
	return "Unknown?!";
}


int main(int argc, char *argv[]) {
	SceCtrlData pad;
	ConsoleId data;
	int i, ret;
	uint8_t CID[32];
	
	psvDebugScreenInit();
	psvDebugScreenSetXY(1, 1);

	psvDebugScreenSetTextColor(SCEGREEN);
	psvDebugScreenPrintf("ConsoleID %s", VERSION);	
	
	ret = _vshSblAimgrGetConsoleId(CID);
	if( ret != 0 ) { //error
		psvDebugScreenSetXY(1, 3);
		psvDebugScreenSetTextColor(RED);
		psvDebugScreenPrintf("An Error occured while getting the ConsoleID! (0x%08X)", ret);
		psvDebugScreenSetXY(1, 5);
		psvDebugScreenPrintf("Exiting in 3 seconds..");
		
		sceKernelDelayThread(3 * 1000000); // 3 seconds
		sceKernelExitProcess(0);
		return -1;
	}

	data.unknown          = (CID[0] << 8) | CID[1];
	data.company_code     = (CID[2] << 8) | CID[3];
	data.product_code     = (CID[4] << 8) | CID[5];
	data.product_sub_code = (CID[6] << 8) | CID[7];
	data.factory_code     = CID[8] >> 2;
	data.serial_data      = ((CID[8] >> 6) << 24) | (CID[9] << 16) | ((CID[10] << 8) | CID[11]);
	data.unique_data      = *(uint32_t*) &CID[12];

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/* 
	psvDebugScreenSetTextColor(GREY);
	
	//psvDebugScreenSetXY(9, 4); /// print samples
	//psvDebugScreenPrintf("\x10  \x11  \x12  \x13  \x14  \x15     \x16  \x17     \x18 \x19 \x1A \x1B    \x1F      \xA9 \xAA");
	
	psvDebugScreenSetXY(9, 6);
	psvDebugScreenPrintf("\x18\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x19");
	
	psvDebugScreenSetXY(9, 10);
	psvDebugScreenPrintf("\x1A\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x17\x1B");

	psvDebugScreenSetXY(9, 7);
	psvDebugScreenPrintf("\x16");
	psvDebugScreenSetXY(9, 8);
	psvDebugScreenPrintf("\x16");
	psvDebugScreenSetXY(9, 9);
	psvDebugScreenPrintf("\x16");
	
	psvDebugScreenSetXY(59, 7);
	psvDebugScreenPrintf("\x16");
	psvDebugScreenSetXY(59, 8);
	psvDebugScreenPrintf("\x16");
	psvDebugScreenSetXY(59, 9);
	psvDebugScreenPrintf("\x16");
	*/
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	psvDebugScreenSetXY(11, 8);
	for(i = 0; i < 16; i++) {
		switch( i ) {
			case 0: case 1: //
				psvDebugScreenSetTextColor(WHITE);
				break;
				
			case 2: case 3: // Company Code
				psvDebugScreenSetTextColor(AZURE);
				break;
				
			case 4: case 5: // Product Code
				psvDebugScreenSetTextColor(YELLOW);
				break;
			
			case 6: case 7: // Product Sub-Code
				psvDebugScreenSetTextColor(RED);
				break;
			
			case 8: // Factory Code
				psvDebugScreenSetTextColor(MAGENTA);
				break;
			
			case 9: case 10: case 11: // Serail Data
				psvDebugScreenSetTextColor(GREEN);
				break;
				
			 case 12: case 13: case 14: case 15: // Unique Data
				psvDebugScreenSetTextColor(ORANGE);
				break;
			
			default: psvDebugScreenSetTextColor(WHITE);
		}
			
		psvDebugScreenPrintf("%02X ", CID[i]);
	}
	
	
	/*psvDebugScreenSetXY(16, 11);
	psvDebugScreenSetTextColor(WHITE);
	psvDebugScreenPrintf("Unknown           ->   %d", data.unknown);*/
	
	psvDebugScreenSetXY(15, 14);
	psvDebugScreenSetTextColor(AZURE);
	psvDebugScreenPrintf("   Company Code   -  Sony Computer Ent."); // data.company_code
	psvDebugScreenSetXY(33, 14);
	psvDebugScreenSetTextColor(GREY);
	psvDebugScreenPrintf("-");
	
	psvDebugScreenSetXY(15, 17);
	psvDebugScreenSetTextColor(YELLOW);
	psvDebugScreenPrintf("    Product Code  -  %s", getProductCode(data.product_code));
	psvDebugScreenSetXY(33, 17);
	psvDebugScreenSetTextColor(GREY);
	psvDebugScreenPrintf("-");

	psvDebugScreenSetXY(15, 20);
	psvDebugScreenSetTextColor(RED);
	psvDebugScreenPrintf("Product Sub-Code  -  %s", getProductSubCode(data.product_sub_code));
	psvDebugScreenSetXY(33, 20);
	psvDebugScreenSetTextColor(GREY);
	psvDebugScreenPrintf("-");

	psvDebugScreenSetXY(15, 23);
	psvDebugScreenSetTextColor(MAGENTA);
	psvDebugScreenPrintf("    Factory Code  -  %s", getFactoryCode(data.factory_code));
	psvDebugScreenSetXY(33, 23);
	psvDebugScreenSetTextColor(GREY);
	psvDebugScreenPrintf("-");

	psvDebugScreenSetXY(15, 26);
	psvDebugScreenSetTextColor(GREEN);
	psvDebugScreenPrintf("          Serial  -  No. %d", data.serial_data);
	psvDebugScreenSetXY(33, 26);
	psvDebugScreenSetTextColor(GREY);
	psvDebugScreenPrintf("-");

	psvDebugScreenSetXY(15, 29);
	psvDebugScreenSetTextColor(ORANGE);
	psvDebugScreenPrintf("     Unique Data  -  0x%08X", data.unique_data);
	psvDebugScreenSetXY(33, 29);
	psvDebugScreenSetTextColor(GREY);
	psvDebugScreenPrintf("-");

	
	
	while(1) {
		memset(&pad, 0, sizeof(pad));
		sceCtrlPeekBufferPositive(0, &pad, 1);
		
		/// hide unique parts
		if( pad.buttons & SCE_CTRL_SQUARE ) {
			psvDebugScreenSetXY(17, 1);
			psvDebugScreenSetTextColor(GREY);
			psvDebugScreenPrintf("-censor");

			psvDebugScreenSetXY(41, 8);
			psvDebugScreenSetTextColor(GREEN);
			psvDebugScreenPrintf("XX XX");
			psvDebugScreenSetXY(47, 8);
			psvDebugScreenSetTextColor(ORANGE);
			psvDebugScreenPrintf("XX XX XX XX");
			
			psvDebugScreenSetXY(36, 26);
			psvDebugScreenSetTextColor(BLACK);
			psvDebugScreenPrintf("               ");
			
			psvDebugScreenSetXY(36, 29);
			psvDebugScreenSetTextColor(BLACK);
			psvDebugScreenPrintf("               ");
		}
				
		/// exit combo
		if( (pad.buttons & SCE_CTRL_START) && (pad.buttons & SCE_CTRL_SELECT) )
			break;
		
	}
	
	sceKernelExitProcess(0);
	return 0;
}
