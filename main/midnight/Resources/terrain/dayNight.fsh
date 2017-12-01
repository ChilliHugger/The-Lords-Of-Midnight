#ifdef GL_ES
precision mediump float;
#endif

varying vec2 v_texCoord;

vec4 m = vec4( 0.0, 0.0, 0.0, 0.0 ) ;
uniform vec4 p_left;
uniform vec4 p_right  ;
uniform float p_alpha  ;

void main()
{
    vec4 c = texture2D(CC_Texture0, v_texCoord).rgba;
    vec4 n;
    
    //if ( c.a > 0.0 ) {
    
        if ( c.g == 0.0  )
            n = vec4(p_left.r, p_left.g, p_left.b, c.a) ;
        else if ( c.g == 1.0 )
            n = vec4(p_right.r, p_right.g, p_right.b, c.a) ;
        else {
            m.r = 1.0 - c.r ;
            m.g = 1.0 - c.g ;
            m.b = 1.0 - c.b ;
            
            
            n = vec4((p_right.r*c.r)+(p_left.r*m.r), (p_right.g*c.g)+(p_left.g*m.g), (p_right.b*c.b)+(p_left.b*m.b), c.a) ;
        }
    //}else{
   //     n = c; //vec4(1,1,1,0.5) ;
   //}
    
    n.a = n.a * p_alpha;
   
    gl_FragColor = n ;
    
}
