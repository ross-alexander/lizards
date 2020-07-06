-- for n,v in pairs(World) do print(n,v) end

function fish()
  w = World.new("/home/ralexand/source/lizards/cxx/Test.xml")
  players = w:players()
  for n,v in pairs(players) do print(v:num()) end
  print(w:game())
  local m = w:map()
--  print(m:width())
--  print(m:height())
  local i
  local j
  for i = 0, m:width()-1 
  do
    for j = 0, m:height()-1
    do
      local h = m:hex(i, j)
      print(h)
      local f = h:features()
        for n,v in pairs(f) do print(n,v) end
    end
  end
end

fish()
