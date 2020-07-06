LuaJSON_lib

Version 1.2, 8 July 2010
by Nathaniel Musgrove (proton.zero@gmail.com)

   LuaJSON_lib provides two functions, json.decode() and json.encode(), and one
   constant, json.null.

   json.decode() parses any single JSON value (object, array, string, number,
   true, false, null) into an appropriate Lua data structure.

      Objects are parsed as tables with string-keys.  Arrays are parsed
      as tables with number-keys.  Strings are parsed as strings,
      numbers as numbers, true as true, false as false, and null is
      parsed as json.null, a constant used solely for the purpose of
      representing JSON's null value.

   json.encode() takes a single Lua value (which may be complex, such as nested
   tables) and outputs a JSON-formatted string representing that value.

      Tables are stringified as arrays if table[1] ~= nil, and
      stringified as objects otherwise (only table elements with keys
      that are strings will be put into the JSON object).  Strings are
      stringified as strings, escaping backslashes, double-quotation
      marks, form feeds, line feeds, carriage returns, tabs, and
      backspace-characters, and unicode-escaping any other character
      less than 0x20.  Numbers are stringified as numbers, booleans as
      booleans, and json.null as null.  Anything other than tables,
      strings, numbers, booleans, and json.null will produce an invalid
      JSON string (except nil, which produces an empty string).

      NOTE: Infinity and not-a-number values are encoded as null.

   At the time of writing, Lua 5.1 has a bug which can cause Lua to crash if
   a particularly long encode() result (15 to 20 kilobytes) is passed directly
   to a function.  In order to bypass this bug, the author recommends storing
   encode() results to a variable, then passing that variable to further
   functions.


Example

-- import the LuaJSON module
local json = require("LuaJSON") -- json is the default table name.
local foo = json.decode([[
            {"name" : "lol", "age" : -1.5e6, "foo" : ["bar", true, null]}
            ]])
print(foo.age)    -- -1500000
print(foo.name)   -- lol
print(foo.foo[1]) -- bar
print(foo.foo[2]) -- true
print(foo.foo[3]) -- null
print(foo.foo[3] == json.null) -- true
foo.foo = "omg :D"
print(json.stringify(foo)) -- {"name":"lol","age":-1500000,"foo":"omg :D"}
