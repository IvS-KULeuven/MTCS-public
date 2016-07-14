///////////////////////////////////////////////////////////////////////////////
// SlalibDriver.h

#ifndef __SLALIBDRIVER_H__
#define __SLALIBDRIVER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TcBase.h"

#define SLALIBDRV_NAME        "SLALIB"
#define SLALIBDRV_Major       1
#define SLALIBDRV_Minor       0

#define DEVICE_CLASS CSlalibDriver

#include "ObjDriver.h"

class CSlalibDriver : public CObjDriver
{
public:
	virtual IOSTATUS	OnLoad();
	virtual VOID		OnUnLoad();

	//////////////////////////////////////////////////////
	// VxD-Services exported by this driver
	static unsigned long	_cdecl SLALIBDRV_GetVersion();
	//////////////////////////////////////////////////////
	
};

Begin_VxD_Service_Table(SLALIBDRV)
	VxD_Service( SLALIBDRV_GetVersion )
End_VxD_Service_Table


#endif // ifndef __SLALIBDRIVER_H__