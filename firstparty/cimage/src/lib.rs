use std::ffi::{c_char, CStr};

use image::GenericImageView;

#[repr(C)]
pub struct cimage_rgba8 {
    pub r: u8,
    pub g: u8,
    pub b: u8,
    pub a: u8,
}

#[repr(C)]
pub struct cimage_image {
    width: u32,
    height: u32,
    pixels: *mut cimage_rgba8,
}

#[no_mangle]
pub unsafe extern "C" fn cimage_image_decode_from_file(path: *const c_char) -> *mut cimage_image {
    let path = unsafe { CStr::from_ptr(path) };
    let path = path.to_str().unwrap();

    let image = image::open(path).unwrap();

    let (width, height) = image.dimensions();
    let pixels = image
        .flipv()
        .into_rgba8()
        .pixels()
        .map(|pixel| {
            let [r, g, b, a] = pixel.0;

            cimage_rgba8 { r, g, b, a }
        })
        .collect::<Vec<_>>()
        .leak()
        .as_mut_ptr();

    Box::leak(Box::new(cimage_image {
        width,
        height,
        pixels,
    })) as *mut _
}

#[no_mangle]
pub unsafe extern "C" fn cimage_image_free(image: *mut cimage_image) {
    let image = unsafe { Box::from_raw(image) };

    drop(Vec::from_raw_parts(
        image.pixels,
        (image.width * image.height) as usize,
        (image.width * image.height) as usize,
    ));
}
