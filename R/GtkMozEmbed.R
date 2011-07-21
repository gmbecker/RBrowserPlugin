initGtkMozEmbed = function()
  {
    .Call("R_InitGTKMozEmbed")
  }


gtkMozNewBrowser = function()
  {
    .Call("R_NewMozGtkBrowser")
  }

gtkMozGetnsIWB = function(browser)
  {
    .Call("R_gtk_moz_embed_get_nsIWebBrowser", browser)
  }

gtkMozLoadURL = function(browser, URL)
  {
    .Call("R_LoadURL", browser, URL)
  }
    
