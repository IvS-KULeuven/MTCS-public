///////////////////////////////////////////////////////////////////////////////
// SlalibCtrl.h

#ifndef __SLALIBCTRL_H__
#define __SLALIBCTRL_H__

#include <atlbase.h>
#include <atlcom.h>

#define SLALIBDRV_NAME "SLALIB"

#include "resource.h"       // main symbols
#include "SlalibW32.h"
#include "TcBase.h"
#include "SlalibClassFactory.h"
#include "TcOCFCtrlImpl.h"

class CSlalibCtrl 
	: public CComObjectRootEx<CComMultiThreadModel>
	, public CComCoClass<CSlalibCtrl, &CLSID_SlalibCtrl>
	, public ISlalibCtrl
	, public ITcOCFCtrlImpl<CSlalibCtrl, CSlalibClassFactory>
{
public:
	CSlalibCtrl();
	virtual ~CSlalibCtrl();

DECLARE_REGISTRY_RESOURCEID(IDR_SLALIBCTRL)
DECLARE_NOT_AGGREGATABLE(CSlalibCtrl)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CSlalibCtrl)
	COM_INTERFACE_ENTRY(ISlalibCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl)
	COM_INTERFACE_ENTRY(ITcCtrl2)
END_COM_MAP()

};

#endif // #ifndef __SLALIBCTRL_H__
