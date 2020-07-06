require "lizards"

-- Read in layout descriptions from text file

layouts = lizards.layouts_t("LAYOUT.JS", lizards.FORMAT_JS)

template = lizards.template_t(layouts, "partial.js", lizards.FORMAT_JS)

grid = template:realize()
lizards.output_t():svg(grid, "partial.svg", 0)
-- lizards.output_t():json(grid, "CUST1.js", 0)
