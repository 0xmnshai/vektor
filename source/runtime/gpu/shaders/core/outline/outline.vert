#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 u_viewportSize;
uniform float offset = 0.05; // Legacy base offset

void main() {
    // 1. Transform position and normal
    vec4 pos_clip = projection * view * model * vec4(aPos, 1.0);
    
    // 2. Expansion direction
    // Hybrid approach: Use normal, but augment with radial direction to handle planes/origins
    vec3 local_dir = aNormal;
    if (length(aNormal) < 0.1) local_dir = normalize(aPos); // Fallback for meshes without normals
    
    // Smooth out the plane: add a bit of radial expansion to the vertex normal
    // This handles cases where the normal is perpendicular to the view
    vec3 expand_dir = normalize(local_dir + normalize(aPos) * 0.2);
    
    // Transform to view space, then project to get screen-space direction
    vec3 normal_view = mat3(view * model) * expand_dir;
    vec2 normal_clip = mat2(projection) * normal_view.xy;
    
    // 3. Constant thickness (4 pixels) in NDC space
    // 4.0 pixels is best for high-DPI screens
    float thickness = 4.0; 
    if (length(normal_clip) > 0.0001) {
        vec2 screen_offset = normalize(normal_clip) * (thickness / u_viewportSize) * 2.0 * pos_clip.w;
        pos_clip.xy += screen_offset;
    }

    // 4. Z-bias: Push slightly towards camera
    pos_clip.z -= 0.0005 * pos_clip.w;

    gl_Position = pos_clip;
}