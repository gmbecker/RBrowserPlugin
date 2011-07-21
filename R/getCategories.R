
getCategories =
function(deep = TRUE)
{
  .Call("R_getCategories", as.logical(deep))
}
