-- ----------------------------------------------------------------------
--
-- bg.lua
--
-- 2022-12-11
--
-- Create SVG output from JSON file
--
-- ----------------------------------------------------------------------

function vary_density(l)
   for density=20,80,10 do
      local g = l.grid_t(32, 16)
      g:lay_islands(density / 100.0)
      local out = g:output(0)
      out.scalef = 1.2
      out.border = 5.0
      out:svg(string.format("bg-%02d.svg", density))
   end
end

function svg(game)
   local output = game.grid:output(0)
--   local singles = out:single('single.txt', 1)
--   print(singles)
--   output.scalef = 1.0
--   output.border = 5.0
   --   output.border_width = 1.0
   
   output.dimensions.border = 0.2
   output.dimensions.margin = 20
   output.dimensions.scale = 1.0
   output:set_colour("water", 0.5, 0.5, 1.0)
   output:set_colour("border", 1.0, 0.0, 0.0)
   output:svg("bg.svg")
end

-- ----------------------------------------------------------------------
--
-- M A I N
--
-- ----------------------------------------------------------------------

local lizards = require "lizards"

-- vary_density(l)

local game = lizards.game_t(lizards.FORMAT_FILE_JS, "grid.js")
svg(game)

-- game:report()

