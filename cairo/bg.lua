function svg(game)
   local out = game.grid:output()
   local singles = out:single('single.txt', 1)

   print(singles)
-- out.scalef = 0.6
-- out.border = 5.0
-- out:svg("bg.svg", 0)
end

local l = require "lizards"

local game = l.game_t(l.FORMAT_FILE_JS, "game.js")
game:report()
