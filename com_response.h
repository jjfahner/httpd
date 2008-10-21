#ifndef HTTPD_RESPONSE_COM_H
#define HTTPD_RESPONSE_COM_H

#include <atlbase.h>
#include <atlcom.h>
#include "registrymap.h"
#include "resource.h"
#include "httpd_h.h"
#include "response.h"

extern CComModule _Module;

//////////////////////////////////////////////////////////////////////////

class ATL_NO_VTABLE ResponseHeader : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public IDispatchImpl<IResponseHeader, &IID_IResponseHeader, 
                       &CAtlModule::m_libid, 0xFFFF, 0xFFFF>
{
public:
  
  //
  // Create instance
  //
  static HRESULT Create(http_response* response, 
          String const& key, IDispatch** pDisp);

  //
  // Implement IResponseHeader
  //
  HRESULT STDMETHODCALLTYPE get_Name(BSTR *name);
  HRESULT STDMETHODCALLTYPE get_Value(BSTR *value);
  HRESULT STDMETHODCALLTYPE put_Value(BSTR value);

  //
  // COM Interface map
  //
  BEGIN_COM_MAP(ResponseHeader)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IResponseHeader)
  END_COM_MAP()

private:

  //
  // Members
  //
  http_response* m_response;
  String m_key;

};

//////////////////////////////////////////////////////////////////////////

class ATL_NO_VTABLE ResponseHeaders : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public IDispatchImpl<IResponseHeaders, &IID_IResponseHeaders, 
  &CAtlModule::m_libid, 0xFFFF, 0xFFFF>
{
public:

  //
  // Create instance
  //
  static HRESULT Create(http_response* response, IDispatch** pDisp);

  //
  // Implement IResponseHeaders
  //
  HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown **ppUnk);
  HRESULT STDMETHODCALLTYPE get_Item(VARIANT index, IResponseHeader **pHeader);
  HRESULT STDMETHODCALLTYPE get_Count(long *pVal);

  //
  // COM Interface map
  //
  BEGIN_COM_MAP(ResponseHeaders)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(IResponseHeaders)
  END_COM_MAP()

private:

  //
  // Members
  //
  http_response* m_response;

};

//////////////////////////////////////////////////////////////////////////

class ATL_NO_VTABLE Response : 
  public CComObjectRootEx<CComMultiThreadModel>,
  public IDispatchImpl<IResponse, &IID_IResponse, &CAtlModule::m_libid, 0xFFFF, 0xFFFF>,
  public CComCoClass<Response, &IID_IResponse>, 
  public ISupportErrorInfoImpl<&IID_IResponse>
{
public:

  //
  // Initialize response
  //
  void Init(http_response* response);

  //
  // Implement IResponse
  //
  HRESULT STDMETHODCALLTYPE get_Version(HttpVersion *version);
  HRESULT STDMETHODCALLTYPE get_Status(BSTR *status);
  HRESULT STDMETHODCALLTYPE put_Status(BSTR status);
  HRESULT STDMETHODCALLTYPE GetHeader(BSTR name, BSTR *value);
  HRESULT STDMETHODCALLTYPE SetHeader(BSTR name, BSTR value);
  HRESULT STDMETHODCALLTYPE SendHeaders();
  HRESULT STDMETHODCALLTYPE get_Buffer(VARIANT_BOOL *result);
  HRESULT STDMETHODCALLTYPE put_Buffer(VARIANT_BOOL value);
  HRESULT STDMETHODCALLTYPE SendHeaders(VARIANT_BOOL *result);
  HRESULT STDMETHODCALLTYPE SendError(int error, VARIANT_BOOL *result);
  HRESULT STDMETHODCALLTYPE Send(void *data, int length);
  HRESULT STDMETHODCALLTYPE Flush();
  HRESULT STDMETHODCALLTYPE Finish();

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

private:

  //
  // Members
  //
  http_response* m_response;

};

#endif // HTTPD_RESPONSE_COM_H
