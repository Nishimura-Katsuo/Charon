#include "headers\diablo2\D2Structs.h"


namespace D2 {
	namespace Types {
		WORD CollMap::getCollision(DWORD localx, DWORD localy, WORD mask) {
			return pMapStart[localx + localy * dwSizeGameX] & mask;
		}

		WORD Room1::getCollision(DWORD localx, DWORD localy, WORD mask) {
			return Coll->pMapStart[localx + localy * Coll->dwSizeGameX] & mask;
		}
	}
}