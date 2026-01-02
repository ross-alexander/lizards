use std::f64;
use std::f64::consts::PI;

type fsize = f64;

pub use crate::point::*;

/* FP64 structures for cairo for mapping to cairo geometry */

#[derive(Copy, Clone)]
pub struct FPoint(pub fsize, pub fsize);

pub struct FBox {
    pub left: fsize,
    pub right: fsize,
    pub top: fsize,
    pub bottom: fsize,
}

pub struct FPlace {
    pub center: FPoint,
    pub bounds: FBox,
    pub hull: [FPoint; 6],
}

impl FPoint {
    pub fn new() -> Self {
        return FPoint(0.0, 0.0);
    }
}

impl FBox {
    pub fn new() -> Self {
        return FBox {
            left: 0.0,
            right: 0.0,
            top: 0.0,
            bottom: 0.0,
        };
    }
    pub fn setunion(&mut self, b: &FBox) -> &mut Self {
        if b.left < self.left {
            self.left = b.left;
        }
        if b.right > self.right {
            self.right = b.right;
        }
        if b.top < self.top {
            self.top = b.top;
        }
        if b.bottom > self.bottom {
            self.bottom = b.bottom;
        }
        return self;
    }
}

impl FPlace {
    pub fn place(p: Point, radius: f64, border: f64) -> Self {
        let sqrt3: f64 = (3.0 as f64).sqrt();
        let yshift: f64 = (p.0.abs() % 2) as f64 / 2.0;
        let cx: f64 = 1.50 * radius * p.0 as f64;
        let cy: f64 = sqrt3 * radius * (p.1 as f64 - yshift);

        let center = FPoint(cx, cy);
        let bounds = FBox {
            left: cx - radius - border,
            right: cx + radius + border,
            top: cy - sqrt3 / 2.0 * radius - border,
            bottom: cy + sqrt3 / 2.0 * radius + border,
        };

        let mut hull: [FPoint; 6] = [FPoint(0.0, 0.0); 6];

        for j in 0..6 {
            let x: f64 = ((j as f64) * 60.0 * PI / 180.0).cos();
            let y: f64 = ((j as f64) * 60.0 * PI / 180.0).sin();
            hull[j] = FPoint(cx + x * radius, cy + y * radius);
        }
        let new = FPlace {
            center: center,
            bounds: bounds,
            hull: hull,
        };
        return new;
    }
}
