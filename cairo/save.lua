local l = require "lizards"

local layouts = l.layouts_t(lizards.FORMAT_FILE_JS, "LAYOUT.JS")
local template = l.template_t(layouts, l.FORMAT_FILE_JS, "save-in.js")
template:save("save-out.js")
