function print(arg)
{
  WScript.StdOut.Write(arg + "\n");
}

function test()
{
    var server = WScript.CreateObject("HTTPD.Server");
    
    var site = server.AddSite("main");
    
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
