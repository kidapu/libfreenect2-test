uniform sampler2DRect tex;

uniform float KinectMinDist;
uniform float KinectMaxDist;

void main()
{
  vec4 col = texture2DRect(tex, gl_TexCoord[0].xy);

    float value = col.r;
    float low1 = KinectMinDist;
    float high1 = KinectMaxDist;
    float low2 = 1.0;
    float high2 = 0.0;
    float d = clamp(low2 + (value - low1) * (high2 - low2) / (high1 - low1), 0.0, 1.0);
    if (d == 1.0) 
    {
        d = 0.0;
    }
    
    gl_FragColor = vec4(vec3(d), 1.0);
}
