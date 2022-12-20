#![allow(non_camel_case_types)] 
#![allow(unused_imports)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_mut)]
#![feature(variant_count)]

extern crate serde;
extern crate serde_json;

// extern crate num;
// #[macro_use]
// extern crate num_derive;

use serde::{Deserialize, Serialize};
use serde::ser::{Serializer, SerializeStruct};
use serde_json::{Number, Value};
use rand::Rng;
use int_enum::IntEnum;

type lsize = i64;

use self::Direction::*;
use std::slice::Iter;

#[derive(Copy, Clone,Deserialize)]
struct Point(lsize, lsize);

#[typetag::serde(tag = "type")]
trait Hex {
    fn descr(&self) -> String;
    fn loc(&self) -> Point;
}

#[derive(Deserialize)]
struct Blank {
    xy: Point
}

#[derive(Deserialize)]
struct Water {
    xy: Point
}

#[derive(Deserialize)]
struct Plains {
    xy: Point
}

struct Player {
    clan: String,
    index: i32
}

impl Serialize for Blank {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer, {
        let mut g = serializer.serialize_struct("hex", 2)?;
        g.serialize_field("x", &self.xy.0)?;
        g.serialize_field("y", &self.xy.1)?;
        g.end()
    }
}    

impl Serialize for Water {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer, {
        let mut g = serializer.serialize_struct("hex", 2)?;
        g.serialize_field("x", &self.xy.0)?;
        g.serialize_field("y", &self.xy.1)?;
        g.end()
    }
}    

impl Serialize for Plains {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer, {
        let mut g = serializer.serialize_struct("hex", 2)?;
        g.serialize_field("x", &self.xy.0)?;
        g.serialize_field("y", &self.xy.1)?;
        g.end()
    }
}    


#[repr(isize)]
#[derive(Clone, Copy, Eq, PartialEq, IntEnum)]
enum Direction {
    NORTH = 0,
    NORTHEAST = 1,
    SOUTHEAST = 2,
    SOUTH = 3,
    SOUTHWEST = 4,
    NORTHWEST = 5
}

impl Direction {
    pub fn iterator() -> impl Iterator<Item = Direction> {
        [NORTH, NORTHEAST, SOUTHEAST, SOUTH, SOUTHWEST, NORTHWEST].iter().copied()
    }
}

#[typetag::serde]
impl Hex for Blank {
    fn descr(&self) -> String {
        format!("Blank {} × {}", self.xy.0, self.xy.1)
    }
    fn loc(&self) -> Point {
        self.xy
    }
}

#[typetag::serde(name = "WAT")]
impl Hex for Water {
    fn descr(&self) -> String {
        format!("Water {} × {}", self.xy.0, self.xy.1)
    }
    fn loc(&self) -> Point {
        self.xy
    }
}

#[typetag::serde(name = "PLA")]
impl Hex for Plains {
    fn descr(&self) -> String {
        format!("Plains {} × {}", self.xy.0, self.xy.1)
    }
    fn loc(&self) -> Point {
        self.xy
    }
}

struct Grid {
    height: lsize,
    width: lsize,
    hexes: Vec::<Box<dyn Hex>>
}

struct World {
    players: Vec::<Player>,
    grid: Grid
}

impl Grid {
    fn new(h : lsize, w : lsize) -> Self
    {
        let world = Grid {height:h, width:w, hexes:Vec::with_capacity((h * w) as usize)};
        return world
    }
    fn get(&mut self, p:Point) -> &Box<dyn Hex> {
        let index = p.0 + p.1 * self.width;
        return &self.hexes[index as usize];
    }
    fn set(&mut self, p : Point, hex : Box<dyn Hex>) -> Box<dyn Hex>
    {
        let index = p.0 + p.1 * self.width;
        let mut ret = std::mem::replace(&mut self.hexes[index as usize], hex);
        return ret;
    }
    fn replace(&mut self, p:Point, hex:Box<dyn Hex>)
    {
        let mut ret = self.set(p, hex);
        drop(ret);
    }
    fn move_dir(&mut self, p:Point, d:Direction) -> Point
    {
        let mut ret : Point = p;
        match d {
            NORTH => {
                ret.1 -= 1;
            }
            NORTHEAST => {
                ret.0 += 1;
                if p.0%2 == 1 {
                    ret.1 -= 1;
                }
            }
            SOUTHEAST => {
                ret.0 += 1;
                if p.0%2 == 0 {
                    ret.1 += 1
                }
            }
            SOUTH => {
                ret.1 += 1;
            }
            SOUTHWEST => {
                ret.0 -= 1;
                if p.0%2 == 0 {
                    ret.1 += 1;
                }
            }
            NORTHWEST => {
                ret.0 -= 1;
                if p.0%2 == 1 {
                    ret.1 -= 1;
                }
            }
        }
        if ret.0 < 0 {
            ret.0 += self.width;
        }
        else if ret.0 >= self.width {
            ret.0 -= self.width;
        }

        if ret.1 < 0 {
            ret.1 += self.height;
        }
        else if ret.1 >= self.height {
            ret.1 -= self.height;
        }
        return ret
    }

    fn lay_island(&mut self, p : Point, land_density : f64) -> usize
    {
        let mut land : usize = 0;
        let mut rng = rand::thread_rng();
        
        if rng.gen::<f64>()  < land_density
        {
            self.replace(p, Box::new(Plains {xy:p}));
            land += 1;
        }
        for dir in Direction::iterator()
        {
            let p:Point = self.move_dir(p, dir);
            self.replace(p, Box::new(Plains {xy:p}));
            land += 1;                         
        }

        /* --------------------
        Check edge proximity
        -------------------- */

        let proximity : f64 = if (p.0 <= (self.width / 5) || p.0 >= (self.width - (self.width / 5))) &&
		                  (p.1 <= (self.height / 5) || p.1 >= (self.height - (self.height / 5)))
        {1.40 * land_density} else {0.60 * land_density};
        
        if rng.gen::<f64>() < proximity
        {
            let mut start : isize = rng.gen_range((NORTH as isize) .. (NORTHWEST as isize));
            let dir : isize = if rng.gen::<f64>() < land_density {1} else {-1};
            let num_dir : isize = std::mem::variant_count::<Direction>() as isize;
            for d in Direction::iterator()
            {
                if rng.gen::<f64>() < 0.35 { break };
                start = (start + dir + num_dir)%num_dir;
                let new_dir : Direction = Direction::from_int(start).unwrap();
                let z = self.move_dir(p, new_dir);
            }
            /*
      for (int i = 0; i < DIRECTIONS && misc_t::uniform(100) < 35; i++)
	{
	  start = (start + dir + DIRECTIONS) % DIRECTIONS;
	  point_t z = move(p, start);
	  hex_t *a_hex = (*this)(z);
	  if (dynamic_cast<plains_t*>(a_hex))
	    {
	      land--;
	      replace(p, new water_t);
	    }
	}
             */
        }
        return land;
}

    fn lay_islands(&mut self, land_density : f64)
    {
        println!("Laying down water...");
        for y in 0 .. self.height {
            for x in 0 .. self.width {
                let mut p:Point = Point(x, y);
                let mut hex:Box<dyn Hex> = Box::new(Water {xy: p});
                self.set(Point(x, y), hex);
            }
        }
        println!("Raising Islands I...");
        let mut land : usize = 0;
        for y in  (1 ..  self.height).step_by(4) {
            for x in ( 1 .. self.width).step_by(4) {
                land += self.lay_island(Point(x, y), land_density);
            }
        }
    }
    
    fn construct(&mut self)
    {
        for y in 0 .. self.height {
            for x in 0 .. self.width {
                self.hexes.push(Box::new(Blank {xy: Point(x, y)}));
            }
        }

        let land_density : f64  = 0.50;
        self.lay_islands(land_density);

        for y in 0 .. self.height {
            for x in 0 .. self.width {
                println!("{}", self.get(Point(x, y)).descr());
            }
        }
    }
}



impl Serialize for Player {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer, {
        let mut g = serializer.serialize_struct("player", 2)?;
        g.serialize_field("clan", &self.clan)?;
        g.serialize_field("index", &self.index)?;
        g.end()
    }
}

impl Serialize for World {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer, {
        let mut g = serializer.serialize_struct("world", 2)?;
        g.serialize_field("grid", &self.grid)?;
        g.serialize_field("players", &self.players)?;
        g.end()
    }
}

impl Serialize for Grid {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer, {
        let mut g = serializer.serialize_struct("grid", 3)?;
        g.serialize_field("width", &self.width)?;
        g.serialize_field("height", &self.height)?;
        g.serialize_field("hexes", &self.hexes)?;
        g.end()
    }
}


/* ----------------------------------------------------------------------
--
-- world_load_json
--
---------------------------------------------------------------------- */

fn world_load_json() -> Box<World> {
    // Get the filenames from the command line.
    let input_path = std::env::args().nth(1).unwrap();
//    let output_path = std::env::args().nth(2).unwrap();

    let mut json = {
        // Load the first file into a string.
        let text = std::fs::read_to_string(&input_path).unwrap();

        // Parse the string into a dynamically-typed JSON structure.
        serde_json::from_str::<Value>(&text).unwrap()
    };
    let size_i64 : i64 = json["size"].as_i64().expect("size must be integer");
    let mut world = Box::new(World {players: Vec::new(), grid: Grid::new(size_i64 as lsize, size_i64 as lsize)});
//    world.players.push(Player { index: 0, clan: String::from("WLD") });
    return world
}

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */

fn main() {
    let mut world = world_load_json();
    world.grid.construct();
    
    // Save the JSON structure into the other file.
    
    std::fs::write("grid.js",serde_json::to_string_pretty(&world).unwrap()).unwrap();
}
