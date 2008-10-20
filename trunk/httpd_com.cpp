#include "precomp.h"
#include "httpd_com.h"
#include "httpd_i.c"

//////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE 
Server::GetSite(BSTR name, ISite **site)
{
  *site = 0;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Server::AddSite(BSTR name, ISite **site)
{
  *site = 0;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Server::RemoveSite(BSTR name)
{
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Server::Start()
{
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Server::Stop()
{
  return S_OK;
}

//////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE 
Response::GetHeader(BSTR name, BSTR *value)
{
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::SetHeader(BSTR name, BSTR value)
{
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::SendHeaders()
{
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::get_Buffer(VARIANT_BOOL *result)
{
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::put_Buffer(VARIANT_BOOL value)
{
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::Flush()
{
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Response::Send(void *data, int length)
{
  return S_OK;
}
