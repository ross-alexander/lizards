#include <string>
#include <map>
#include <vector>
#include <typeinfo>

#include <errno.h>
#include <assert.h>
#include <cairomm/context.h>

#include <json-c/json.h>

#include "lizards.h"

serial_t::serial_t()
{
  json = 0;
}

serial_t::serial_t(json_object *js)
{
  json = js;
}

serial_map_t::serial_map_t()
{
  json = json_object_new_object();
}

serial_map_t::serial_map_t(serial_t s)
{
  json = s.json;
}

serial_array_t::serial_array_t(serial_t s)
{
  json = s.json;
  assert(json_object_is_type(json, json_type_array));
}

serial_t::serial_t(const char *s)
{
  json = json_object_new_string(s);
}

serial_t::serial_t(int i)
{
  json = json_object_new_int(i);
}

serial_t::serial_t(double d)
{
  json = json_object_new_double(d);
}

serial_array_t::serial_array_t()
{
  json = json_object_new_array();
}  

serial_t& serial_map_t::add(const char *k, serial_t v)
{
  json_object_object_add(json, k, v.json);
  return (*this);
}

serial_t& serial_map_t::add(std::string k, serial_t v)
{
  json_object_object_add(json, k.c_str(), v.json);
  return (*this);
}

serial_t& serial_array_t::add(serial_t v)
{
  json_object_array_add(json, v.json);
  return (*this);
}

const char* serial_t::to_string(unsigned int format)
{
  return json_object_to_json_string_ext(json, JSON_C_TO_STRING_PRETTY);
}

int serial_t::to_file(const char *file)
{
  return json_object_to_file_ext((char*)file, json, JSON_C_TO_STRING_PRETTY);
}

serial_t serial_t::from_file(const char* file)
{
  json_object *js = json_object_from_file(file);
  assert(js);
  return serial_t(js);
}
  
int serial_t::get(const char* s, int& x)
{
  json_object *o;
  if (!json_object_object_get_ex(json, s, &o))
    return 0;
  if (!json_object_is_type(o, json_type_int))
    return 0;
  x = json_object_get_int(o);
  return 1;
}

int serial_t::get(const char* s, const char*& x)
{
  json_object *o;
  if (!json_object_object_get_ex(json, s, &o))
    return 0;
  if (!json_object_is_type(o, json_type_string))
    return 0;
  x = json_object_get_string(o);
  return 1;
}

int serial_t::get(const char* s, std::string& x)
{
  json_object *o;
  if (!json_object_object_get_ex(json, s, &o))
    return 0;
  if (!json_object_is_type(o, json_type_string))
    return 0;
  x = json_object_get_string(o);
  return 1;
}


int serial_t::get(const char* s, serial_map_t& x)
{
  json_object *o;
  if (!json_object_object_get_ex(json, s, &o))
    return 0;
  if (!json_object_is_type(o, json_type_object))
    return 0;
  x = serial_map_t(o);
  return 1;
}

int serial_t::get(const char* s, serial_array_t& x)
{
  json_object *o;
  if (!json_object_object_get_ex(json, s, &o))
    return 0;
  if (!json_object_is_type(o, json_type_array))
    return 0;
  x = serial_array_t(o);
  return 1;
}

unsigned int serial_array_t::size()
{
  return json_object_array_length(json);
}

serial_t serial_array_t::operator[](int x)
{
  json_object* o = json_object_array_get_idx(json, x);
  return serial_t(o);
}
