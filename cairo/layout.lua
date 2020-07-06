require "lizards"

local rex = require "rex_pcre"
local json = require "json"

if (#arg > 0) then
   local args = {}
   rx = rex.new('^--([A-Za-z0-9]+)=(.*)$')
   for i = 1, #arg,1
   do
      local match, endmatch, subs = rx:tfind(arg[i])
      if (match) then
	 args[subs[1]] = subs[2]
      end
   end
   if (not args.input) then
      print("Requires --input=<file>")
      return
   end
   local layouts = lizards.layouts_t(lizards.FORMAT_FILE_JS, args.input)
   if (not args.layout) then
      print("Requires --layout=<1 .. " .. layouts:size() .. ">")
      return
   end
   local layout = layouts:layout(args.layout)
   if (not args.output) then
      print("Requires --output=<file>")
      return
   end
   local grid = layout:realize()
   local output = grid:output()
   output:svg(args.output, 0)
else

-- Read in layout descriptions from text file

   local layouts = lizards.layouts_t(lizards.FORMAT_FILE_JS, "LAYOUT.JS")

   local id = "1"
   local layout = layouts:layout(id)

   print(layouts, layout)
   
   -- The realize function creates a grid_t object with 
   
   grid = layout:realize()
   
   -- Output to SVG
   
   lizards.output_t(grid, 0):svg(string.format("layout-%s.svg", id))
   lizards.output_t(grid, 0):json(string.format("layout-%s.js", id))
end

