pub mod simd;

use rayon::prelude::*;

pub unsafe extern "C" fn vk_compute_world_matrices(
    locals: *const f32,
    worlds: *mut f32,
    count: usize,
) {
    let locals_slice = unsafe { std::slice::from_raw_parts(locals, count * 16) };
    let worlds_slice = unsafe { std::slice::from_raw_parts_mut(worlds, count * 16) };

    worlds_slice
        .par_chunks_exact_mut(16)
        .enumerate()
        .for_each(|(i, world_chunk)| {
            let local_chunk = &locals_slice[i * 16..i * 16 + 16];
            unsafe {
                simd::multiply_matrices_simd(
                    local_chunk.as_ptr(),
                    local_chunk.as_ptr(),
                    world_chunk.as_mut_ptr(),
                );
            }
        });
}

pub unsafe extern "C" fn vk_compute_matrix_vector_muls(
    matrices: *const f32,
    vectors: *const f32,
    outs: *mut f32,
    count: usize,
) {
    let mats_slice = unsafe { std::slice::from_raw_parts(matrices, count * 16) };
    let vecs_slice = unsafe { std::slice::from_raw_parts(vectors, count * 4) };
    let outs_slice = unsafe { std::slice::from_raw_parts_mut(outs, count * 4) };

    outs_slice
        .par_chunks_exact_mut(4)
        .enumerate()
        .for_each(|(i, out_chunk)| {
            let mat_chunk = &mats_slice[i * 16..i * 16 + 16];
            let vec_chunk = &vecs_slice[i * 4..i * 4 + 4];
            unsafe {
                simd::multiply_matrix_vector_simd(
                    mat_chunk.as_ptr(),
                    vec_chunk.as_ptr(),
                    out_chunk.as_mut_ptr(),
                );
            }
        });
}

pub unsafe extern "C" fn vk_add_vectors(
    a_vecs: *const f32,
    b_vecs: *const f32,
    outs: *mut f32,
    count: usize,
) {
    let a_slice = unsafe { std::slice::from_raw_parts(a_vecs, count * 4) };
    let b_slice = unsafe { std::slice::from_raw_parts(b_vecs, count * 4) };
    let outs_slice = unsafe { std::slice::from_raw_parts_mut(outs, count * 4) };

    outs_slice
        .par_chunks_exact_mut(4)
        .enumerate()
        .for_each(|(i, out_chunk)| {
            let a_chunk = &a_slice[i * 4..i * 4 + 4];
            let b_chunk = &b_slice[i * 4..i * 4 + 4];
            unsafe {
                simd::add_vectors_simd(a_chunk.as_ptr(), b_chunk.as_ptr(), out_chunk.as_mut_ptr());
            }
        });
}

pub unsafe extern "C" fn vk_dot_products(
    a_vecs: *const f32,
    b_vecs: *const f32,
    outs: *mut f32,
    count: usize,
) {
    let a_slice = unsafe { std::slice::from_raw_parts(a_vecs, count * 4) };
    let b_slice = unsafe { std::slice::from_raw_parts(b_vecs, count * 4) };
    let outs_slice = unsafe { std::slice::from_raw_parts_mut(outs, count) };

    // par_iter over elements instead of chunks because output is 1 f32 per vector
    outs_slice
        .par_iter_mut()
        .enumerate()
        .for_each(|(i, out_val)| {
            let a_chunk = &a_slice[i * 4..i * 4 + 4];
            let b_chunk = &b_slice[i * 4..i * 4 + 4];
            unsafe {
                *out_val = simd::dot_product_simd(a_chunk.as_ptr(), b_chunk.as_ptr());
            }
        });
}