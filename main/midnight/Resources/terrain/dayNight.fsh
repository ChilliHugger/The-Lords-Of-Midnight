uniform vec4 p_left;
uniform vec4 p_right  ;
uniform float p_alpha  ;

void main()
{
    vec4 c = texture2D(u_texture, cc_FragTexCoord1).rgba;
    vec4 n;
    vec4 m = vec4( 0.0, 0.0, 0.0, 0.0 ) ;
    
    //if ( c.a > 0.0 ) {
    
        if ( c.g == 0.0  )
            n = vec4(p_left.r, p_left.g, p_left.b, c.a) ;
        else if ( c.g == 1.0 )
            n = vec4(p_right.r, p_right.g, p_right.b, c.a) ;
        else {
            m.r = 1.0 - c.r ;
            m.g = 1.0 - c.g ;
            m.b = 1.0 - c.b ;
            m.a = c.a;
            
            n = vec4((p_right.r*c.r)+(p_left.r*m.r), (p_right.g*c.g)+(p_left.g*m.g), (p_right.b*c.b)+(p_left.b*m.b), m.a) ;
        }
    //}else{
   //     n = c; //vec4(1,1,1,0.5) ;
   //}
    
    n.a = n.a * p_alpha;
   
    gl_FragColor = n ;
    
}
