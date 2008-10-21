#ifndef HTTPD_COM_H
#define HTTPD_COM_H

#include <atlbase.h>
#include <atlcom.h>
#include "registrymap.h"
#include "httpd_h.h"
#include "resource.h"
#include "httpd.h"

extern CComModule _Module;

class http_site;
class http_response;

//////////////////////////////////////////////////////////////////////////

class ATL_NO_VTABLE Server : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public IDispatchImpl<IServer, &IID_IServer, &CAtlModule::m_libid, 0xFFFF, 0xFFFF>,
  public CComCoClass<Server, &IID_IServer>, 
  public ISupportErrorInfoImpl<&IID_IServer>
{
public:

  //
  // Implement IServer
  //
  HRESULT STDMETHODCALLTYPE get_Logfile(BSTR *filename);
  HRESULT STDMETHODCALLTYPE put_Logfile(BSTR filename);
  HRESULT STDMETHODCALLTYPE get_Sites(ISites** sites);
  HRESULT STDMETHODCALLTYPE Start();
  HRESULT STDMETHODCALLTYPE Stop();

  //
  // COM interface map
  //
  BEGIN_COM_MAP(Server)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IServer)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
  END_COM_MAP()

  //
  // COM registry map
  //
  DECLARE_REGISTRY_RESOURCEID_EX(IDR_REGISTRY)
  BEGIN_REGISTRY_MAP(Server)
    REGMAP_ENTRY("PROGID",      "HTTPD.Server")
    REGMAP_ENTRY("VERSION",     "1")
    REGMAP_ENTRY("DESCRIPTION", "HTTPD Server class")
    REGMAP_UUID ("CLSID",       CLSID_Server)
    REGMAP_UUID ("LIBID",       LIBID_HTTPD)
    REGMAP_ENTRY("THREADING",   "Apartment")
  END_REGISTRY_MAP()

};

//////////////////////////////////////////////////////////////////////////

class ATL_NO_VTABLE Site : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public IDispatchImpl<ISite, &IID_ISite, &CAtlModule::m_libid, 0xFFFF, 0xFFFF>,
  public CComCoClass<Site, &IID_ISite>, 
  public ISupportErrorInfoImpl<&IID_ISite>
{
public:

  //
  // Create instance
  //
  static HRESULT Create(http_site* site, IDispatch** pDisp);

  //
  // Initialize instance
  //
  void Init(http_site* site);

  //
  // Implement ISite
  //
  HRESULT STDMETHODCALLTYPE get_Name(BSTR *name);
  HRESULT STDMETHODCALLTYPE get_Root(BSTR *root);
  HRESULT STDMETHODCALLTYPE put_Root(BSTR root);
  HRESULT STDMETHODCALLTYPE AddAlias(BSTR name, int port);
  HRESULT STDMETHODCALLTYPE AddMimeType(BSTR extension, BSTR mimetype, VARIANT handler);
  HRESULT STDMETHODCALLTYPE AddDefaultDocument(BSTR document);

  //
  // COM interface map
  //
  BEGIN_COM_MAP(Site)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISite)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
  END_COM_MAP()

  //
  // COM registry map
  //
  DECLARE_REGISTRY_RESOURCEID_EX(IDR_REGISTRY)
  BEGIN_REGISTRY_MAP(Site)
    REGMAP_ENTRY("PROGID",      "HTTPD.Site")
    REGMAP_ENTRY("VERSION",     "1")
    REGMAP_ENTRY("DESCRIPTION", "HTTPD Site class")
    REGMAP_UUID ("CLSID",       CLSID_Site)
    REGMAP_UUID ("LIBID",       LIBID_HTTPD)
    REGMAP_ENTRY("THREADING",   "Apartment")
  END_REGISTRY_MAP()

private:

  //
  // Members
  //
  http_site* m_site;

};

//////////////////////////////////////////////////////////////////////////

class ATL_NO_VTABLE Sites : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public IDispatchImpl<ISites, &IID_ISites, &CAtlModule::m_libid, 0xFFFF, 0xFFFF>,
  public CComCoClass<Sites, &IID_ISites>, 
  public ISupportErrorInfoImpl<&IID_ISites>
{
public:

  //
  // Implement ISites
  //
  HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown **ppUnk);
  HRESULT STDMETHODCALLTYPE get_Item(VARIANT index, ISite **site);
  HRESULT STDMETHODCALLTYPE get_Count(long *pVal);
  HRESULT STDMETHODCALLTYPE Add(BSTR name, ISite **site);

  //
  // COM interface map
  //
  BEGIN_COM_MAP(Sites)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISites)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
  END_COM_MAP()

};

//////////////////////////////////////////////////////////////////////////

class SitesVARIANT : 
  public IEnumVARIANT,
  public CComObjectRoot
{
public:

  typedef http_daemon::site_iterator iter;

  //
  // Creation
  //
  static HRESULT Create(iter ii, iter it, iter ie, IUnknown** ppUnk);

  //
  // Implement IEnumVARIANT
  //
  HRESULT STDMETHODCALLTYPE Next(ULONG celt, VARIANT * rgVar, ULONG * pCeltFetched);           
  HRESULT STDMETHODCALLTYPE Skip(ULONG celt);
  HRESULT STDMETHODCALLTYPE Reset();
  HRESULT STDMETHODCALLTYPE Clone(IEnumVARIANT ** ppEnum); 

  //
  // COM interface map
  //
  BEGIN_COM_MAP(SitesVARIANT)
    COM_INTERFACE_ENTRY(IEnumVARIANT)
  END_COM_MAP()

  //
  // Don't allow aggregation
  //
  DECLARE_NOT_AGGREGATABLE(SitesVARIANT) 

private:

  //
  // Iterators
  //
  iter m_ii;
  iter m_it;
  iter m_ie;

};


//////////////////////////////////////////////////////////////////////////

class ATL_NO_VTABLE Request : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public IDispatchImpl<IRequest, &IID_IRequest, &CAtlModule::m_libid, 0xFFFF, 0xFFFF>,
  public CComCoClass<Request, &IID_IRequest>, 
  public ISupportErrorInfoImpl<&IID_IRequest>
{
public:

  //
  // COM interface map
  //
  BEGIN_COM_MAP(Request)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IRequest)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
  END_COM_MAP()

  //
  // COM registry map
  //
  DECLARE_REGISTRY_RESOURCEID_EX(IDR_REGISTRY)
  BEGIN_REGISTRY_MAP(Request)
    REGMAP_ENTRY("PROGID",      "HTTPD.Request")
    REGMAP_ENTRY("VERSION",     "1")
    REGMAP_ENTRY("DESCRIPTION", "HTTPD Request class")
    REGMAP_UUID ("CLSID",       CLSID_Request)
    REGMAP_UUID ("LIBID",       LIBID_HTTPD)
    REGMAP_ENTRY("THREADING",   "Apartment")
  END_REGISTRY_MAP()

};

//////////////////////////////////////////////////////////////////////////

#endif // HTTPD_COM_H
