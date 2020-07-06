w = World.new("/home/ralexand/source/lizards/cxx/Test.xml")

p = w:players()

for x = 0,w:map():width()-1 do
   for y = 0,w:map():height()-1 do
      local h = w:map():hex(x,y)
      print(x, y, h, p[h:owner()])
   end
end

i = getmetatable(w).__index;
i["a"] = 10
print(getmetatable(w).__index)
print(w.a)
for n,v in pairs(getmetatable(w).__index) do print(n,v) end

map = w:map()
print(map)
print(map:width())
h = map:hex(1, 2)
print(h)
-- for n,v in pairs(map:hexes()) do print(n,v) end

for n,v in pairs(w:players()) do print(v) end