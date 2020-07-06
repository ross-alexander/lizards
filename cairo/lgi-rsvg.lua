-- ----------------------------------------------------------------------
--
-- lgi and Rsvg test
--
-- 2020-07-04: Ross Alexander
--   Update to version Gdk to 3.0 as Gdk 4.0 doesn't work with LGI currently
--
-- ----------------------------------------------------------------------

local lgi = require 'lgi'

local GdkPixbuf = lgi.require('GdkPixbuf')
local cairo = lgi.require('cairo')
local Gdk = lgi.require('Gdk', '3.0')
local Rsvg = lgi.require('Rsvg')

local assert = lgi.assert

local svg = Rsvg.Handle.new_from_file('svg/lizard.svg')

local background = assert(GdkPixbuf.Pixbuf.new_from_file('/home/ralexand/src/tile/images/013167-FC50.jpg'))
local back_width, back_height = background.width, background.height
print(background)

local surface = cairo.SvgSurface.create('gdktest.svg', back_width, back_height)

local cr = cairo.Context.create(surface)
cr:set_source_pixbuf(background, 0, 0)
cr:paint()

svg:render_cairo(cr)
