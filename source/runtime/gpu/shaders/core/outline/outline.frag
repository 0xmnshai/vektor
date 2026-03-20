#version 410 core

out vec4 FragColor;

uniform float u_time;

void main() {
    // High-quality Pulse effect
    float pulse = 0.5 + 0.5 * sin(u_time * 5.0);
    vec3 base_color = vec3(1.0, 0.5, 0.0); // Orange
    vec3 glow_color = vec3(1.0, 0.8, 0.3); // Yellowish-white
    
    vec3 final_color = mix(base_color, glow_color, pulse * 0.4);
    FragColor = vec4(final_color, 1.0);
}
