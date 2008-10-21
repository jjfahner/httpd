Sub Print(arg)

  WScript.Echo arg

End Sub

Sub Test()

  ' Create http server
  Set server = WScript.CreateObject("HTTPD.Server")

  ' Add a site
  Set site = server.Sites.Add("main")
  If site Is Nothing Then
    Print "Error: failed to add site to server"
    Return
  End If

  ' Check collection size    
  If server.Sites.Count <> 1 Then
    Print "Error: expected 1 site in collection"
    Return
  End If

  ' Add a second site
  server.Sites.Add "test1"
  server.Sites.Add "test2"

  ' Enumerate sites using IEnumVARIANT
  For Each site In server.Sites
    Print site.Name
  Next

  ' Enumerate sites using count
  'For i = 0 To server.Sites.Count - 1
  '    Print server.Sites(i).Name
  'Next

  ' Retrieve it through the collection
  Set site = server.Sites("main")
  If site Is Nothing Then
    Print "Error: failed to retrieve site from collection"
    Return
  End If

  site.AddAlias "*", 80
  site.Root = "d:\\pronto\\startscherm"
  site.AddDefaultDocument "index.htm"

  site.AddMimeType "html", "text/html"
  site.AddMimeType "htm",  "text/html"
  site.AddMimeType "css",  "text/css"
  site.AddMimeType "gif",  "image/gif"
  site.AddMimeType "jpg",  "image/jpeg"
  site.AddMimeType "js",   "text/javascript"

  server.Start

  WScript.Sleep 20000
  
End Sub

Print "starting..."

Test

Print "Done"

WScript.Sleep 1000
