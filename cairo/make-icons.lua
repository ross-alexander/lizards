require "lizards"

-- Create single hex layout

local layouts = lizards.layouts_t(lizards.FORMAT_STRING_JS, "[{'id':1,'hexes':[[1,0,0]]}]")

local templates = {
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'WAT','title':'WAT'}]}", image = "images/WAT.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'PLA','title':'PLA'}]}", image = "images/PLA.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'SCR','title':'SCR'}]}", image = "images/SCR.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'SWA','title':'SWA'}]}", image = "images/SWA.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'FER','title':'FER'}]}", image = "images/FER.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'RUI','title':'RUI'}]}", image = "images/RUI.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'TEM','title':'TEM'}]}", image = "images/TEM.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'PEA','title':'PEA'}]}", image = "images/PEA.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'VOL','title':'VOL'}]}", image = "images/VOL.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'CUR','title':'CUR'}]}", image = "images/CUR.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'WHI','title':'WHI'}]}", image = "images/WHI.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'RED','title':'RED'}]}", image = "images/RED.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'BLK','title':'BLK'}]}", image = "images/BLK.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'GRN','title':'GRN'}]}", image = "images/GRN.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'GRY','title':'GRY'}]}", image = "images/GRY.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'YEL','title':'YEL'}]}", image = "images/YEL.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'type':'DEN','title':'HOME'}]}", image = "images/HOME.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'title':'GRN','band':{'GRN':10}}]}", image = "images/GRN_add.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'title':'GRY','band':{'GRY':10}}]}", image = "images/GRY_add.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'title':'BLK','band':{'BLK':10}}]}", image = "images/BLK_add.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'title':'YEL','band':{'YEL':10}}]}", image = "images/YEL_add.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'title':'RED','band':{'RED':10}}]}", image = "images/RED_add.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'title':'GRN','band':{'GRN':10}}]}", image = "images/GRN_sub.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'title':'GRY','band':{'GRY':10}}]}", image = "images/GRY_sub.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'title':'BLK','band':{'BLK':10}}]}", image = "images/BLK_sub.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'title':'YEL','band':{'YEL':10}}]}", image = "images/YEL_sub.svg" },
   { tmpl = "{'layout':1,'hexes':[{'id':1,'title':'RED','band':{'RED':10}}]}", image = "images/RED_sub.svg" },
}

for i = 1, #templates, 1 do
   local t = templates[i]
   local template = lizards.template_t(layouts, lizards.FORMAT_STRING_JS, t.tmpl)
   local grid = template:realize()
   grid:output():svg(t.image, 0)
end
