require "lizards"

serial = lizards.serial_t()
grid = serial:from_file("grid.xml")

for i = 0, game.nplayers, 1 do
   file = string.format("p%02d.svg", i)
   print(file)
   lizards.output_t():svg(grid, file, 40, i)
end
