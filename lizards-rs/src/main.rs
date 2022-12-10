#![allow(non_camel_case_types)] 
#![allow(unused_imports)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_mut)]

extern crate serde_json;

use serde_json::{Number, Value};
use rand::Rng;

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
    fn set(&mut self, p : Point, hex : Box<dyn Hex>) -> Box<dyn Hex>
    {
        let index = p.1 + self.size + p.0;
        let mut ret = std::mem::replace(&mut self.grid[index], hex);
        return ret;
    }
    
    fn lay_island(&mut self, p : Point, land_density : f64) -> usize
    {
        let land : usize = 0;
        let mut rng = rand::thread_rng();
/*        
        if (rng.gen::<f64>  < land_density)
        {
            replace(p, new plains_t);
            land++;
    }
        */
/*        
        for (int dir = 0; dir < DIRECTIONS; dir++)
        {
            point_t z = this->move(p, dir);
            self.set(
            replace(z, new plains_t);
            land++;
        }
*/
  /* --------------------
     Check edge proximity
     -------------------- */
/*  
  int proximity =  ((p.x <= (width / 5) || p.x >= (width - (width / 5))) &&
		    (p.y <= (height / 5) || p.y >= (height - (height / 5)))) ?
    (140 * land_density) : (60 * land_density);

  if (misc_t::uniform(100) < proximity)
    {
      int start = misc_t::uniform(DIRECTIONS);
      int dir = (misc_t::uniform(100) < (100 * land_density)) ? 1 : -1;

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
    }
        */
        return land;
}

    fn lay_islands(&mut self, land_density : f64)
    {
        println!("Laying down water...");
        for y in 0 .. self.size {
            for x in 0 .. self.size {
                let mut p:Point = Point(x, y);
                let mut hex:Box<dyn Hex> = Box::new(Water {xy: p});
                self.set(Point(x, y), hex);
            }
        }
        println!("Raising Islands I...");
        let mut land : usize = 0;
        for y in  (1 ..  self.size).step_by(4) {
            for x in ( 1 .. self.size).step_by(4) {
                land += self.lay_island(Point(x, y), land_density);
            }
        }
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
        let land_density : f64  = 0.50;
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
