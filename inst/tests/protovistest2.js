
var vis = new pv.Panel().canvas('mycanvas')
.width(300)
.height(300)
.add (pv.Line)
.data ([1,0.5, 0.1, 0.01, 0.001, 0.3, 0.2,0.1,1])
.left (function () { return this.index * 30; })
.bottom (function (d) { return d * 250; });

vis.root.render();
