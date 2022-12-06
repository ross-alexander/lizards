#![allow(non_camel_case_types)] 
#![allow(unused_imports)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_mut)]

extern crate serde_json;

use serde_json::{Number, Value};

#[derive(Copy, Clone)]
struct Point(usize, usize);

trait Hex {
    fn descr(&self) -> String;
    fn loc(&self) -> Point;
}

struct Blank {
    xy: Point
}

struct Water {
    xy: Point
}

impl Hex for Blank {
    fn descr(&self) -> String {
        format!("Blank {} × {}", self.xy.0, self.xy.1)
    }
    fn loc(&self) -> Point {
        self.xy
    }
}

impl Hex for Water {
    fn descr(&self) -> String {
        format!("Water {} × {}", self.xy.0, self.xy.1)
    }
    fn loc(&self) -> Point {
        self.xy
    }
}

struct World {
    size : usize,
    grid : Vec::<Box<dyn Hex>>
}

impl World {
    fn new(s : usize) -> Self
    {
        let world = World {size:s, grid:Vec::with_capacity(s * s)};
        return world
    }
    fn construct(&mut self)
    {
        let size = self.size;
        
        for y in 0 .. size {
            for x in 0 .. size {
                self.grid.push(Box::new(Blank {xy: Point(x, y)}));
                println!("{} × {}", x, y)
            }
        }
        for y in 0 .. size {
            for x in 0 .. size {
                println!("{}", self.grid[y*size + x].descr());
            }
        }
        
        for y in 0 .. size {
            for x in 0 .. size {
                let hex : &Box<dyn Hex> = &self.grid[y*size+x];
                self.grid[y*size+x] = Box::new(Water {xy: hex.loc()});
            }
        }
        
        for y in 0 .. size {
            for x in 0 .. size {
                println!("{}", self.grid[y*size + x].descr());
            }
        }
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
    let size_i64 : i64 = json["scale"].as_i64().expect("scale must be integer");
    let mut world = Box::new(World::new(size_i64 as usize));
    return world
}

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */

fn main() {
    let mut world = world_load_json();
    world.construct();
    
    // // Save the JSON structure into the other file.
    // std::fs::write(
    //     output_path,
    //     serde_json::to_string_pretty(&missy_diet).unwrap(),
    // )
    // .unwrap();
}
