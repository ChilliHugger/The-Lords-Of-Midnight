#version 310 es
precision highp float;
precision highp int;

layout(location = COLOR0) in vec4 v_color;
layout(location = TEXCOORD0) in vec2 v_texCoord;

layout(location = SV_Target0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D u_tex0;

layout(std140) uniform fs_ub {
    vec4 p_colour;
    float p_alpha;
    float p_fade;
};

#ifndef saturate
#define saturate(v) clamp(v, 0.0, 1.0)
#endif

void main()
{
    vec4 c = texture(u_tex0, v_texCoord).rgba;
    vec4 n;
    vec4 b = vec4( 0.0, 0.0, 0.0, 0.0 ) ;
    
    vec3 fragRGB = c.rgb; // current color

    // convert to non-PMA
    fragRGB = saturate(fragRGB / c.a);
    
    if(c.a!=0.0)
    {
        b.a = 1.0 - (c.a*p_fade) ;
        b.r = (p_colour.r * b.a) ;
        b.g = (p_colour.g * b.a) ;
        b.b = (p_colour.b * b.a) ;
        
        n.r = b.r + (fragRGB.r*(c.a*p_fade)) ;
        n.g = b.g + (fragRGB.g*(c.a*p_fade)) ;
        n.b = b.b + (fragRGB.b*(c.a*p_fade)) ;
    }
    
    n.a = c.a * p_alpha;

    fragRGB = n.rgb * n.a; // Premultiply alpha
    
    FragColor = vec4(fragRGB.rgb, n.a);
}
