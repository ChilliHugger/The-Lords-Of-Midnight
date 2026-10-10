#version 310 es
precision highp float;
precision highp int;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(location = SV_Target0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec4 p_colour;      // flat colour of the outline
    vec4 p_uvRect;      // uv bounds (minU, minV, maxU, maxV) of the source frame within the texture
    vec2 p_radius;      // outline radius in uv units
    float p_alpha;      // overall alpha
    float p_power;      // fade curve, 1 = linear, >1 = tighter to the sprite
};

#define RINGS 16
#define DIRECTIONS 24
#define TWO_PI 6.28318530718

// alpha of the source frame at uv, anything outside the frame is transparent
// so neighbouring frames in a texture atlas never bleed into the outline
float frameAlpha(vec2 uv)
{
    if (uv.x < p_uvRect.x || uv.y < p_uvRect.y || uv.x > p_uvRect.z || uv.y > p_uvRect.w) {
        return 0.0;
    }
    // ignore near-invisible stray pixels, otherwise each one grows its own glow
    return smoothstep(0.1, 0.4, texture(u_tex0, uv).a);
}

void main()
{
    // Distance based glow: the strongest nearby pixel, weighted by how far away it is.
    // Unlike an average this doesn't dilute as the radius grows. Many closely spaced
    // rings with an eased falloff keep the steps between rings invisible.
    float a = frameAlpha(v_texCoord);

    // rotate the sample pattern a random amount per pixel so any banding from the
    // fixed sample lattice becomes fine grain instead of visible hatching
    float jitter = fract(sin(dot(v_texCoord, vec2(12.9898, 78.233)) * 4096.0) * 43758.5453);

    for (int r = 1; r <= RINGS; r++) {
        float t = float(r) / float(RINGS);
        float weight = pow(1.0 - smoothstep(0.0, 1.0, t), p_power);
        vec2 offset = p_radius * t;

        // stagger every other ring so the samples don't line up radially
        float phase = ((r % 2 == 0) ? 0.5 : 0.0) + jitter;

        for (int d = 0; d < DIRECTIONS; d++) {
            float angle = TWO_PI * (float(d) + phase) / float(DIRECTIONS);
            vec2 uv = v_texCoord + vec2(cos(angle), sin(angle)) * offset;
            a = max(a, frameAlpha(uv) * weight);
        }
    }

    a *= p_alpha * p_colour.a;

    // premultiplied alpha
    FragColor = vec4(p_colour.rgb * a, a);
}
