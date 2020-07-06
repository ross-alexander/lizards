require "lizards"

local rex = require "rex_pcre"

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
   if (not args.layouts) then
      print("Requires --layouts=<file>")
      return
   end

   local layouts = lizards.layouts_t(lizards.FORMAT_FILE_JS, args.layouts)

   if (layouts.layouts:size() == 0) then
      print("Failed to load layout file");
      return
   end
   
   if (not args.input) then
      print("Requires --input=<file>")
      return
   end

   local template = lizards.template_t(layouts, lizards.FORMAT_FILE_JS, args.input)

   local grid = template:realize()
   local output = grid:output(0)

   if (args.hit) then
      local ha = {}
      for w in string.gmatch(args.hit, "[^,]+") do
	 table.insert(ha, w)
      end
      local hit = output:hit(ha[1], ha[2], ha[3], ha[4])
      if (not hit == nil)
      then
	 print(hit)
      end
   end

   if (args.output) then
      template:save(args.output)
   end

   if (args.js) then
      output:json(args.js)
   end

   if (args.svg) then
      output:svg(args.svg)
   end
end
