local lizards = require "lizards"

-- ----------------------------------------------------------------------
--
-- use the lgi library to integrate with cairo and GTK+
--
-- ----------------------------------------------------------------------

local lgi = require 'lgi'
local cairo = lgi.cairo
local Rsvg = lgi.require('Rsvg')

math.randomseed(os.time())

game = lizards.game_t(lizards.FORMAT_FILE_JS, "game.js")

-- --------------------
-- Use C++ to create SVG files
-- --------------------

for i = 0, game.nplayers, 1 do
   local output = lizards.output_t(game, i)
   file = string.format("p%02d.svg", i)
   output:svg(file)
end

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

function svg(game, player)
   local grid = game.grid
   local hexes = grid:owned(player, lizards.OWNED_BAND_ONLY)

   -- distance function for PEAK
   
   function distance(hex)
      if hex:isa(lizards.PEAK) then return 3 else return 2 end
   end

   -- Find all visible hexes

   local seen = {}
   for i = 0, hexes:size()-1, 1 do
      local hex = hexes[i]
      mark(grid, hexes[i], distance(hexes[i]), seen)
   end

   -- Print visible hexes

   for k,v in spairs(seen) do
      print(k, v:tostring())
   end

   -- Find center hex
   
   local hex_min_weight = nil
   local min_weight = grid.width^2 * grid.height^2
   for ak, av in pairs(seen) do
      local total = 0
      for bk, bv in pairs(seen) do
	 local dx = math.abs(bv.xy.x - av.xy.x)
	 local dy = math.abs(bv.xy.y - av.xy.y)

	 total = total + math.min(dx, grid.width - dx)
	 total = total + math.min(dy, grid.width - dy)
      end
      if (total < min_weight) then
	 hex_min_weight = av
	 min_weight = total
      end
   end
   print("minimum", hex_min_weight, min_weight)   

   local center = hex_min_weight.xy
   center.x = center.x - center.x%2;

   local middle = lizards.point_t(grid.width / 2, grid.height / 2);
   local diff = middle - center;

--   print(middle)
--   print(center)
--   print(diff)

   local file = string.format("newgame-p%02d.svg", player)

   local places = {}
   local bounds = lizards.fbox_t()

   -- --------------------
   -- place hexes and expand edge
   -- --------------------
   
   for ak, av in pairs(seen) do
      local p = av.xy + diff
      p:clip(grid.width, grid.height)
      local place = lizards.fplace_t(p, 40, 5)
      bounds:setunion(place.bounds)
      local t = { place = place, hex = av }

      if (av.owner > 0) then
	 t.owner = game:player(av.owner).code
      end

      if (svgmap[av:isa()]) then
	 t.symbol = svgmap[av:isa()]
      end

      -- --------------------
      -- Are we are den
      -- --------------------
      
      if (av:isa(lizards.DEN)) then
	 if (av.owner == 0) then
	    t.header = "Free"
	 elseif (av.owner == player) then
	    t.header = t.owner
	    local band = av:get(lizards.BAND)
	    if (av.home) then
	       t.symbol = svgmap[lizards.HOME_DEN];
	    end
	    if (band ~= nil) then
	       t.lizards = band:size() + av.pop
	    else
	       t.lizards = av.pop
	    end
	 end

	 -- --------------------
	 -- Are we a band
	 -- --------------------
	 
      elseif (av:get(lizards.BAND)) then
	 local band = av:get(lizards.BAND)
	 if ((av.owner == player) or not(av:isa(lizards.SCRUB) or av:isa(lizards.RUIN))) then
	    t.lizards = band:size()
	    t.header = t.owner
	 end
      end
      table.insert(places, t)
   end
   
   local canvas = cairo.SvgSurface.create(file, math.floor(bounds.right - bounds.left), math.floor(bounds.bottom - bounds.top))
   local ctx = cairo.Context.create(canvas)

   ctx:select_font_face("Bitstream Vera Sans", cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_NORMAL);
--   ctx:select_font_face("Courier 10 Pitch", cairo.FONT_SLANT_NORMAL, cairo.FONT_WEIGHT_BOLD);
   ctx:set_font_size(10);

   local font_options = cairo.FontOptions.create()
   
   font_options.set_hint_style(cairo.HINT_STYLE_NONE);
   font_options.set_hint_metrics(cairo.HINT_METRICS_OFF);
   font_options.set_antialias(cairo.ANTIALIAS_GRAY);
   ctx:set_font_options(font_options);

   ctx:save()
--   ctx:set_source_rgb(0.9, 0.9, 1.0)
--   ctx:rectangle(0, 0, bounds.right - bounds.left, bounds.bottom - bounds.top)
--   ctx:fill()

   ctx:translate(-bounds.left, -bounds.top)

   ctx:set_line_width(0.2)
   ctx:set_source_rgb(0.0, 0.0, 0.0)
   
   for key, value in ipairs(places) do
      local place = value.place
      ctx:move_to(place.hull[0].x, place.hull[0].y)
      for i = 1, 5, 1 do
	 ctx:line_to(place.hull[i].x, place.hull[i].y)
      end
      ctx:close_path()
      if (value.hex:isa(lizards.WATER)) then
	 ctx:set_source_rgb(0.80, 0.94, 0.99);
      else
	 ctx:set_source_rgb(0.80, 0.99, 0.80);
      end
      ctx:fill_preserve()
      ctx:set_source_rgb(0.0, 0.0, 0.0)
      ctx:stroke()

      local cx = place.center.x;
      local cy = place.center.y;
      
      ctx:save();
      ctx:translate(cx - 20.0, cy - 15.0);
      ctx:scale(0.20, 0.20);
      if (value.symbol ~= nil) then
	 value.symbol:render_cairo(ctx)
      end
      ctx:restore();

      -- --------------------
      -- trailer
      -- --------------------
      
      local trailer = value.hex:getid()
      local e = ctx:text_extents(trailer)
      ctx:move_to(cx -e.width/2, cy + math.sqrt(3)/2*40.0 - 2);
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
	 ctx:move_to(cx - e.width/2, cy - math.sqrt(3)/2*40.0 + e.height + 2);
	 ctx:show_text(header);
	 ctx:restore();
      end
      
      -- --------------------
      -- lizards
      -- --------------------
      
      if (value.lizards ~= nil and value.lizards > 0) then
	 ctx:save();
	 ctx:translate(cx, cy - 25);
	 ctx:scale(0.15, 0.15);
	 svgmap[lizards.BAND]:render_cairo(ctx)
	 ctx:restore();
	 
	 local size = string.format("%d", value.lizards)
	 local e = ctx:text_extents(size);
	 ctx:move_to(cx - e.width, cy - 5);
	 ctx:show_text(size);	 
      end
   end
   ctx:restore()
   
   print("------------------------------")
end

local svg = Rsvg.Handle.new_from_file('svg/lizard.svg')
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

for p = 1, game.nplayers, 1 do
   svg(game, p)
end
