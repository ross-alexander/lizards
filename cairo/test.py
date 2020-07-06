#!/usr/bin/env python

import random
import lizards


def bands(p):
  print "Player ", p
  for i in range(0, grid.width):
    for j in range(0, grid.height):
      hex = grid(lizards.point_t(i, j))
      if (hex.owner == p):
        band = hex.get(lizards.BAND)
        if (band):
          print "Band at", hex.id(), "with", band.size(), "lizards"

def seen_r(grid, hex, d, seen_l, seen_h):
  if (hex.id() not in seen_h):
    seen_h[hex.id()] = 1
    seen_l.append(hex)
    if (d == 0):
      return
    for i in range(0, 6):
      adj = grid(grid.move(hex.xy, i))
      seen_r(grid, adj, d-1, seen_l, seen_h)

def seen(grid, p):
  seen_h = {}
  seen_l = []
  band_l = []
  for i in range(0, grid.width):
    for j in range(0, grid.height):
      hex = grid(lizards.point_t(i, j))
      if (hex.owner == p):
        band = hex.get(lizards.BAND)
        if (band):
          band_l.append(hex)
  print "Seen ", len(band_l), " bands"

  for h in band_l:
    if (h.is_type(lizards.PEAK) | h.is_type(lizards.VOLCANO)):
      d = 2
    else:
      d = 1
    seen_r(grid, h, d, seen_l, seen_h)
  return seen_l

# ----------------------------------------------------------------------

def seen_mark_r(grid, p, h, d):
  mark = h.mark(p)
  if (d == 0):
    return
  for i in range(0, 6):
    adj = grid(grid.move(h.xy, i))
    seen_mark_r(grid, p, adj, d-1)
  
def seen_mark(grid, p):
  bands = []
  for i in range(0, grid.width):
    for j in range(0, grid.height):
      hex = grid(lizards.point_t(i,j))
      if (hex.owner == p):
        band = hex.get(lizards.BAND)
        if (band):
          bands.append(hex)
          print("Band at ", hex.id())
      hex.clear(p)
  for h in bands:
    if (h.is_type(lizards.PEAK) | h.is_type(lizards.VOLCANO)):
      seen_mark_r(grid, p, h, 2)
    else:
      seen_mark_r(grid, p, h, 1)
  seen_l = []
  for i in range(0, grid.width):
    for j in range(0, grid.height):
      hex = grid(lizards.point_t(i,j))
      if (hex.check(p)):
        seen_l.append(hex)


# ----------------------------------------------------------------------

grid = lizards.grid_t(16, 16)
grid.lay_islands()
layouts = lizards.layouts_t("LAYOUT.TXT")


game = lizards.game_t("game.xml")
places = []
for i in range(0, grid.width/8):
  for j in range(0, grid.height/8):
    places.append({'x':i*8+4, 'y':j*8+4})

random.seed()
for i in range(0, grid.width * grid.height):
  a = random.shuffle(places)

for i in range(1, game.nplayers+1):
  template = lizards.template_t(layouts, game.player(i).startup)
  grid.place_template(template, i, lizards.point_t(places[i-1]['x'], places[i-1]['y']))

for p in range(1, game.nplayers+1):
  print "----------------------------------------"
  file = "p%02d.svg" % p
  lizards.output_t().svg(grid, file, 40, p)
