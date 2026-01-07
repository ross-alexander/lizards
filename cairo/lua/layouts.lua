-- ----------------------------------------------------------------------
--
-- 2026-01-06: Add custom layouts
--
-- 2026-01-01: Create SVG of layouts and standard templates
--
-- ----------------------------------------------------------------------

local lizards = require("lizards")

function layouts_svg(l)
   local names = l:keys()
   for i = 0, names:size()-1, 1 do
      local layout = l:layout(names[i])
      local grid = layout:realize()
      local svg = "layout-"..names[i]..".svg"
      local output = grid:output(0)
      output:set_dimension("border", 0.8)
      output:set_dimension("fontsize", 8.0)
      output:set_colour("border", 1.0, 0.0, 0.0)
      output:set_colour("blank", 0.9, 0.6, 0.6)
      output:set_param("font", "Linux BioLinum O")
      output:svg(svg)
   end
end

lizards.log_level = 7

local layouts_custom = lizards.layouts_t(lizards.FORMAT_FILE_JS, "custom_layouts.js")
layouts_svg(layouts_custom)

local layouts_std = lizards.layouts_t(lizards.FORMAT_FILE_JS, "../LAYOUT.JS");
layouts_svg(layouts_std)

local templates = {"CUST1", "CUST2", "CUST3", "CUST4"}

for _,t in ipairs(templates)
do
   local json = "../"..t..".JS"
   local svg = t..".svg"
   
   local template = lizards.template_t(layouts_std, lizards.FORMAT_FILE_JS, json);
   local grid = template:realize();
   local output = grid:output(0);
   output:svg(svg);
end
