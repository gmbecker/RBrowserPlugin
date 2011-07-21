
parseEval =
function(str, env = globalenv())
{
  e = parse(text = str)
  eval(e, env)
}
