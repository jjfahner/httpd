/** \file comlib/registrymap.hpp
  * Defines a 'registry' map for adding variables to rgs files.
  * Copyright 2001-2003 Michael Geddes.  All rights reserved.
  */

#ifndef COMLIB_REGISTRYMAP_HPP
#define COMLIB_REGISTRYMAP_HPP

#include <atlbase.h>

struct _ATL_REGMAP_ENTRYKeeper : public _ATL_REGMAP_ENTRY
{
	_ATL_REGMAP_ENTRYKeeper(){ szKey=NULL; szData=NULL;}
	_ATL_REGMAP_ENTRYKeeper(LPCOLESTR key, LPCOLESTR data) 
	{
		szKey=key;
		LPOLESTR newData;
		szData = LPCOLESTR(newData =  new wchar_t[wcslen(data)+1]);
		wcscpy(newData,data);
	}
	_ATL_REGMAP_ENTRYKeeper(LPCOLESTR key, UINT resid)
	{
		static wchar_t Data[256];
		USES_CONVERSION;

		szKey=key;
		if( 0 == LoadStringW(_pModule->m_hInstResource, resid, Data, 255))
		{
			*Data = '\0';
		}

		int	length = wcslen(Data);

		szData = new wchar_t[length];
		wcscpy(const_cast<wchar_t *>(szData),Data);
	}

	_ATL_REGMAP_ENTRYKeeper(LPCOLESTR key, REFGUID guid) 
	{
		szKey=key;
		LPOLESTR newData;
		szData = LPCOLESTR(newData =  new wchar_t[40]);
		if(szData!=NULL)
		{
			if(0==StringFromGUID2(guid, newData,40))
			{
				*newData=NULL;
			}
		}
	}
	~_ATL_REGMAP_ENTRYKeeper()
	{
		delete [] (LPOLESTR)szData;
	}
};

#define BEGIN_REGISTRY_MAP(x) \
	static struct _ATL_REGMAP_ENTRY *_GetRegistryMap()\
	{\
		static const _ATL_REGMAP_ENTRYKeeper map[] = {
#define REGMAP_ENTRY(x,y) _ATL_REGMAP_ENTRYKeeper(OLESTR(##x),OLESTR(##y)),

#define REGMAP_RESOURCE(x,resid) _ATL_REGMAP_ENTRYKeeper(OLESTR(##x),), 

#define REGMAP_UUID(x,clsid) _ATL_REGMAP_ENTRYKeeper(OLESTR(##x),clsid),

#define END_REGISTRY_MAP() _ATL_REGMAP_ENTRYKeeper() }; return (_ATL_REGMAP_ENTRY *)map;}

#define DECLARE_REGISTRY_RESOURCEID_EX(x)\
	static HRESULT WINAPI UpdateRegistry(BOOL bRegister)\
	{\
		return _Module.UpdateRegistryFromResource((UINT)x, bRegister, _GetRegistryMap() );\
	}


#endif /* COMLIB_REGISTRYMAP_HPP */
