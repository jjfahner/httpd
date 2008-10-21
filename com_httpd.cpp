#include "precomp.h"
#include "com_httpd.h"
#include "httpd.h"
#include "fsresolver.h"
#include "response.h"

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

/*static */HRESULT 
Site::Create(http_site* site, IDispatch** pDisp)
{
  // Clear result value
  *pDisp = 0;

  // Create instance of Site class
  CComObject<Site>* pObject;
  if(FAILED(pObject->CreateInstance(&pObject)))
  {
    delete pObject;
    return E_UNEXPECTED;
  }

  // Set site
  pObject->m_site = site;

  // Query for IDispatch
  return pObject->QueryInterface(IID_IDispatch, (void**)pDisp);
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
  return SitesVARIANT::Create(
    httpd.site_begin(), 
    httpd.site_begin(), 
    httpd.site_end(), 
    ppUnk);
}

HRESULT STDMETHODCALLTYPE 
Sites::get_Item(VARIANT index, ISite **site)
{
  *site = 0;
  http_site* p = 0;

  // Check for byref
  VARTYPE vt = index.vt;
  bool byref = (vt & VT_BYREF) == VT_BYREF;
  if(byref) vt &= ~VT_BYREF;

  // Find site for index
  if(vt == VT_INT || vt == VT_I4)
  {
    size_t idx = 0;
    idx = byref ? *index.pintVal : index.intVal;
    
    if(idx < (int)httpd.sites().size())
    {
      http_daemon::site_iterator it;
      it = httpd.site_begin();
      std::advance(it, index.intVal);
      p = it->second;
    }
  }
  else if(vt == VT_BSTR)
  {
    p = httpd.get_site(byref ? *index.pbstrVal : index.bstrVal);
  }
  
  // No site found
  if(p == 0)
  {
    return E_INVALIDARG;
  }

  // Create the site instance
  return Site::Create(p, (IDispatch**)site);
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
  return Site::Create(p, (IDispatch**)site);
}

//////////////////////////////////////////////////////////////////////////

/*static*/ HRESULT
SitesVARIANT::Create(iter ii, iter it, iter ie, IUnknown** ppUnk)
{
  // Create instance of Site class
  CComObject<SitesVARIANT>* pObject;
  if(FAILED(pObject->CreateInstance(&pObject)))
  {
    return 0;
  }

  // Initialize instance
  pObject->m_ii = ii;
  pObject->m_it = it;
  pObject->m_ie = ie;

  // Query for interface
  return pObject->QueryInterface(IID_IUnknown, (void**)ppUnk);
}

HRESULT STDMETHODCALLTYPE 
SitesVARIANT::Next(ULONG celt, VARIANT* rgVar, ULONG * pCeltFetched)
{
  while(celt && m_it != m_ie)
  {
    // Create site
    IDispatch* pDisp = 0;
    if(FAILED(Site::Create(m_it->second, &pDisp)))
    {
      break;
    }

    // Set type
    rgVar->vt = VT_DISPATCH;
    rgVar->pdispVal = pDisp;

    // If given, update count
    if(pCeltFetched)
    {
      ++*pCeltFetched;
    }

    // Next item
    ++m_it;
    ++rgVar;
    --celt;
  }
  return celt == 0 ? S_OK : S_FALSE;
}

HRESULT STDMETHODCALLTYPE 
SitesVARIANT::Skip(ULONG celt)
{
  while(celt && m_it != m_ie)
  {
    ++m_it;
  }
  return celt ? E_FAIL : S_OK;
}

HRESULT STDMETHODCALLTYPE 
SitesVARIANT::Reset()
{
  m_it = m_ii;
  return S_OK;
}

HRESULT STDMETHODCALLTYPE 
SitesVARIANT::Clone(IEnumVARIANT ** ppEnum)
{
  return SitesVARIANT::Create(m_ii, m_it, m_ie, (IUnknown**)ppEnum);
}
