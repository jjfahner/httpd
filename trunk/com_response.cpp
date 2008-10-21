#include "precomp.h"
#include "com_response.h"
#include "httpd.h"
#include "response.h"

//////////////////////////////////////////////////////////////////////////

/*static*/ HRESULT 
ResponseHeader::Create(http_response* response, 
                       String const& key, 
                       IDispatch** pDisp)
{
  // Clear result value
  *pDisp = 0;

  // Create instance of Site class
  CComObject<ResponseHeader>* pObject;
  if(FAILED(pObject->CreateInstance(&pObject)))
  {
    delete pObject;
    return E_UNEXPECTED;
  }

  // Set header proxy
  pObject->m_response = response;
  pObject->m_key = key;

  // Query for IDispatch
  return pObject->QueryInterface(IID_IDispatch, (void**)pDisp);
}

HRESULT STDMETHODCALLTYPE 
ResponseHeader::get_Name(BSTR *name)
{
  *name = m_key.AllocSysString();
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
ResponseHeader::get_Value(BSTR *value)
{
  *value = m_response->headers[m_key].as_str().AllocSysString();
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
ResponseHeader::put_Value(BSTR value)
{
  m_response->headers[m_key] = String(value);
  return S_OK;
}

//////////////////////////////////////////////////////////////////////////

/*static*/ HRESULT 
ResponseHeaders::Create(http_response* response, IDispatch** pDisp)
{
  // Clear result value
  *pDisp = 0;

  // Create instance of Site class
  CComObject<ResponseHeaders>* pObject;
  if(FAILED(pObject->CreateInstance(&pObject)))
  {
    delete pObject;
    return E_UNEXPECTED;
  }

  // Set header proxy
  pObject->m_response = response;

  // Query for IDispatch
  return pObject->QueryInterface(IID_IDispatch, (void**)pDisp);
}

HRESULT STDMETHODCALLTYPE 
ResponseHeaders::get__NewEnum(IUnknown **ppUnk)
{
  *ppUnk = 0;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
ResponseHeaders::get_Item(VARIANT index, IResponseHeader **pSite)
{
  if(index.vt == VT_BSTR)
  {
    return ResponseHeader::Create(m_response, 
          index.bstrVal, (IDispatch**)pSite);
  }
  return E_INVALIDARG;
}

HRESULT STDMETHODCALLTYPE 
ResponseHeaders::get_Count(long *pVal)
{
  *pVal = m_response->headers.size();
  return S_OK;
}

//////////////////////////////////////////////////////////////////////////

void 
Response::Init(http_response* response)
{
  m_response = response;
}

HRESULT STDMETHODCALLTYPE 
Response::get_Version(HttpVersion *version)
{
  if(m_response->version() == httpver_1_0)
  {
    *version = HTTP_1_0;
  }
  else if(m_response->version() == httpver_1_1)
  {
    *version = HTTP_1_1;
  }
  else
  {
    return E_UNEXPECTED;
  }
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::get_Status(BSTR *status)
{
  *status = m_response->status().AllocSysString();
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::put_Status(BSTR status)
{
  m_response->set_status(status);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::GetHeader(BSTR name, BSTR *value)
{
  *value = m_response->headers[name].as_str().AllocSysString();
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::SetHeader(BSTR name, BSTR value)
{
  m_response->headers[name] = String(value);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::SendHeaders()
{
  m_response->send_headers();
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::get_Buffer(VARIANT_BOOL *result)
{
  *result = m_response->buffered() ? 
VARIANT_TRUE : VARIANT_FALSE;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::put_Buffer(VARIANT_BOOL value)
{
  m_response->set_buffered(value != VARIANT_FALSE);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::SendHeaders(VARIANT_BOOL *result)
{
  m_response->send_headers();
  *result = VARIANT_TRUE;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::SendError(int error, VARIANT_BOOL *result)
{
  m_response->send_error(error);
  *result = VARIANT_TRUE;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::Send(void *data, int length)
{
  m_response->send((char*)data, length);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::Flush()
{
  m_response->flush();
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::Finish()
{
  m_response->finish();
  return S_OK;
}
