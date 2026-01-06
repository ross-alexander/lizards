-- ----------------------------------------------------------------------
--
-- use the lgi/LuaGObject library to integrate with cairo and GTK+
--
-- ----------------------------------------------------------------------

local lizards = require "lizards"

local lgi = require 'LuaGObject'
local cairo = lgi.cairo
local Rsvg = lgi.require('Rsvg')

-- ----------------------------------------------------------------------
--
-- spairs
--
-- Sort table by either keys or order function
--
-- ----------------------------------------------------------------------

function spairs(t, order)
    -- collect the keys
    local keys = {}
    for k in pairs(t) do keys[#keys+1] = k end

    -- if order function given, sort by it by passing the table and keys a, b,
    -- otherwise just sort the keys
    
    if order then
        table.sort(keys, function(a,b) return order(t, a, b) end)
    else
        table.sort(keys)
    end

    -- return the iterator function
    local i = 0
    return function()
        i = i + 1
        if keys[i] then
            return keys[i], t[keys[i]]
        end
    end
end

-- ----------------------------------------------------------------------
--
-- mark
--
-- ----------------------------------------------------------------------

function mark(grid, hex, depth, res)
   res[hex:getid()] = hex;
   if (depth > 0) then
      local pt = hex.xy;
      for dir = 0, lizards.DIRECTIONS - 1, 1 do
	 local npt = grid:move(pt, dir)
	 mark(grid, grid(npt), depth - 1, res)
      end
   end
end


-- ----------------------------------------------------------------------
--
-- svg
--
-- ----------------------------------------------------------------------

function svg(params, svgmap, game, player)
   local grid = game.grid
   local hexes = grid:owned(player, lizards.OWNED_BAND_ONLY)

   -- distance function for PEAK
   
   function distance(hex)
      if hex:isa(lizards.PEAK) then return 3 else return 2 end
   end

   -- Find all visible hexes

   local seen = {}
   for i = 0, hexes:size()-1, 1 do
      mark(grid, hexes[i], distance(hexes[i]), seen)
   end

   -- Print visible hexes

   for k,v in spairs(seen) do
      print(k, v:tostring())
   end

   -- Find center hex
   
   local hex_min_weight = nil
   local min_weight = grid.width^2 * grid.height^2
   for _, a in pairs(seen) do
      local total = 0
      for _, b in pairs(seen) do
	 local dx = math.abs(b.xy.x - a.xy.x)
	 local dy = math.abs(b.xy.y - a.xy.y)

	 total = total + math.min(dx, grid.width - dx)
	 total = total + math.min(dy, grid.width - dy)
      end
      if (total < min_weight) then
	 hex_min_weight = a
	 min_weight = total
      end
   end
   print("minimum", hex_min_weight:getid(), min_weight)   

-- Shift center x if odd
   
   local center = hex_min_weight.xy
   center.x = center.x - center.x%2;

   local middle = lizards.point_t(grid.width / 2, grid.height / 2);
   local diff = middle - center;

--   print(middle)
--   print(center)
--   print(diff)

   local file = string.format("newgame-p%02d-lua.svg", player)

   local places = {}
   local bounds = lizards.fbox_t()

   local radius = params.radius
   local margin = params.margin
   
   -- --------------------
   -- place hexes and expand edge into places{}
   -- --------------------
   
   for _, hex in pairs(seen) do
      local p = hex.xy + diff

      -- Ensure point isn't off the grid
      
      p:clip(grid.width, grid.height)

      -- Use existing C++ function to create bounding box and hex frame
      
      local place = lizards.fplace_t(p, radius, margin)

      -- Expand bounding box
      
      bounds:setunion(place.bounds)
      local t = { place = place, hex = hex }

      if (hex.owner > 0) then
	 t.owner = game:player(hex.owner).code
      end

      if (svgmap[hex:isa()]) then
	 t.symbol = svgmap[hex:isa()]
      end

      -- --------------------
      -- Are we are den
      -- --------------------
      
      if (hex:isa(lizards.DEN)) then
	 if (hex.owner == 0) then
	    t.header = "Free"
	 elseif (hex.owner == player) then
	    t.header = t.owner
	    local band = hex:get(lizards.BAND)
	    if (hex.home) then
	       t.symbol = svgmap[lizards.HOME_DEN]
	       t.center = "HOME DEN"
	    end
	    if (band ~= nil) then
	       t.lizards = band:size() + hex.pop
	    else
	       t.lizards = hex.pop
	    end
	 end

	 -- --------------------
	 -- Are we a band
	 -- --------------------
	 
      elseif (hex:get(lizards.BAND)) then
	 local band = hex:get(lizards.BAND)
	 if ((hex.owner == player) or not(hex:isa(lizards.SCRUB) or hex:isa(lizards.RUIN))) then
	    t.lizards = band:size()
	    t.header = t.owner
	 end
      end
      table.insert(places, t)
   end

   -- Create SVG surface and Cairo context
   
   local canvas = cairo.SvgSurface.create(file, math.floor(bounds.right - bounds.left), math.floor(bounds.bottom - bounds.top))
   local ctx = cairo.Context.create(canvas)

   -- Set font
   
   ctx:select_font_face(params.font, cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_NORMAL);
   ctx:set_font_size(params.fontsize);

   local font_options = cairo.FontOptions.create()
   font_options.set_hint_style(cairo.HINT_STYLE_NONE);
   font_options.set_hint_metrics(cairo.HINT_METRICS_OFF);
   font_options.set_antialias(cairo.ANTIALIAS_GRAY);
   ctx:set_font_options(font_options);

   ctx:set_line_width(params.border)

   -- Save context then translating bounds
   
   ctx:save()
   ctx:translate(-bounds.left, -bounds.top)
   
   for _, value in ipairs(places) do
      local place = value.place

      -- Create hex frame path
      
      ctx:move_to(place.hull[0].x, place.hull[0].y)
      for i = 1, 5, 1 do
	 ctx:line_to(place.hull[i].x, place.hull[i].y)
      end
      ctx:close_path()

      -- The original lizards is black and white so colour is new
      -- Currently only two colours are used, water and land

      local color
      if (value.hex:isa(lizards.WATER)) then
	 color = params.colors.water
      else
	 color = params.colors.land
      end
      ctx:set_source_rgb(color.red, color.green, color.blue)
      ctx:fill_preserve()
      color = params.colors.border
      ctx:set_source_rgb(color.red, color.green, color.blue)
      ctx:stroke()

      local cx = place.center.x;
      local cy = place.center.y;
      
      -- --------------------
      -- symbol, using render_document
      -- --------------------
      
      if (value.symbol ~= nil) then
	 local viewport = Rsvg.Rectangle()
	 viewport.x = cx - 20
	 viewport.y = cy - 15.0
	 viewport.width = 40.0
	 viewport.height = 40.0
	 value.symbol:render_document(ctx,viewport)
      end

      -- --------------------
      -- trailer
      -- --------------------
      
      local trailer = value.hex:getid()
      local e = ctx:text_extents(trailer)
      ctx:move_to(cx -e.width/2, cy + math.sqrt(3)/2 * radius - 2);
      ctx:set_source_rgb(0.0, 0.0, 0.0);
      ctx:show_text(trailer);

      -- --------------------
      -- header
      -- --------------------

      if (value.header) then
	 ctx:save();
	 ctx:set_source_rgb(1.0, 0.0, 0.0);
	 local header = value.header
	 local e = ctx:text_extents(header);
	 ctx:move_to(cx - e.width/2, cy - math.sqrt(3)/2 * radius + e.height + 2);
	 ctx:show_text(header);
	 ctx:restore();
      end
      
      -- --------------------
      -- lizards
      -- --------------------
      
      if (value.lizards ~= nil and value.lizards > 0) then
	 local viewport = Rsvg.Rectangle()
	 viewport.x = cx
	 viewport.y = cy - 25.0
	 viewport.width = 30.0
	 viewport.height = 30.0
	 svgmap[lizards.BAND]:render_document(ctx, viewport)
	 
	 local size = string.format("%d", value.lizards)
	 local e = ctx:text_extents(size);
	 ctx:move_to(cx - e.width, cy - 5);
	 ctx:show_text(size);	 
      end

      -- --------------------
      -- center
      -- --------------------
      
      if value.center ~= nil then
	 ctx:save()
	 ctx:translate(cx, cy)
	 local e = ctx:text_extents(value.center)
	 ctx:rel_move_to(- e.width / 2, 15)
	 ctx:show_text(value.center)
	 ctx:restore()
      end
   end
   ctx:restore()
   
   print("------------------------------")
end

-- ----------------------------------------------------------------------
--
-- M A I N
--
-- ----------------------------------------------------------------------

math.randomseed(os.time())

game = lizards.game_t(lizards.FORMAT_FILE_JS, "grid.js")

-- --------------------
-- Use C++ to create SVG files
-- --------------------

for i = 0, game.nplayers, 1 do
   local output = lizards.output_t(game, i)
   file = string.format("newgame-p%02d-cpp.svg", i)
   output:svg(file)
end


-- local svg = Rsvg.Handle.new_from_file('svg/lizard.svg')

local svgmap = {
   [lizards.SCRUB] = Rsvg.Handle.new_from_file('svg/scrub.svg'),
   [lizards.PLAINS] = Rsvg.Handle.new_from_file('svg/plains.svg'),
   [lizards.FERTILE] = Rsvg.Handle.new_from_file('svg/fertile.svg'),
   [lizards.SCRUB] = Rsvg.Handle.new_from_file('svg/scrub.svg'),
   [lizards.TEMPLE] = Rsvg.Handle.new_from_file('svg/temple.svg'),
   [lizards.RUIN] = Rsvg.Handle.new_from_file('svg/ruin.svg'),
   [lizards.SWAMP] = Rsvg.Handle.new_from_file('svg/swamp.svg'),
   [lizards.RED_DEN] = Rsvg.Handle.new_from_file('svg/den-red.svg'),
   [lizards.BLACK_DEN] = Rsvg.Handle.new_from_file('svg/den-black.svg'),
   [lizards.GREEN_DEN] = Rsvg.Handle.new_from_file('svg/den-green.svg'),
   [lizards.YELLOW_DEN] = Rsvg.Handle.new_from_file('svg/den-yellow.svg'),
   [lizards.GREY_DEN] = Rsvg.Handle.new_from_file('svg/den-grey.svg'),
   [lizards.HOME_DEN] = Rsvg.Handle.new_from_file('svg/den-home.svg'),
   [lizards.BAND] = Rsvg.Handle.new_from_file('svg/lizard.svg'),
}

print(game:tostring())
local grid = game.grid
print(grid:tostring())

local params = {
   font = "Adwaita Sans",
   fontsize = 10.0,
   radius = 40.0,
   margin = 5.0,
   border = 0.2,
   colors = {
      border = { red = 0.0, green = 0.0, blue = 0.0 },
      water = { red = 0.80, green = 0.94, blue = 0.99 },
      land = { red = 0.80, green = 0.99, blue = 0.80 },
      blank = { red = 0.80, green = 0.80, blue = 0.80 },
   },
}
for p = 1, game.nplayers, 1 do
   svg(params, svgmap, game, p)
end
