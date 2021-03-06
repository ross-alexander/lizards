# This file was automatically generated by SWIG (http://www.swig.org).
# Version 2.0.8
#
# Do not make changes to this file unless you know what you are doing--modify
# the SWIG interface file instead.



from sys import version_info
if version_info >= (2,6,0):
    def swig_import_helper():
        from os.path import dirname
        import imp
        fp = None
        try:
            fp, pathname, description = imp.find_module('_lizards', [dirname(__file__)])
        except ImportError:
            import _lizards
            return _lizards
        if fp is not None:
            try:
                _mod = imp.load_module('_lizards', fp, pathname, description)
            finally:
                fp.close()
            return _mod
    _lizards = swig_import_helper()
    del swig_import_helper
else:
    import _lizards
del version_info
try:
    _swig_property = property
except NameError:
    pass # Python < 2.2 doesn't have 'property'.
def _swig_setattr_nondynamic(self,class_type,name,value,static=1):
    if (name == "thisown"): return self.this.own(value)
    if (name == "this"):
        if type(value).__name__ == 'SwigPyObject':
            self.__dict__[name] = value
            return
    method = class_type.__swig_setmethods__.get(name,None)
    if method: return method(self,value)
    if (not static):
        self.__dict__[name] = value
    else:
        raise AttributeError("You cannot add attributes to %s" % self)

def _swig_setattr(self,class_type,name,value):
    return _swig_setattr_nondynamic(self,class_type,name,value,0)

def _swig_getattr(self,class_type,name):
    if (name == "thisown"): return self.this.own()
    method = class_type.__swig_getmethods__.get(name,None)
    if method: return method(self)
    raise AttributeError(name)

def _swig_repr(self):
    try: strthis = "proxy of " + self.this.__repr__()
    except: strthis = ""
    return "<%s.%s; %s >" % (self.__class__.__module__, self.__class__.__name__, strthis,)

try:
    _object = object
    _newclass = 1
except AttributeError:
    class _object : pass
    _newclass = 0


DEN = _lizards.DEN
BAND = _lizards.BAND
PEAK = _lizards.PEAK
VOLCANO = _lizards.VOLCANO
RED = _lizards.RED
GREEN = _lizards.GREEN
GREY = _lizards.GREY
BLACK = _lizards.BLACK
YELLOW = _lizards.YELLOW
UNSETTLED = _lizards.UNSETTLED
FORMAT_FILE_TXT = _lizards.FORMAT_FILE_TXT
FORMAT_FILE_XML = _lizards.FORMAT_FILE_XML
FORMAT_FILE_JS = _lizards.FORMAT_FILE_JS
FORMAT_STRING_TXT = _lizards.FORMAT_STRING_TXT
FORMAT_STRING_XML = _lizards.FORMAT_STRING_XML
FORMAT_STRING_JS = _lizards.FORMAT_STRING_JS
class point_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, point_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, point_t, name)
    __repr__ = _swig_repr
    __swig_setmethods__["x"] = _lizards.point_t_x_set
    __swig_getmethods__["x"] = _lizards.point_t_x_get
    if _newclass:x = _swig_property(_lizards.point_t_x_get, _lizards.point_t_x_set)
    __swig_setmethods__["y"] = _lizards.point_t_y_set
    __swig_getmethods__["y"] = _lizards.point_t_y_get
    if _newclass:y = _swig_property(_lizards.point_t_y_get, _lizards.point_t_y_set)
    def __init__(self, *args): 
        this = _lizards.new_point_t(*args)
        try: self.this.append(this)
        except: self.this = this
    def __eq__(self, *args): return _lizards.point_t___eq__(self, *args)
    def __ne__(self, *args): return _lizards.point_t___ne__(self, *args)
    def __iadd__(self, *args): return _lizards.point_t___iadd__(self, *args)
    def __add__(self, *args): return _lizards.point_t___add__(self, *args)
    __swig_destroy__ = _lizards.delete_point_t
    __del__ = lambda self : None;
point_t_swigregister = _lizards.point_t_swigregister
point_t_swigregister(point_t)

class feature_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, feature_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, feature_t, name)
    __repr__ = _swig_repr
    def id(self): return _lizards.feature_t_id(self)
    def __init__(self): 
        this = _lizards.new_feature_t()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lizards.delete_feature_t
    __del__ = lambda self : None;
feature_t_swigregister = _lizards.feature_t_swigregister
feature_t_swigregister(feature_t)

class band_t(feature_t):
    __swig_setmethods__ = {}
    for _s in [feature_t]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, band_t, name, value)
    __swig_getmethods__ = {}
    for _s in [feature_t]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, band_t, name)
    __repr__ = _swig_repr
    def set(self, *args): return _lizards.band_t_set(self, *args)
    def get(self, *args): return _lizards.band_t_get(self, *args)
    def size(self): return _lizards.band_t_size(self)
    def __init__(self): 
        this = _lizards.new_band_t()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lizards.delete_band_t
    __del__ = lambda self : None;
band_t_swigregister = _lizards.band_t_swigregister
band_t_swigregister(band_t)

class raft_t(feature_t):
    __swig_setmethods__ = {}
    for _s in [feature_t]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, raft_t, name, value)
    __swig_getmethods__ = {}
    for _s in [feature_t]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, raft_t, name)
    __repr__ = _swig_repr
    __swig_setmethods__["age"] = _lizards.raft_t_age_set
    __swig_getmethods__["age"] = _lizards.raft_t_age_get
    if _newclass:age = _swig_property(_lizards.raft_t_age_get, _lizards.raft_t_age_set)
    def __init__(self): 
        this = _lizards.new_raft_t()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lizards.delete_raft_t
    __del__ = lambda self : None;
raft_t_swigregister = _lizards.raft_t_swigregister
raft_t_swigregister(raft_t)

class hex_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, hex_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, hex_t, name)
    __repr__ = _swig_repr
    __swig_setmethods__["owner"] = _lizards.hex_t_owner_set
    __swig_getmethods__["owner"] = _lizards.hex_t_owner_get
    if _newclass:owner = _swig_property(_lizards.hex_t_owner_get, _lizards.hex_t_owner_set)
    def getid(self): return _lizards.hex_t_getid(self)
    def setid(self, *args): return _lizards.hex_t_setid(self, *args)
    __swig_setmethods__["xy"] = _lizards.hex_t_xy_set
    __swig_getmethods__["xy"] = _lizards.hex_t_xy_get
    if _newclass:xy = _swig_property(_lizards.hex_t_xy_get, _lizards.hex_t_xy_set)
    def is_type(self, *args): return _lizards.hex_t_is_type(self, *args)
    def get(self, *args): return _lizards.hex_t_get(self, *args)
    def set(self, *args): return _lizards.hex_t_set(self, *args)
    def __init__(self): 
        this = _lizards.new_hex_t()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lizards.delete_hex_t
    __del__ = lambda self : None;
hex_t_swigregister = _lizards.hex_t_swigregister
hex_t_swigregister(hex_t)

class water_t(hex_t):
    __swig_setmethods__ = {}
    for _s in [hex_t]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, water_t, name, value)
    __swig_getmethods__ = {}
    for _s in [hex_t]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, water_t, name)
    __repr__ = _swig_repr
    def __init__(self): 
        this = _lizards.new_water_t()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lizards.delete_water_t
    __del__ = lambda self : None;
water_t_swigregister = _lizards.water_t_swigregister
water_t_swigregister(water_t)

class plains_t(hex_t):
    __swig_setmethods__ = {}
    for _s in [hex_t]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, plains_t, name, value)
    __swig_getmethods__ = {}
    for _s in [hex_t]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, plains_t, name)
    __repr__ = _swig_repr
    def __init__(self): 
        this = _lizards.new_plains_t()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lizards.delete_plains_t
    __del__ = lambda self : None;
plains_t_swigregister = _lizards.plains_t_swigregister
plains_t_swigregister(plains_t)

class fertile_t(hex_t):
    __swig_setmethods__ = {}
    for _s in [hex_t]: __swig_setmethods__.update(getattr(_s,'__swig_setmethods__',{}))
    __setattr__ = lambda self, name, value: _swig_setattr(self, fertile_t, name, value)
    __swig_getmethods__ = {}
    for _s in [hex_t]: __swig_getmethods__.update(getattr(_s,'__swig_getmethods__',{}))
    __getattr__ = lambda self, name: _swig_getattr(self, fertile_t, name)
    __repr__ = _swig_repr
    def __init__(self): 
        this = _lizards.new_fertile_t()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lizards.delete_fertile_t
    __del__ = lambda self : None;
fertile_t_swigregister = _lizards.fertile_t_swigregister
fertile_t_swigregister(fertile_t)

class layout_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, layout_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, layout_t, name)
    __repr__ = _swig_repr
    __swig_setmethods__["size"] = _lizards.layout_t_size_set
    __swig_getmethods__["size"] = _lizards.layout_t_size_get
    if _newclass:size = _swig_property(_lizards.layout_t_size_get, _lizards.layout_t_size_set)
    def realize(self): return _lizards.layout_t_realize(self)
    def __init__(self): 
        this = _lizards.new_layout_t()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lizards.delete_layout_t
    __del__ = lambda self : None;
layout_t_swigregister = _lizards.layout_t_swigregister
layout_t_swigregister(layout_t)

class layouts_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, layouts_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, layouts_t, name)
    __repr__ = _swig_repr
    def size(self): return _lizards.layouts_t_size(self)
    def __init__(self, *args): 
        this = _lizards.new_layouts_t(*args)
        try: self.this.append(this)
        except: self.this = this
    def layout(self, *args): return _lizards.layouts_t_layout(self, *args)
    __swig_destroy__ = _lizards.delete_layouts_t
    __del__ = lambda self : None;
layouts_t_swigregister = _lizards.layouts_t_swigregister
layouts_t_swigregister(layouts_t)

class template_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, template_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, template_t, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _lizards.new_template_t(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_setmethods__["cost"] = _lizards.template_t_cost_set
    __swig_getmethods__["cost"] = _lizards.template_t_cost_get
    if _newclass:cost = _swig_property(_lizards.template_t_cost_get, _lizards.template_t_cost_set)
    __swig_setmethods__["active"] = _lizards.template_t_active_set
    __swig_getmethods__["active"] = _lizards.template_t_active_get
    if _newclass:active = _swig_property(_lizards.template_t_active_get, _lizards.template_t_active_set)
    def realize(self): return _lizards.template_t_realize(self)
    def save(self, *args): return _lizards.template_t_save(self, *args)
    __swig_destroy__ = _lizards.delete_template_t
    __del__ = lambda self : None;
template_t_swigregister = _lizards.template_t_swigregister
template_t_swigregister(template_t)

class grid_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, grid_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, grid_t, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _lizards.new_grid_t(*args)
        try: self.this.append(this)
        except: self.this = this
    def lay_islands(self, *args): return _lizards.grid_t_lay_islands(self, *args)
    def generate_typical(self): return _lizards.grid_t_generate_typical(self)
    def __call__(self, *args): return _lizards.grid_t___call__(self, *args)
    __swig_setmethods__["width"] = _lizards.grid_t_width_set
    __swig_getmethods__["width"] = _lizards.grid_t_width_get
    if _newclass:width = _swig_property(_lizards.grid_t_width_get, _lizards.grid_t_width_set)
    __swig_setmethods__["height"] = _lizards.grid_t_height_set
    __swig_getmethods__["height"] = _lizards.grid_t_height_get
    if _newclass:height = _swig_property(_lizards.grid_t_height_get, _lizards.grid_t_height_set)
    def move(self, *args): return _lizards.grid_t_move(self, *args)
    def replace(self, *args): return _lizards.grid_t_replace(self, *args)
    def place_template(self, *args): return _lizards.grid_t_place_template(self, *args)
    def output(self): return _lizards.grid_t_output(self)
    __swig_destroy__ = _lizards.delete_grid_t
    __del__ = lambda self : None;
grid_t_swigregister = _lizards.grid_t_swigregister
grid_t_swigregister(grid_t)

class output_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, output_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, output_t, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _lizards.new_output_t(*args)
        try: self.this.append(this)
        except: self.this = this
    def svg(self, *args): return _lizards.output_t_svg(self, *args)
    def json(self, *args): return _lizards.output_t_json(self, *args)
    def hit(self, *args): return _lizards.output_t_hit(self, *args)
    __swig_destroy__ = _lizards.delete_output_t
    __del__ = lambda self : None;
output_t_swigregister = _lizards.output_t_swigregister
output_t_swigregister(output_t)

class player_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, player_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, player_t, name)
    __repr__ = _swig_repr
    __swig_setmethods__["startup"] = _lizards.player_t_startup_set
    __swig_getmethods__["startup"] = _lizards.player_t_startup_get
    if _newclass:startup = _swig_property(_lizards.player_t_startup_get, _lizards.player_t_startup_set)
    def __init__(self): 
        this = _lizards.new_player_t()
        try: self.this.append(this)
        except: self.this = this
    __swig_destroy__ = _lizards.delete_player_t
    __del__ = lambda self : None;
player_t_swigregister = _lizards.player_t_swigregister
player_t_swigregister(player_t)

class game_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, game_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, game_t, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _lizards.new_game_t(*args)
        try: self.this.append(this)
        except: self.this = this
    __swig_setmethods__["nplayers"] = _lizards.game_t_nplayers_set
    __swig_getmethods__["nplayers"] = _lizards.game_t_nplayers_get
    if _newclass:nplayers = _swig_property(_lizards.game_t_nplayers_get, _lizards.game_t_nplayers_set)
    def player(self, *args): return _lizards.game_t_player(self, *args)
    def bootstrap(self): return _lizards.game_t_bootstrap(self)
    __swig_destroy__ = _lizards.delete_game_t
    __del__ = lambda self : None;
game_t_swigregister = _lizards.game_t_swigregister
game_t_swigregister(game_t)

class overlay_grid_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, overlay_grid_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, overlay_grid_t, name)
    __repr__ = _swig_repr
    def __init__(self, *args): 
        this = _lizards.new_overlay_grid_t(*args)
        try: self.this.append(this)
        except: self.this = this
    def mark(self, *args): return _lizards.overlay_grid_t_mark(self, *args)
    __swig_destroy__ = _lizards.delete_overlay_grid_t
    __del__ = lambda self : None;
overlay_grid_t_swigregister = _lizards.overlay_grid_t_swigregister
overlay_grid_t_swigregister(overlay_grid_t)

class serial_t(_object):
    __swig_setmethods__ = {}
    __setattr__ = lambda self, name, value: _swig_setattr(self, serial_t, name, value)
    __swig_getmethods__ = {}
    __getattr__ = lambda self, name: _swig_getattr(self, serial_t, name)
    __repr__ = _swig_repr
    def __init__(self): 
        this = _lizards.new_serial_t()
        try: self.this.append(this)
        except: self.this = this
    def to_file(self, *args): return _lizards.serial_t_to_file(self, *args)
    def from_file(self, *args): return _lizards.serial_t_from_file(self, *args)
    __swig_destroy__ = _lizards.delete_serial_t
    __del__ = lambda self : None;
serial_t_swigregister = _lizards.serial_t_swigregister
serial_t_swigregister(serial_t)

# This file is compatible with both classic and new-style classes.


