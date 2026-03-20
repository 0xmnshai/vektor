use wide::f32x4;

pub unsafe fn multiply_matrices_simd(a: *const f32, b: *const f32, out: *mut f32) {
    unsafe {
        let a_slice = std::slice::from_raw_parts(a, 16);
        let b_slice = std::slice::from_raw_parts(b, 16);
        let out_slice = std::slice::from_raw_parts_mut(out, 16);

        // A 4x4 matrix is essentially four 4-float vectors (columns).
        // We load them into 128-bit hardware registers directly.
        let a_col0 = f32x4::from([a_slice[0], a_slice[1], a_slice[2], a_slice[3]]);
        let a_col1 = f32x4::from([a_slice[4], a_slice[5], a_slice[6], a_slice[7]]);
        let a_col2 = f32x4::from([a_slice[8], a_slice[9], a_slice[10], a_slice[11]]);
        let a_col3 = f32x4::from([a_slice[12], a_slice[13], a_slice[14], a_slice[15]]);

        // Process dot-products at the hardware level.
        for i in 0..4 {
            let b_val0 = f32x4::splat(b_slice[i * 4 + 0]);
            let b_val1 = f32x4::splat(b_slice[i * 4 + 1]);
            let b_val2 = f32x4::splat(b_slice[i * 4 + 2]);
            let b_val3 = f32x4::splat(b_slice[i * 4 + 3]);

            let res = (a_col0 * b_val0) + (a_col1 * b_val1) + (a_col2 * b_val2) + (a_col3 * b_val3);

            // Store back into memory
            let arr_res = res.to_array();
            out_slice[i * 4 + 0] = arr_res[0];
            out_slice[i * 4 + 1] = arr_res[1];
            out_slice[i * 4 + 2] = arr_res[2];
            out_slice[i * 4 + 3] = arr_res[3];
        }
    }
}

pub unsafe fn multiply_matrix_vector_simd(mat: *const f32, vec: *const f32, out: *mut f32) {
    unsafe {
        let mat_slice = std::slice::from_raw_parts(mat, 16);
        let vec_slice = std::slice::from_raw_parts(vec, 4);
        let out_slice = std::slice::from_raw_parts_mut(out, 4);

        let col0 = f32x4::from([mat_slice[0], mat_slice[1], mat_slice[2], mat_slice[3]]);
        let col1 = f32x4::from([mat_slice[4], mat_slice[5], mat_slice[6], mat_slice[7]]);
        let col2 = f32x4::from([mat_slice[8], mat_slice[9], mat_slice[10], mat_slice[11]]);
        let col3 = f32x4::from([mat_slice[12], mat_slice[13], mat_slice[14], mat_slice[15]]);

        let vx = f32x4::splat(vec_slice[0]);
        let vy = f32x4::splat(vec_slice[1]);
        let vz = f32x4::splat(vec_slice[2]);
        let vw = f32x4::splat(vec_slice[3]);

        let res = (col0 * vx) + (col1 * vy) + (col2 * vz) + (col3 * vw);
        let arr_res = res.to_array();

        out_slice[0] = arr_res[0];
        out_slice[1] = arr_res[1];
        out_slice[2] = arr_res[2];
        out_slice[3] = arr_res[3];
    }
}

pub unsafe fn add_vectors_simd(a: *const f32, b: *const f32, out: *mut f32) {
    unsafe {
        let a_slice = std::slice::from_raw_parts(a, 4);
        let b_slice = std::slice::from_raw_parts(b, 4);
        let out_slice = std::slice::from_raw_parts_mut(out, 4);

        let va = f32x4::from([a_slice[0], a_slice[1], a_slice[2], a_slice[3]]);
        let vb = f32x4::from([b_slice[0], b_slice[1], b_slice[2], b_slice[3]]);

        let res = va + vb;
        let arr_res = res.to_array();
        out_slice[0] = arr_res[0];
        out_slice[1] = arr_res[1];
        out_slice[2] = arr_res[2];
        out_slice[3] = arr_res[3];
    }
}

pub unsafe fn dot_product_simd(a: *const f32, b: *const f32) -> f32 {
    unsafe {
        let a_slice = std::slice::from_raw_parts(a, 4);
        let b_slice = std::slice::from_raw_parts(b, 4);

        let va = f32x4::from([a_slice[0], a_slice[1], a_slice[2], a_slice[3]]);
        let vb = f32x4::from([b_slice[0], b_slice[1], b_slice[2], b_slice[3]]);

        let mul = va * vb;
        let arr = mul.to_array();
        arr[0] + arr[1] + arr[2] + arr[3]
    }
}