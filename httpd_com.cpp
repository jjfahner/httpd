#include "precomp.h"
#include "httpd_com.h"
#include "httpd_i.c"
#include "httpd.h"
#include "fsresolver.h"

//////////////////////////////////////////////////////////////////////////

//
// Create a Site instance for a http_site
//

HRESULT CreateSite(http_site* forSite, ISite** pSite)
{
  // Clear result value
  *pSite = 0;

  // Create instance of Site class
  CComObject<Site>* pObject;
  if(FAILED(pObject->CreateInstance(&pObject)))
  {
    return E_UNEXPECTED;
  }

  // Retrieve site interface
  if(FAILED(pObject->QueryInterface(IID_ISite, (void**)pSite)))
  {
    return E_NOINTERFACE;
  }

  // Set reference to site
  pObject->Init(forSite);

  // Done
  return S_OK;
}


//////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE 
Server::get_Logfile(BSTR *filename)
{
  *filename = httpd.logfile().AllocSysString();
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Server::put_Logfile(BSTR filename)
{
  httpd.init_logfile(filename);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Server::get_Sites(ISites** sites)
{
  // Create instance of Site class
  CComObject<Sites>* pObject;
  if(FAILED(pObject->CreateInstance(&pObject)))
  {
    return E_UNEXPECTED;
  }

  // Retrieve site interface
  if(FAILED(pObject->QueryInterface(IID_ISites, (void**)sites)))
  {
    return E_NOINTERFACE;
  }

  // Succeeded
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Server::Start()
{
  httpd.start();
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Server::Stop()
{
  httpd.stop();
  return S_OK;
}

//////////////////////////////////////////////////////////////////////////

void 
Site::Init(http_site* site)
{
  m_site = site;
}

HRESULT STDMETHODCALLTYPE 
Site::get_Name(BSTR *name)
{
  *name = m_site->name().AllocSysString();
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Site::get_Root(BSTR *root)
{
  mime_resolver* resolver = m_site->resolver();
  filesystem_resolver* fs = dynamic_cast<filesystem_resolver*>(resolver);
  if(fs)
  {
    *root = fs->root().AllocSysString();
  }
  else
  {
    *root = String().AllocSysString();
  }
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Site::put_Root(BSTR root)
{
  m_site->set_resolver(new filesystem_resolver(m_site, root));
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Site::AddAlias(BSTR name, int port)
{
  m_site->add_alias(name, port);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Site::AddMimeType(BSTR extension, 
                  BSTR mimetype, 
                  VARIANT handler)
{
  m_site->set_mime_type(extension, mimetype);
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Site::AddDefaultDocument(BSTR document)
{
  m_site->default_docs.push_back(document);
  return S_OK;
}

//////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE 
Sites::get__NewEnum(IUnknown **ppUnk)
{
  *ppUnk = 0;
  return E_FAIL;
}

HRESULT STDMETHODCALLTYPE 
Sites::get_Item(VARIANT index, ISite **site)
{
  *site = 0;
  http_site* p = 0;

  // Find site for index
  if(index.vt == VT_INT || index.vt == VT_I4)
  {
    if(index.intVal < (int)httpd.sites().size())
    {
      http_daemon::site_iterator it;
      it = httpd.site_begin();
      std::advance(it, index.intVal);
      p = it->second;
    }
  }
  else if(index.vt == VT_BSTR)
  {
    p = httpd.get_site(index.bstrVal);
  }
  
  // No site found
  if(p == 0)
  {
    return E_INVALIDARG;
  }

  // Create the site instance
  return CreateSite(p, site);
}

HRESULT STDMETHODCALLTYPE 
Sites::get_Count(long *pVal)
{
  *pVal = httpd.sites().size();
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
Sites::Add(BSTR name, ISite **site)
{
  // Find existing site by name
  http_site* p = httpd.get_site(name);
  if(p == 0)
  {
    p = new http_site(name, true);
    httpd.add_site(p);
  }

  // Create the site instance
  return CreateSite(p, site);
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