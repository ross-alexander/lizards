use serde::{Deserialize, Serialize};
use std::str::from_utf8;

pub type lsize = i64;

#[derive(Copy, Clone, Deserialize)]
pub struct Point(pub lsize, pub lsize);

impl Point {
    pub fn id(&self) -> String {
        let mut s: String = std::format!("{}", self.1 + 1);
        let mut k = 0;
         // The conversion from a rust string to a C string (in ffi for cairo) does not like null characters, even if they are at the end of the string, so need to fit the text in exactly       
        if self.0 >= 26 {
            let mut bvec:[u8; 2] = [b'A' + ((self.0 / 26) - 1) as u8, b'A' + (self.0 % 26) as u8];
            s.push_str(from_utf8(&bvec).unwrap());
        } else {
            let mut bvec:[u8; 1] = [b'A' + (self.0 % 26) as u8];
            s.push_str(from_utf8(&bvec).unwrap());
        }
        return s;
    }
}
