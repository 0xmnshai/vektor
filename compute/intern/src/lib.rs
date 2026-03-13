#[cxx::bridge]
mod intern_ffi {
    // Math Functions
    extern "Rust" {
        fn compute_matrices_rs(locals: &[f32], worlds: &mut [f32], count: usize);
        fn compute_matrix_vector_muls_rs(
            matrices: &[f32],
            vectors: &[f32],
            outs: &mut [f32],
            count: usize,
        );
        fn add_vectors_rs(a_vecs: &[f32], b_vecs: &[f32], outs: &mut [f32], count: usize);
        fn dot_products_rs(a_vecs: &[f32], b_vecs: &[f32], outs: &mut [f32], count: usize);
    }
}

pub fn compute_matrices_rs(locals: &[f32], worlds: &mut [f32], count: usize) {
    unsafe {
        math_accel::vk_compute_world_matrices(locals.as_ptr(), worlds.as_mut_ptr(), count);
    }
}

pub fn compute_matrix_vector_muls_rs(
    matrices: &[f32],
    vectors: &[f32],
    outs: &mut [f32],
    count: usize,
) {
    unsafe {
        math_accel::vk_compute_matrix_vector_muls(
            matrices.as_ptr(),
            vectors.as_ptr(),
            outs.as_mut_ptr(),
            count,
        );
    }
}

pub fn add_vectors_rs(a_vecs: &[f32], b_vecs: &[f32], outs: &mut [f32], count: usize) {
    unsafe {
        math_accel::vk_add_vectors(a_vecs.as_ptr(), b_vecs.as_ptr(), outs.as_mut_ptr(), count);
    }
}

pub fn dot_products_rs(a_vecs: &[f32], b_vecs: &[f32], outs: &mut [f32], count: usize) {
    unsafe {
        math_accel::vk_dot_products(a_vecs.as_ptr(), b_vecs.as_ptr(), outs.as_mut_ptr(), count);
    }
}
