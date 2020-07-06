local json = require("json")

local t = {
   { x = 10, y = 10 },
   { x = 20, y = 20 },
};

print(json.encode(t))
