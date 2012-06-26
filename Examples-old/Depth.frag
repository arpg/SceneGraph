// see http://olivers.posterous.com/linear-depth-in-glsl-for-real


// NB we setup our texture using:
// glTexImage2D( GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F, m_nTexWidth, 
//                        m_nTexHeight, 0, GL_RGBA, GL_FLOAT, NULL );
// which means our RGBA texture is actually a float, and it is NOT clamped to 0-1.  Awesome.

varying float depth; // from the vertex shader
void main(void)
{
    float A = gl_ProjectionMatrix[2].z;
    float B = gl_ProjectionMatrix[3].z;
    float zNear = - B / (1.0 - A);
    float zFar  =   B / (1.0 + A);
    float d = 0.5*(-A*depth + B) / depth + 0.5;
//    gl_FragDepth  = d;
//    gl_FragColor = vec4( vec3(d), 1.0);
//    gl_FragColor = vec4( vec3(depth), 1.0);
//    gl_FragColor[0] = d;
//    gl_FragColor = vec4( 1000.10101, 0.0, 0.0, 1.0 );
    gl_FragColor = vec4( vec3(depth), 1.0 ); // not really needed, but texture maps look better
    gl_FragColor[0] = depth; // overwrite all 4 bytes with the unclamped ieee float value for depth.
}

