uniform vec4 p_left;
uniform vec4 p_right  ;
uniform float p_alpha  ;

#ifndef saturate
#define saturate(v) clamp(v, 0.0, 1.0)
#endif

void main()
{
    vec4 c = texture2D(u_texture, cc_FragTexCoord1).rgba;
    vec4 n;
    vec4 m = vec4( 0.0, 0.0, 0.0, 0.0 ) ;
    
    vec3 fragRGB = c.rgb; // current color

    // convert to non-PMA
    fragRGB = saturate(fragRGB / c.a);

    if(c.a!=0.0)
    {
        m.g = 1.0 - fragRGB.g ;
        m.a = c.a;
            
        n = vec4(
            (p_right.r*fragRGB.g)+(p_left.r*m.g),
            (p_right.g*fragRGB.g)+(p_left.g*m.g),
            (p_right.b*fragRGB.g)+(p_left.b*m.g), c.a * p_alpha) ;
    }

    fragRGB = n.rgb * n.a; // Premultiply alpha

    gl_FragColor = vec4(fragRGB.rgb, n.a);

}
