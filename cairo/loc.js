function loaded() {
    var root = document.documentElement;
    root.setAttribute('onclick', 'clicked(event)');
}

function clicked(event)
{
    var hotspot = document.getElementById('hotspot');
    var t = event.target;
    var rect = t.getBoundingClientRect();
    var x = event.clientX  - rect.left;
    var y = event.clientY - rect.top;
    var s = {'x':x, 'y':y};
    hotspot.innerHTML=JSON.stringify(s);
}

window.onload=loaded;
