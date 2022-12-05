#![allow(non_camel_case_types)] 
#![allow(unused_imports)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_mut)]

extern crate serde_json;

use serde_json::{Number, Value};

trait Hex {
    fn descr(&self) -> String;
}

struct Point(usize, usize);

struct Blank {
    xy: Point
}


impl Hex for Blank {
    fn descr(&self) -> String {
        format!("Blank {} × {}", self.xy.0, self.xy.1)
    }
}

struct hex_t {
    x : i64,
    y : i64,
    label : String
}

fn main() {
    // Get the filenames from the command line.
    let input_path = std::env::args().nth(1).unwrap();
//    let output_path = std::env::args().nth(2).unwrap();

    let mut world = {
        // Load the first file into a string.
        let text = std::fs::read_to_string(&input_path).unwrap();

        // Parse the string into a dynamically-typed JSON structure.
        serde_json::from_str::<Value>(&text).unwrap()
    };

    let size_i64 : i64 = world["scale"].as_i64().expect("scale must be integer");
    let size : usize = size_i64 as usize;
    let mut swamp : Vec::<Box<dyn Hex>> = Vec::with_capacity(size * size);

    for y in 0 .. size {
        for x in 0 .. size {
            swamp.push(Box::new(Blank {xy: Point(x, y)}));
                       //            swamp[(y*size + x) as usize] = 
                       println!("{} × {}", x, y)
        }
    }
    for y in 0 .. size {
        for x in 0 .. size {
            println!("{}", swamp[y*size + x].descr());
        }
    }
    
    // // Save the JSON structure into the other file.
    // std::fs::write(
    //     output_path,
    //     serde_json::to_string_pretty(&missy_diet).unwrap(),
    // )
    // .unwrap();
}
