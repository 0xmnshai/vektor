fn main() {
    cxx_build::bridge("src/lib.rs")
        .compile("intern_bridge");
}
