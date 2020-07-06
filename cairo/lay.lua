require "lizards"

layouts = lizards.layouts_t("LAYOUT.TXT")
template = lizards.template_t(layouts, "tmp/cust.map")

grid = template:realize()
lizards.output_t():svg(grid, "tmp/cust.svg", 0)

stream = io.open("tmp/cust.htm", "w")
if (stream) then
   stream:write("<ul>")
   stream:write("<li>Cost of template is ",template.cost, "</li>\n")
   stream:write("</ul>\n")
end

for x = 0, grid.width-1, 1 do
   for y = 0, grid.height-1, 1 do
      local hex = grid(lizards.point_t(x, y))
      if (not (hex == nil)) then
	 if (hex:get(lizards.BAND)) then
	    print(hex:id())
	 end
      end
   end
end

