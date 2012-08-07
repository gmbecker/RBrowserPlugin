function carsDat(request)
{
    var json =  R.getCarDat(request);
    var obj = JSON.parse(json);
    
    //unfortunately the YahooUI DataSource is row-centric, while the data.frame in R is column centric so we need to transform the data
    var arr = [];
    var row = [];
    for (var i =0; i< obj["mpg"].length; i++) 
	{
	    row = [];
	    for (var field in obj)
		{
		    row.push(obj[field][i]);
		}
	    arr.push(row);
	}
    
    return arr;
}

function editRData(fnCallback, oNewValue)
{
    var rownum = this._oDataTable.getRecordIndex(this._oRecord);
    var colnum = this._oColumn._nKeyIndex;
    //remember R indexing starts at 1!!!
    //callRFunction("editCarDat", [rownum + 1, colnum + 1, oNewValue]);
    R.editCarDat( [rownum + 1, colnum + 1, oNewValue]);
    fnCallback(true, oNewValue);
				 
} 

var dsFunction = new YAHOO.util.FunctionDataSource(carsDat);
//     dsFunction.responseType = TYPE_JSON;
dsFunction.responseSchema = {
    //    resultsList : "results",
    fields : [ "mpg" , "cyl" , "disp", "hp", "drat", "wt", "qsec", "vs", "am", "gear", "carb" ] 
};
var myColumnDefs = [
		    {key:"mpg", label :"mpg", editor: new YAHOO.widget.TextboxCellEditor({ validator: YAHOO.widget.DataTable.validateNumber, asyncSubmitter: editRData } ) },
		    {key:"cyl", label :"cyl", editor: new YAHOO.widget.TextboxCellEditor({ validator: YAHOO.widget.DataTable.validateNumber, asyncSubmitter: editRData } ) },
		    {key:"disp", label :"disp", editor: new YAHOO.widget.TextboxCellEditor({ validator: YAHOO.widget.DataTable.validateNumber, asyncSubmitter: editRData } ) },
		    {key:"hp", label :"hp", 
		     editor: new YAHOO.widget.TextboxCellEditor({ 
			     validator: YAHOO.widget.DataTable.validateNumber,
			     asyncSubmitter:  function(fnCallback, oNewValue) 
			     {

				 
			     }

			     } ), },
		    {key:"drat", label :"drat", editor: new YAHOO.widget.TextboxCellEditor({ validator: YAHOO.widget.DataTable.validateNumber, asyncSubmitter: editRData } ) },
		    {key:"wt", label :"wt", editor: new YAHOO.widget.TextboxCellEditor({ validator: YAHOO.widget.DataTable.validateNumber, asyncSubmitter: editRData } ) },
		    {key:"qsec", label :"qsec", editor: new YAHOO.widget.TextboxCellEditor({ validator: YAHOO.widget.DataTable.validateNumber, asyncSubmitter: editRData } ) },
		    {key:"vs", label :"vs", editor: new YAHOO.widget.TextboxCellEditor({ validator: YAHOO.widget.DataTable.validateNumber, asyncSubmitter: editRData } ) },
		    {key:"am", label :"am", editor: new YAHOO.widget.TextboxCellEditor({ validator: YAHOO.widget.DataTable.validateNumber, asyncSubmitter: editRData } ) },        
		    {key:"gear", label :"gear", editor: new YAHOO.widget.TextboxCellEditor({ validator: YAHOO.widget.DataTable.validateNumber, asyncSubmitter: editRData } ) },
		    {key:"carb", label :"carb", editor: new YAHOO.widget.TextboxCellEditor({ validator: YAHOO.widget.DataTable.validateNumber, asyncSubmitter: editRData } ) }
		    ];

// Defer instantiation
function doinstantiate()
{
    var myDataTableDeferred = new YAHOO.widget.ScrollingDataTable("table", myColumnDefs, dsFunction, {height: "100px"});
    myDataTableDeferred.subscribe("cellClickEvent", myDataTableDeferred.onEventShowCellEditor);
};


YAHOO.util.Event.addListener(window, "load", doinstantiate);
 


  