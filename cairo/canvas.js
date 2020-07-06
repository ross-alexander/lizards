var swamp;

// ----------------------------------------------------------------------
//
// Point
//
// ----------------------------------------------------------------------

var Point = function(xp, yp)
{
    this.x = xp;
    this.y = yp;
}

function paint(f)
{
//    var canvas = document.getElementById('canvas')
//    canvas.width = swamp.width;
//    canvas.height = swamp.height;
//        console.log(canvas)

    if (!swamp)
	return;
    
    console.log(swamp)

    let radius = 100;
    let border = 5;
    
    var svg = document.getElementById('svg');

//    var otherCanvas = document.getElementById("wrapped_canvas");
//    var wrappedCtx = otherCanvas.getContext("2d");   
//    var ctx = new C2S({ctx:wrappedCtx, width:swamp.width, height:swamp.height});


    let width = swamp.width * radius * 2 + border * 2;
    let height = swamp.height * radius * 2 + border * 2;
    
    var ctx = new C2S(width, height);
    
//    var ctx = canvas.getContext('2d')

    
//    ctx.fillRect(0, 0, canvas.width, canvas.height)
//    var img = new Image();
//    img.src = 'full-1.svg';
    ctx.save()

    ctx.fillStyle = "skyblue"
    ctx.fillRect(0, 0, width, height)

    //    ctx.drawImage(img, 0, 0, swamp.w, swamp.h);
//    ctx.translate(-swamp.bounds.left, -swamp.bounds.top);

    ctx.lineWidth = 0.5
    ctx.strokeStyle = 'black'
    ctx.translate((-swamp.min_x * 2 + 1) * radius, (-swamp.min_y * 2 + 1) * radius)
    var hexes = swamp.hexes
    Object.keys(hexes).map(function f(id) {
	let hex = hexes[id]
	let ofs = Math.abs(hex.x%2)

	let center = new Point(hex.x * radius * 1.5, (hex.y * 2 - ofs) * radius * Math.sqrt(3)/2)
	
	ctx.beginPath()

	for (let i = 0; i < 6; i++)
	{
	    if (i > 0)
		ctx.lineTo(center.x + Math.cos(Math.PI / 3 * i) * radius, center.y + Math.sin(Math.PI / 3 * i) * radius)
	    else
		ctx.moveTo(center.x + Math.cos(Math.PI / 3 * i) * radius, center.y + Math.sin(Math.PI / 3 * i) * radius)
	}
	ctx.closePath();
	ctx.fillStyle = "green";
     	ctx.fill();
	ctx.stroke();
    // 	if (hex.symbol)
    // 	{
    // 	    var img = new Image();
    // 	    img.src = hex.symbol;
    // 	    console.log(img);
    // 	    ctx.save();
    // 	    ctx.translate(hex.center.x - 20, hex.center.y - 15);
    // 	    ctx.scale(0.20, 0.20);
    // 	    ctx.drawImage(img, 0, 0);
    // 	    ctx.restore();
    // 	}
	}
    )
    ctx.restore();

    if (svg.children.length > 0)
    {
        svg.removeChild(svg.children[0]);
    } 
    
    svg.appendChild(ctx.getSvg());
}

var layout_t = function(l)
{
    this.id = l.id;
    this.cost = l.cost;
    this.hexes = {};
    
    var max_x;
    var min_x;
    var max_y;
    var min_y;

    for (let i = 0; i < l.hexes.length; i++)
    {
	let x = l.hexes[i][1];
	let y = l.hexes[i][2];

	max_x = (x > max_x) || (i == 0) ? x : max_x;
	min_x = (x < min_x) || (i == 0) ? x : min_x;
	
	max_y = (y > max_y) || (i == 0) ? y : max_y;
	min_y = (y < min_y) || (i == 0) ? y : min_y;
	this.hexes[l.hexes[i][0]] = { x: x, y: y };
    }

    this.max_x = max_x;
    this.min_x = min_x;
    this.max_y = max_y;
    this.min_y = min_y;
    this.width = max_x - min_x + 1;
    this.height = max_y - min_y + 1;
}

layout_t.prototype.toString = function()
{
    return "layout_t(" + this.id + ":" + this.width + ", " + this.height + ")";
}


window.onload = function(f)
{

    fetch('LAYOUT.JS', {
	method: 'get'
    }).then(function(response) {
	return  response.json() }).then(function(layouts) {
	    console.log(layouts)
	    var lobj = layouts.map(function(l) { return new layout_t(l); });
	    var id = lobj.join(" + ");
	    document.getElementById("spam").innerHTML = id;
	    swamp = lobj[3];
    }).catch(function(err) {
	console.log(err)
	// Error :(
    });
        
//    var httpreq = new XMLHttpRequest(); // a new request
//    httpreq.open("GET", "LAYOUT.JS", false);
//    httpreq.send(null);
//    var text = httpreq.responseText;
//    var layouts = JSON.parse(text);


    //    console.log(f)
    //    console.log(document)
    //    console.log(swamp)
}

window.onresize = function(f)
{
    paint(f)
}
