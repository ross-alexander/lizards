-- ----------------------------------------------------------------------
--
-- 2026-01-01: Create SVG of layouts and standard templates
--
-- ----------------------------------------------------------------------

local lizards = require("lizards")

local layouts = lizards.layouts_t(lizards.FORMAT_FILE_JS, "../LAYOUT.JS");

local names = layouts:keys()

for i = 0, names:size()-1, 1 do
   local layout = layouts:layout(names[i])
   local grid = layout:realize()
   local svg = "layout-"..names[i]..".svg"
   local output = grid:output(0)
   output:svg(svg)
end

templates = {"CUST1", "CUST2", "CUST3", "CUST4"}

for _,t in ipairs(templates)
do
   local json = "../"..t..".JS"
   local svg = t..".svg"
   
   local template = lizards.template_t(layouts, lizards.FORMAT_FILE_JS, json);
   local grid = template:realize();
   local output = grid:output(0);
   output:svg(svg);
end
