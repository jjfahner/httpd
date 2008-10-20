#ifndef HTTPD_COM_H
#define HTTPD_COM_H

#include <atlbase.h>
#include <atlcom.h>
#include "httpd_h.h"
#include "registrymap.h"
#include "resource.h"

extern CComModule _Module;

//////////////////////////////////////////////////////////////////////////

class Server : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public IDispatchImpl<IServer, &IID_IServer, &CAtlModule::m_libid, 0xFFFF, 0xFFFF>,
  public CComCoClass<Server, &IID_IServer>, 
  public ISupportErrorInfoImpl<&IID_IServer>
{
public:

  //
  // Implement IServer
  //
  virtual HRESULT STDMETHODCALLTYPE GetSite(BSTR name, ISite **site);
  virtual HRESULT STDMETHODCALLTYPE AddSite(BSTR name, ISite **site);
  virtual HRESULT STDMETHODCALLTYPE RemoveSite(BSTR name);
  virtual HRESULT STDMETHODCALLTYPE Start();
  virtual HRESULT STDMETHODCALLTYPE Stop();

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

class Site : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public IDispatchImpl<ISite, &IID_ISite, &CAtlModule::m_libid, 0xFFFF, 0xFFFF>,
  public CComCoClass<Site, &IID_ISite>, 
  public ISupportErrorInfoImpl<&IID_ISite>
{
public:

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
};

//////////////////////////////////////////////////////////////////////////

class Request : 
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

class Response : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public IDispatchImpl<IResponse, &IID_IResponse, &CAtlModule::m_libid, 0xFFFF, 0xFFFF>,
  public CComCoClass<Response, &IID_IResponse>, 
  public ISupportErrorInfoImpl<&IID_IResponse>
{
public:

  //
  // Implement IResponse
  //
  virtual HRESULT STDMETHODCALLTYPE GetHeader(BSTR name, BSTR *value);
  virtual HRESULT STDMETHODCALLTYPE SetHeader(BSTR name, BSTR value);
  virtual HRESULT STDMETHODCALLTYPE SendHeaders();
  virtual HRESULT STDMETHODCALLTYPE get_Buffer(VARIANT_BOOL *result);
  virtual HRESULT STDMETHODCALLTYPE put_Buffer(VARIANT_BOOL value);
  virtual HRESULT STDMETHODCALLTYPE Flush();
  virtual HRESULT STDMETHODCALLTYPE Send(void *data, int length);
  
  //
  // COM Interface map
  //
  BEGIN_COM_MAP(Response)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IResponse)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
  END_COM_MAP()

  //
  // Registry map
  //
  DECLARE_REGISTRY_RESOURCEID_EX(IDR_REGISTRY)
  BEGIN_REGISTRY_MAP(Response)
    REGMAP_ENTRY("PROGID",      "HTTPD.Response")
    REGMAP_ENTRY("VERSION",     "1")
    REGMAP_ENTRY("DESCRIPTION", "HTTPD Response class")
    REGMAP_UUID ("CLSID",       CLSID_Response)
    REGMAP_UUID ("LIBID",       LIBID_HTTPD)
    REGMAP_ENTRY("THREADING",   "Apartment")
  END_REGISTRY_MAP()

};

//////////////////////////////////////////////////////////////////////////

#endif // HTTPD_COM_H
