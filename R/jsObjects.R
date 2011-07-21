nsIDocument =
function(js)
{
  .Call("R_as_nsIDocument", js)
}

nsINode =
function(js)
{
  .Call("R_as_nsINode", js)
}

jsObject =
function(doc)
{
  y = as(doc, "nsIDocument")
  x = .Call("R_nsIDocument_getJSObject", doc)
cat("<jsObject()>\n"); print(x) ; print(y); cat("</jsObject()>\n")
  x
}

showIDocument =
function(obj)
{
  doc = nsIDocument(obj)
cat("<showIDocument>\n")
  print(doc)
  showINode(obj)
  if(!is.null(doc)) {
    cat("CharacterSetSource", .Call("R_nsIDocument_getDocumentCharacterSetSource", doc), "\n")
    cat("Content-Type", .Call("R_nsIDocument_getContentType", doc), "\n")    
    cat("BaseURI "); print(.Call("R_nsIDocument_GetBaseURI", doc))
    cat("Window "); print(.Call("R_nsIDocument_GetWindow", doc))    
  }
cat("</showIDocument>")  
  TRUE
}


showINode =
function(obj)
{
  print(nsINode(obj))
  TRUE
}


jsEval =
function(doc, script = 'alert("Hi there");', file = "no-file", line = 0L)
{
  doc = as(doc, "nsIDocument")
  jobj = jsObject(doc)
cat("<jsEval>\n")  
print(class(script))
print(script)
cat("</jsEval>\n")    
  script = paste(as.character(script), collapse = "\n")
  
  .Call("R_evalJSScript", doc, jobj, script, as.character(file), as.integer(line))
}

jsEval =
function(doc, script = 'window.status = "This is a message from R"; alert("Hi there");', file = "no-file", line = 0L)
{
  doc = as(doc, "nsIDocument")
  window = .Call("R_nsIDocument_GetWindow", doc)
  script = paste(as.character(script), collapse = "\n")
cat("Calling R_embed_evalJSScript\n")
  if(is.loaded("R_embed_evalJSScript"))
     .Call("R_embed_evalJSScript", doc, script, as.character(file), as.integer(line))  
}


testIHTMLDocument =
function(doc)
{
  win = .Call("R_nsIDocument_GetIHTMLDocument", doc)
}
