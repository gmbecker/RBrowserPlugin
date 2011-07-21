\name{RFirefox}
\alias{RFirefox}
\title{RFirefox package}
\description{This is a package that provides the foundations
  for embedding R within Firefox and also allowing R programmers
  access to objects within Firefox at run-time.
  In other words, when the R engine is embedded in Firefox and
  code of some sort (e.g. JavaScript within an HTML or SVG document
  displayed in the browser, or JavaScript code run at startup via
  a XUL interface) passes control to an R function (or evaluating R
  code as a string!), then that R code can access
  global objects in Firefox and the objects it is passed from
  the JavaScript code. As a result, we can invoke methods
  of available JavaScript objects, walk the DOM, manipulate
  Firefox itself, access the singletons in XPCOM and utilize
  Firefox services.
}
\author{Duncan Temple Lang}

\keyword{programming}
\concept{interactive documents}
\concept{HTML}
\concept{JavaScript}
  