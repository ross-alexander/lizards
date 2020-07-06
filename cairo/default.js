function funny()
{
    var img = document.getElementById('bigbox');
    if (img)
	{
	    var pics = JSON.parse(img.getAttribute("images"));
	    if (!img.hasAttribute('index'))
		img.setAttribute('index', 0);
	    var index = img.getAttribute('index');
	    if (index < 0)
		index = pics.length - 1;
	    if (index >= pics.length)
		index = 0;
	    img.setAttribute('index', index);
	    img.setAttribute("src", pics[index].path);
	    document.getElementById("caption").innerHTML=pics[index].desc;
	}
    var icons = document.getElementsByClassName('icon');
    for (var i = 0; i < icons.length; i++)
	{
	    var icon = icons.item(i);
	    icon.setAttribute('onclick', 'icon_click(event)');
	}
    var bb = document.getElementById('bb');
    if (bb)
    {
	var img = document.getElementById('bb');
	var tmpl = JSON.parse(img.getAttribute('tmpl'));
	var sb = document.getElementById('sb');
	if (sb)
	{
	    sb.innerHTML=tmpl.desc;
	}
    }
}

function rotate(d)
{
    var img = document.getElementById('bigbox');
    var index = img.getAttribute('index');
    index = (+index) + d;
    img.setAttribute('index', index);
    funny();
}

function postie()
{
    var form = document.getElementById('bigpost');
    var img = document.getElementById('bigbox');
    var pics = JSON.parse(img.getAttribute('images'));
    var index = img.getAttribute('index');
    var id = pics[index].id;
    var hidden = document.getElementById('hidden');
    var values = {'function':'select_layout','id':id};
    hidden.setAttribute('value', JSON.stringify(values));
    form.submit();
}

function bbpost(e)
{
    var form = document.getElementById('bigpost');
    var img = document.getElementById('bb');
    var tmpl = JSON.parse(img.getAttribute('tmpl'));
    var target = e.target;
    var rect = target.getBoundingClientRect();
    var x = e.clientX - rect.left;
    var y = e.clientY - rect.top;
    var values = {'function':'show_template','x':x,'y':y,'width':rect.width,'height':rect.height,'tmpl':tmpl};
    var hidden = document.getElementById('hidden');
    hidden.setAttribute('value', JSON.stringify(values));
    form.submit();
}

function icon_click(e)
{
    var form = document.getElementById('bigpost');
    var img = document.getElementById('bb');
    var tmpl = JSON.parse(img.getAttribute('tmpl'));
    var target = e.target;

    var values = {'function':'show_template','tmpl':tmpl};

    if (target.hasAttribute('icon_type') && tmpl.active)
    {
	values.icon = target.getAttribute('icon_type');
//	var active = tmpl.active;
//	var icon = target.getAttribute('icon_type');
//	var hex;
	// for (var i = 0; i < tmpl.hexes.length; i++)
	// {
	//     var h = tmpl.hexes[i];
	//     if(h.id == active)
	//     {
	// 	hex = h;
	//     }
	// }
	// if (hex)
	// {
	//     hex.type = icon;
	// }	
	// else
	// {
	//     hex = {'id':active, 'type':icon};
	//     tmpl.hexes.push(hex);
	// }
    }
    var hidden = document.getElementById('hidden');
    hidden.setAttribute('value', JSON.stringify(values));
    form.submit();
}

window.onload=funny;
