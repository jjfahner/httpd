#include "precomp.h"
#include "httpd_com.h"

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
