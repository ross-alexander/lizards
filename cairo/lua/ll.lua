-- ----------------------------------------------------------------------
--
-- lualizards
--
-- Uses SWIG to create shared object library
--
-- ----------------------------------------------------------------------

local lizards = require("lizards")
local json = require("json")

local game = lizards.game_t(lizards.FORMAT_FILE_JS, "game.js")

local grid = game.grid

local serial = grid:serialize()
serial:to_file("grid.js")

output = lizards.output_t(game, 0)
output:svg("ll.svg")

if (false) then
   
   local f = io.open("single-embed.html", "w")

   f:write("<html>\n")
   f:write("<head>\n")
   f:write("<title>Lizards!</title>\n")
   f:write("<link type=\"text/css\" rel=\"stylesheet\" href=\"default.css\"/>\n")
   f:write("<script language=\"JavaScript\" src=\"default.js\"> </script>\n")
   f:write("</head>\n")
   f:write("<body>\n")
   f:write("<div>Current hex: <span id=\"current\">none</span></div>\n")
   
   for i = 1, game.nplayers, 1 do
      local file = string.format("p%02d-%%02d.svg", i)
      --   print(file)
      local singles = output:single(file, i)
      local left = singles.bounds.left;
      local top = singles.bounds.top;
      
      print(left, top)
      
      f:write(string.format("<div id=\"map\" style=\"position:relative; width:%fpt; height:%fpt;\">\n", singles.bounds.right-singles.bounds.left, singles.bounds.bottom-singles.bounds.top))
      
      for j = 0, singles:size()-1, 1
      do
	 local s = singles.singles[j]
	 print(s.id, s.center.x, s.center.y, s.file)
	 local w = s.bounds.right - s.bounds.left;
	 local h = s.bounds.bottom - s.bounds.top;
	 f:write(string.format("<embed id=\"%s\" src=\"%s\" style=\"position:absolute; left:%fpt; top: %fpt;\"/>\n", s.id, s.file, s.bounds.left - left, s.bounds.top - top))
      end
      f:write("</div>\n")
   end
   f:write("</body></html>\n")
   
   
   local f = io.open("full.html", "w")
   f:write("<html>\n")
   f:write("<head>\n")
   f:write("<title>Lizards!</title>\n")
   f:write("<link type=\"text/css\" rel=\"stylesheet\" href=\"default.css\"/>\n")
   f:write("<script language=\"JavaScript\" src=\"default.js\"> </script>\n")
   f:write("</head>\n")
   f:write("<body>\n")
   f:write("<div>Current hex: <span id=\"current\">none</span></div>\n")
   
   for i = 1, game.nplayers, 1 do
      local file = string.format("full-%d.svg", i) 
      local singles = output:svg(file, i)
      
      local m = {}
      
      local w = singles.bounds.right-singles.bounds.left
      local h = singles.bounds.bottom-singles.bounds.top
      
      m.w = w
      m.h = h
      m.hexes = {}
      
      f:write(string.format("<img src=\"%s\" usemap=\"#grid\" width=\"%fpx\" height=\"%fpx\"/>\n", file, w, h))
      f:write("<map name=\"grid\">\n")
      for j = 0, singles:size()-1, 1
      do
	 local h = {
	    path = {}
	 }
	 local s = singles.singles[j]
	 f:write(string.format("<area id=\"%s\" href=\"javascript:map('%s');\" shape=\"poly\" coords=\"", s.id, s.id))
	 for k = 0, s.path:size()-1, 1 do
	    local p = {
	       x = s.path[k].x,
	       y = s.path[k].y
	    }
	    h.path[k+1] = p;
	    f:write(string.format("%05.2f,%05.2f,", s.path[k].x,s.path[k].y))
	 end
	 m.hexes[s.id] = h
	 f:write("\"/>\n")
      end
      f:write("</map>\n")
      f:write("<script language=\"JavaScript\"> var swamp=", json.encode(m), ";</script>\n")
   end
   f:write("</body></html>\n")
end

