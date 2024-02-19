#version 310 es
precision highp float;
precision highp int;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(location = SV_Target0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec4 p_left;
    vec4 p_right;
    float p_alpha;
};


#ifndef saturate
#define saturate(v) clamp(v, 0.0, 1.0)
#endif

void main()
{
    vec4 c = texture(u_tex0, v_texCoord).rgba;
    vec4 n;
    vec4 m = vec4( 0.0, 0.0, 0.0, 0.0 ) ;

    vec3 fragRGB = c.rgb; // current color

    // convert to non-PMA
    fragRGB = saturate(fragRGB / c.a);

    //if(c.a!=0.0)
    {
        m.g = 1.0 - fragRGB.g ;
        m.a = c.a;

        n = vec4(
            (p_right.r*fragRGB.g)+(p_left.r*m.g),
            (p_right.g*fragRGB.g)+(p_left.g*m.g),
            (p_right.b*fragRGB.g)+(p_left.b*m.g), c.a * p_alpha) ;
    }

    fragRGB = n.rgb * n.a; // Premultiply alpha

    FragColor = vec4(fragRGB.rgb, n.a);
}
