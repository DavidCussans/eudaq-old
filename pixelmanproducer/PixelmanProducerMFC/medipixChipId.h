#pragma once

#include "mpxpluginmgrapi.h"


class medipixChipId
{
public:
	DEVID deviceId;
	int chipNo;
	DevInfo deviceInfo;
	i16* databuffer;
	u32 sizeOfDataBuffer;
};