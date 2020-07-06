local lizards = require("lizards")

local layouts = lizards.layouts_t(lizards.FORMAT_FILE_JS, "../LAYOUT.JS");

local template = lizards.template_t(layouts, lizards.FORMAT_FILE_JS, "../CUST2.JS");

local grid = template:realize();
local output = grid:output();
output:svg('layout.svg', 0);

-- $template->setactive('04');
-- $template->save('test.js');
