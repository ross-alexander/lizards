function start() {
    var fish = document.getElementById('current');
    fish.innerHTML="Fred";

    var embeds = document.getElementsByTagName('embed');
    for (i = 0; i < embeds.length; i++)
    {
	var embed = embeds[i];
	console.log(embed.id);

	var svg = embed.getSVGDocument();
	svg.hexId = embed.id;

	var styleElement = svg.createElementNS("http://www.w3.org/2000/svg", "style");
	styleElement.textContent = "svg { pointer-events: none; } svg path { pointer-events: fill; }";

	var defs = svg.getElementsByTagName('defs')
	defs[0].appendChild(styleElement);
    
	svg.addEventListener("click", clicker_object, false);
    }
}

function clicker(i)
{
    var fish = document.getElementById('fish');
    fish.innerHTML=i.getAttribute('src');
}

function map(i)
{
    var fish = document.getElementById('current');
    fish.innerHTML=i;
}



function clicker_object(e)
{
    var fish = document.getElementById('current');
    fish.innerHTML=e.currentTarget.hexId;
}

window.onload=start;
