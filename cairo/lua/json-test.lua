local json = require("JSON")

local filename = "/locker/work/201602-csrx/arch/20160429/pwg/conf-20160429-wgc.js"

local f = assert(io.open(filename, "r"))
local t = f:read("*all")
f:close()

local foo = json:decode([[
            {"name" : "lol", "age" : -1.5e6, "foo" : ["bar", true, null]}
            ]])
print(foo.age)    -- -1500000
print(foo.name)   -- lol
print(foo.foo[1]) -- bar
print(foo.foo[2]) -- true
print(foo.foo[3]) -- null
print(foo.foo[3] == json.null) -- true
foo.foo = "omg :D"
print(json:encode(foo)) -- {"name":"lol","age":-1500000,"foo":"omg :D"}

local srx = json:decode(t)
