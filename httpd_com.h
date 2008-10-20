#ifndef HTTPD_COM_H
#define HTTPD_COM_H

#include <atlbase.h>
#include <atlcom.h>
#include "httpd_h.h"

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
  // Update registry
  //
  static HRESULT WINAPI UpdateRegistry(BOOL) { return S_OK; }

  //
  // COM Interface map
  //
  BEGIN_COM_MAP(Server)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IServer)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
  END_COM_MAP()

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
  // Update registry
  //
  static HRESULT WINAPI UpdateRegistry(BOOL) { return S_OK; }

  //
  // COM Interface map
  //
  BEGIN_COM_MAP(Site)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ISite)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
  END_COM_MAP()

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
  // Update registry
  //
  static HRESULT WINAPI UpdateRegistry(BOOL) { return S_OK; }

  //
  // COM Interface map
  //
  BEGIN_COM_MAP(Request)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IRequest)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
  END_COM_MAP()

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
  // Update registry
  //
  static HRESULT WINAPI UpdateRegistry(BOOL) { return S_OK; }
  
  //
  // COM Interface map
  //
  BEGIN_COM_MAP(Response)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IResponse)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
  END_COM_MAP()

};

//////////////////////////////////////////////////////////////////////////

#endif // HTTPD_COM_H
