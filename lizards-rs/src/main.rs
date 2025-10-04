/* ----------------------------------------------------------------------
--
-- lizards-rs
--
-- An attempt to learn rust using the lizards code as a base.
--
---------------------------------------------------------------------- */

#![allow(non_camel_case_types)]
#![allow(unused_imports)]
#![allow(dead_code)]
#![allow(unused_variables)]
#![allow(unused_mut)]

use cairo::{Context, Format, ImageSurface, Surface, PdfSurface, SvgSurface};
use int_enum::IntEnum;
use rand::prelude::*;
use serde::ser::{SerializeStruct, Serializer};
use serde::{Deserialize, Serialize};
use serde_json::{Number, Value};
use std::any::{Any, TypeId};
use std::f64;
use std::f64::consts::PI;
use std::fs::File;
use std::process::exit;
use std::slice::Iter;
use strum_macros::FromRepr;
use variant_count::VariantCount;

pub mod point;
pub mod floats;
use crate::point::*;
use crate::floats::*;

/* define co-ordinate as a signed 64 bit integer */

/* Use the enum values in Direction */

use self::Direction::*;

/* Create a point and use derive macro to create implementation of the trait */
/* The point is a tuple rather than explicit x & y members */

/* rust doesn't not have inheritance, instead "subclasses" have different
implementations of a "parent" trait */

#[derive(Deserialize)]
struct Blank {
    xy: Point,
}

#[derive(Deserialize)]
struct Water {
    xy: Point,
}

#[derive(Deserialize)]
struct Plains {
    xy: Point,
}

#[derive(PartialEq)]
enum Hexes {
    Blank,
    Water,
    Plains,
}

#[typetag::serde(tag = "type")]
trait Hex {
    fn descr(&self) -> String;
    fn loc(&self) -> Point;
    fn isa(&self, htype: Hexes) -> bool;
}

struct Grid {
    height: lsize,
    width: lsize,
    hexes: Vec<Box<dyn Hex>>,
}

struct World {
    players: Vec<Player>,
    grid: Grid,
}

struct Player {
    clan: String,
    index: i32,
}

/* The enum looks like a C enum are is a signed interger */

#[repr(isize)]
#[derive(Clone, Copy, Eq, PartialEq, IntEnum, VariantCount, FromRepr)]
enum Direction {
    NORTH = 0,
    NORTHEAST = 1,
    SOUTHEAST = 2,
    SOUTH = 3,
    SOUTHWEST = 4,
    NORTHWEST = 5,
}

/* ----------------------------------------------------------------------
--
-- implementations
--
---------------------------------------------------------------------- */

impl Serialize for Blank {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        let mut g = serializer.serialize_struct("hex", 2)?;
        g.serialize_field("x", &self.xy.0)?;
        g.serialize_field("y", &self.xy.1)?;
        g.end()
    }
}

impl Serialize for Water {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        let mut g = serializer.serialize_struct("hex", 2)?;
        g.serialize_field("x", &self.xy.0)?;
        g.serialize_field("y", &self.xy.1)?;
        g.end()
    }
}

impl Serialize for Plains {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        let mut g = serializer.serialize_struct("hex", 2)?;
        g.serialize_field("x", &self.xy.0)?;
        g.serialize_field("y", &self.xy.1)?;
        g.end()
    }
}

impl Direction {
    pub fn iterator() -> impl Iterator<Item = Direction> {
        [NORTH, NORTHEAST, SOUTHEAST, SOUTH, SOUTHWEST, NORTHWEST]
            .iter()
            .copied()
    }
}

#[typetag::serde]
impl Hex for Blank {
    fn descr(&self) -> String {
        format!("{:2} × {:2} Blank", self.xy.0, self.xy.1)
    }
    fn loc(&self) -> Point {
        self.xy
    }
    fn isa(&self, htype: Hexes) -> bool {
        return if htype == Hexes::Blank { true } else { false };
    }
}

#[typetag::serde(name = "WAT")]
impl Hex for Water {
    fn descr(&self) -> String {
        format!("{:2} × {:2} {:3} Water", self.xy.0, self.xy.1, self.xy.id())
    }
    fn loc(&self) -> Point {
        self.xy
    }
    fn isa(&self, htype: Hexes) -> bool {
        return if htype == Hexes::Water { true } else { false };
    }
}

#[typetag::serde(name = "PLA")]
impl Hex for Plains {
    fn descr(&self) -> String {
        format!("{:2} × {:2} {:3} Plains", self.xy.0, self.xy.1, self.xy.id())
    }
    fn loc(&self) -> Point {
        self.xy
    }
    fn isa(&self, htype: Hexes) -> bool {
        return if htype == Hexes::Plains { true } else { false };
    }
}

impl Grid {
    fn new(h: lsize, w: lsize) -> Self {
        let world = Grid {
            height: h,
            width: w,
            hexes: Vec::with_capacity((h * w) as usize),
        };
        return world;
    }
    fn get(&mut self, p: Point) -> &Box<dyn Hex> {
        let index = p.0 + p.1 * self.width;
        return &self.hexes[index as usize];
    }
    fn set(&mut self, p: Point, hex: Box<dyn Hex>) -> Box<dyn Hex> {
        let index = p.0 + p.1 * self.width;
        let mut ret = std::mem::replace(&mut self.hexes[index as usize], hex);
        return ret;
    }
    fn replace(&mut self, p: Point, hex: Box<dyn Hex>) {
        let mut ret = self.set(p, hex);
        drop(ret);
    }
    fn move_dir(&mut self, p: Point, d: Direction) -> Point {
        let mut ret: Point = p;
        match d {
            NORTH => {
                ret.1 -= 1;
            }
            NORTHEAST => {
                ret.0 += 1;
                if p.0 % 2 == 1 {
                    ret.1 -= 1;
                }
            }
            SOUTHEAST => {
                ret.0 += 1;
                if p.0 % 2 == 0 {
                    ret.1 += 1
                }
            }
            SOUTH => {
                ret.1 += 1;
            }
            SOUTHWEST => {
                ret.0 -= 1;
                if p.0 % 2 == 0 {
                    ret.1 += 1;
                }
            }
            NORTHWEST => {
                ret.0 -= 1;
                if p.0 % 2 == 1 {
                    ret.1 -= 1;
                }
            }
        }
        if ret.0 < 0 {
            ret.0 += self.width;
        } else if ret.0 >= self.width {
            ret.0 -= self.width;
        }

        if ret.1 < 0 {
            ret.1 += self.height;
        } else if ret.1 >= self.height {
            ret.1 -= self.height;
        }
        return ret;
    }

    fn lay_island(&mut self, p: Point, land_density: f64) -> usize {
        let mut land: usize = 0;
        let mut rng = rand::rng();

        if rng.random::<f64>() < land_density {
            self.replace(p, Box::new(Plains { xy: p }));
            land += 1;
        }
        for dir in Direction::iterator() {
            let p: Point = self.move_dir(p, dir);
            self.replace(p, Box::new(Plains { xy: p }));
            land += 1;
        }

        /* --------------------
        Check edge proximity
        -------------------- */

        let proximity: f64 = if (p.0 <= (self.width / 5) || p.0 >= (self.width - (self.width / 5)))
            && (p.1 <= (self.height / 5) || (p.1 >= (self.height - (self.height / 5))))
        {
            1.40 * land_density
        } else {
            0.60 * land_density
        };

        let prox_rng = rng.random::<f64>();
        if prox_rng < proximity {
            let mut start: isize =
                rng.random_range((NORTH as usize)..(NORTHWEST as usize)) as isize; // .try_into().unwrap();
            let dir: isize = if rng.random::<f64>() < land_density {
                1
            } else {
                -1
            };
            let num_dir: isize = Direction::VARIANT_COUNT as isize;

            for d in Direction::iterator() {
                if rng.random::<f64>() < 0.35 {
                    break;
                };
                println!(
                    "-- Proximity = {} RNG = {} start = {} dir = {} num_dir = {}",
                    proximity, prox_rng, start, dir, num_dir
                );
                start = (start + dir + num_dir) % num_dir;
                println!(
                    "++ Proximity = {} RNG = {} start = {} dir = {} num_dir = {}",
                    proximity, prox_rng, start, dir, num_dir
                );
                let new_dir: Direction = Direction::from_repr(start).unwrap();
                let z = self.move_dir(p, new_dir);
                println!("** {} {}", self.get(p).descr(), self.get(z).descr());
                let mut new_hex = self.get(z);
                if new_hex.isa(Hexes::Plains) {
                    land -= 1;
                    self.replace(z, Box::new(Water { xy: p }));
                }
            }
        }
        return land;
    }

    fn lay_islands(&mut self, land_density: f64) {
        println!("Laying down water...");
        for y in 0..self.height {
            for x in 0..self.width {
                let mut p: Point = Point(x, y);
                let mut hex: Box<dyn Hex> = Box::new(Water { xy: p });
                self.set(Point(x, y), hex);
            }
        }
        println!("Raising Islands I...");
        let mut land: usize = 0;
        for y in (1..self.height).step_by(4) {
            for x in (1..self.width).step_by(4) {
                land += self.lay_island(Point(x, y), land_density);
            }
        }
    }

    fn construct(&mut self) {
        for y in 0..self.height {
            for x in 0..self.width {
                self.hexes.push(Box::new(Blank { xy: Point(x, y) }));
            }
        }

        let land_density: f64 = 0.50;
        self.lay_islands(land_density);

        for y in 0..self.height {
            for x in 0..self.width {
                println!("{}", self.get(Point(x, y)).descr());
            }
        }
    }

    fn edge(&mut self, radius: f64, border: f64) -> FBox {
        // Calculate the full bounding box

        let mut edge = FBox::new();
        for y in 0..self.height {
            for x in 0..self.width {
                let h = self.get(Point(x, y)); // Currently not used
                let place = FPlace::place(Point(x, y), radius, border);
                if (x == 0) && (y == 0) {
                    edge = place.bounds
                } else {
                    edge.setunion(&place.bounds);
                }
            }
        }
        println!(
            "edge box: {:5.2} {:5.2} {:5.2} {:5.2}",
            edge.left, edge.right, edge.top, edge.bottom
        );
        return edge;
    }
    fn cairo_paint(&mut self, radius: f64, border: f64, surface: &cairo::Surface) {
        let edge: FBox = self.edge(radius, border);

        let loader = rsvg::Loader::new();
        let mut rng = rand::rng();
        let paths = [
            "../images/scouts.svg",
            "../images/flee.svg",
            "../images/fight-1.svg",
            "../images/fight-2.svg",
            "../images/dance.svg",
            "../images/catouche.svg",
        ];
        // let handle: [rsvg::SvgHandle; 3]  = paths.map(|x| { rsvg::Loader::new().read_path(x).unwrap() });
        let handle = paths.map(|x| { rsvg::Loader::new().read_path(x).unwrap() });

//        let href = handle.each_ref();
        let renderer = handle.each_ref().map(|f| rsvg::CairoRenderer::new(f));
        //        let renderer: [rsvg::CairoRenderer; 3] = href.map(|f| rsvg::CairoRenderer::new(f));

//        let renderer: [rsvg::CairoRenderer; 3] = [
//            rsvg::CairoRenderer::new(&handle[0]),
//            rsvg::CairoRenderer::new(&handle[1]),
//            rsvg::CairoRenderer::new(&handle[2]),
//        ];
        
        // Get image width & height as signed integers
        let context = Context::new(&surface).expect("Could not create context");

        context.translate(-edge.left, -edge.top); // translate to match bounding box to (0,0) (w,h) image box

        context.set_source_rgb(1.0, 1.0, 1.0); // Set image background to white
        context.paint().unwrap(); // Not sure why paint returns an error when C API function is void

        context.set_line_width(1.0);

        context.select_font_face("Adwaita Sans", cairo::FontSlant::Normal, cairo::FontWeight::Bold);
        context.set_font_size(10.0);

        let sqrt3: f64 = (3.0 as f64).sqrt();

        for y in 0..self.height {
            for x in 0..self.width {
                let p = Point(x, y);
                let h = self.get(Point(x, y));
                let place = FPlace::place(Point(x, y), radius, border);

                context.new_path();
                context.move_to(place.hull[0].0, place.hull[0].1);
                for i in 1..6 {
                    context.line_to(place.hull[i].0, place.hull[i].1);
                }
                context.close_path();

                if h.isa(Hexes::Water) {
                    context.set_source_rgb(0.6, 0.6, 1.0);
                } else {
                    context.set_source_rgb(0.6, 1.0, 0.6);
                }
                context.fill_preserve().unwrap();
                context.set_source_rgb(0.0, 0.0, 0.0);
                context.stroke().unwrap();

                let view = cairo::Rectangle::new(place.center.0 - 20.0, place.center.1 - 20.0, 40.0, 40.0); // {.x = cx - 20.0, .y = cy - 15.0, .width = 40, .height = 40}
                let r = rng.random::<u32>() % paths.len() as u32;
                renderer[r as usize].render_document(&context, &view).unwrap();

                let s: String = p.id();
                let ext: cairo::TextExtents = context.text_extents(&s).unwrap();
                context.move_to(place.center.0 - (ext.width() / 2.0), place.center.1 + (radius * sqrt3 / 2.0) - 2.0);
                context.show_text(&s).unwrap();
            }
        }
    }
    fn cairo_png(&mut self, radius: f64, border: f64, path: &str) {

        let edge: FBox = self.edge(radius, border);
        let width: i32 = (edge.right - edge.left) as i32;
        let height: i32 = (edge.bottom - edge.top) as i32;

        println!("canvas size: {} {}", width, height);

        // Create cairo surface & context

        let image =
            ImageSurface::create(Format::ARgb32, width, height).expect("Could not create surface");
        self.cairo_paint(radius, border, image.as_ref());
        let mut stream = File::create(path).expect("Could not create file");
        image
            .write_to_png(&mut stream)
            .expect("Could not write png");
        let pdf = PdfSurface::new(width as f64, height as f64, "output.pdf").expect("Could not create PDF surface");
        self.cairo_paint(radius, border, pdf.as_ref());
    }
}

impl Serialize for Player {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        let mut g = serializer.serialize_struct("player", 2)?;
        g.serialize_field("clan", &self.clan)?;
        g.serialize_field("index", &self.index)?;
        g.end()
    }
}

impl Serialize for Grid {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        let mut g = serializer.serialize_struct("grid", 3)?;
        g.serialize_field("width", &self.width)?;
        g.serialize_field("height", &self.height)?;
        g.serialize_field("hexes", &self.hexes)?;
        g.end()
    }
}

impl Serialize for World {
    fn serialize<S>(&self, serializer: S) -> Result<S::Ok, S::Error>
    where
        S: Serializer,
    {
        let mut g = serializer.serialize_struct("world", 2)?;
        g.serialize_field("grid", &self.grid)?;
        g.serialize_field("players", &self.players)?;
        g.end()
    }
}

/* ----------------------------------------------------------------------
--
-- world_load_json
--
---------------------------------------------------------------------- */

impl World {
    fn load_json(input_path: &str) -> Box<World> {
        let mut json: serde_json::Value = {
            // Load the first file into a string.
            let text = std::fs::read_to_string(&input_path).unwrap();
            
            // Parse the string into a dynamically-typed JSON structure.
            serde_json::from_str::<Value>(&text).unwrap()
        };
        
        println!("Loading json file {}", input_path);
        
        let width: i64 = json["width"].as_i64().expect("size must be integer");
        let height: i64 = json["height"].as_i64().expect("size must be integer");
        
        println!("{} {}", width, height);

        type WorldResult = std::result::Result<Box<World>, &'static str>;

        let res: WorldResult = match &json["hexes"] {
            serde_json::Value::Null => {
                Ok(Box::new(World {
                    players: Vec::new(),
                    grid: Grid::new(width as lsize, height as lsize),
                }))
            }
            serde_json::Value::Object(_) => {
                Err("Found object")
            }
            serde_json::Value::Array(hexes) => {
                println!("{}", hexes.len());
                for hex in hexes {
                    println!("Found {:?}", hex);
                }
                Err("Found Array")
            }
            _ => {
                Err("Incorrect structure")
            }
        };

        //        let mut world =;
        //    world.players.push(Player { index: 0, clan: String::from("WLD") });
        return res.expect("World creation failed");
    }
}

/* ----------------------------------------------------------------------
--
-- main
--
---------------------------------------------------------------------- */

fn main() {
    let mut args = &mut std::env::args();
    let argc = args.len();
    let progname: String = args.nth(0).unwrap();

    if argc < 2 {
        println!("{}: <world.js>", progname);
        exit(1);
    }

    // Get the filenames from the command line.  Because we have
    // already got an item from args nth(0) is actually argv[1]
    
    let mut world: Box<World> = World::load_json(&args.nth(0).unwrap());
    world.grid.construct();

    let g = &mut world.grid;
    g.cairo_png(30.0, 5.0, "output.png");

    // Save the JSON structure into the other file.

    std::fs::write("grid.js", serde_json::to_string_pretty(&world).unwrap()).unwrap();
}
