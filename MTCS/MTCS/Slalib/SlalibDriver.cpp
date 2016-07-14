///////////////////////////////////////////////////////////////////////////////
// SlalibDriver.cpp
#include "TcPch.h"
#pragma hdrstop

#include "SlalibDriver.h"
#include "SlalibClassFactory.h"

DECLARE_GENERIC_DEVICE(SLALIBDRV)

IOSTATUS CSlalibDriver::OnLoad( )
{
	TRACE(_T("CObjClassFactory::OnLoad()\n") );
	m_pObjClassFactory = new CSlalibClassFactory();

	return IOSTATUS_SUCCESS;
}

VOID CSlalibDriver::OnUnLoad( )
{
	delete m_pObjClassFactory;
}

unsigned long _cdecl CSlalibDriver::SLALIBDRV_GetVersion( )
{
	return( (SLALIBDRV_Major << 8) | SLALIBDRV_Minor );
}

