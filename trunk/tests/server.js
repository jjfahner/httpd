function print(arg)
{
  WScript.StdOut.Write(arg + "\n");
}

function test()
{
  // Create http server
  var server = WScript.CreateObject("HTTPD.Server");

  // Add a site
  var site = server.Sites.Add("main");
  if(site == null)
  {
    print("Error: failed to add site to server");
    return;
  }

  // Check collection size    
  if(server.Sites.Count != 1)
  {
    print("Error: expected 1 site in collection");
    return;
  }

  // Add a second site
  server.Sites.Add("test1");
  server.Sites.Add("test2");

  // Enumerate sites
  for(var i = 0; i < server.Sites.Count; ++i)
  {
    print(server.Sites(i).Name);
  }

  // Retrieve it through the collection
  site = server.Sites("main");
  if(site == null)
  {
    print("Error: failed to retrieve site from collection");
    return;
  }

  site.AddAlias('*', 80);
  site.Root = 'd:\\pronto\\startscherm';
  site.AddDefaultDocument('index.htm');

  site.AddMimeType('html', 'text/html');
  site.AddMimeType('htm',  'text/html');
  site.AddMimeType('css',  'text/css');
  site.AddMimeType('gif',  'image/gif');
  site.AddMimeType('jpg',  'image/jpeg');
  site.AddMimeType('js',   'text/javascript');

  server.Start();

  WScript.Sleep(20000);
}

try
{
  print('starting...');
  test();
}
catch(ex)
{
  if(typeof(ex) == 'object')
  {
    print("Error: " + ex.message);
  }
  else
  {
    print("Error: " + ex);
  }
}

print('Done');
WScript.Sleep(1000);
