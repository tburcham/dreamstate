#version 150 core

//////////////////////////////////////////////////////////////////////
//                                                                  //
//                                                                  //
//  Code developed by Javier Villaroel,                             //
//                  Madrid, Spain, March 2015                       //
//                  javier@ultra-lab.net                            //
//                  www.ultra-lab.net                               //
//                                                                  //
//////////////////////////////////////////////////////////////////////

#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2DRect tex0;

//varying vec2 texCoordVarying;
//varying vec2 VTexCoord[];

in vec2 texCoord;
out vec4 outputColor;

in vec4 vFragColor;

uniform float time;
uniform vec2 mouse;
uniform vec2 resolution;


void main(void)
{
    /*vec2 center = resolution/2.0;
    
    vec2 pos = texCoord.st;
    float pi = 3.1415;
    float R;
    vec2 r = pos - center;
    float mr = length(r);
    vec2 posM;
    float angT = time; //10.0*time;
    
    float expansionT = 50000000.0;
    float giroT = 20.0;
    float radioMax = 10000.0;
    
    
    
    if (time < expansionT){
        
        R = radioMax - time*(radioMax - 0.5*resolution.y)/expansionT;
    } else {
        if ( time < giroT + expansionT){
            R = resolution.y * 0.5;
            angT =  0.5 * resolution.y * sin(2.0*pi*(time - expansionT)/giroT);
        } else {
            R =  -(radioMax - 0.5*resolution.y)*(giroT+expansionT)/expansionT
            + time*(radioMax - 0.5*resolution.y)/expansionT
            + resolution.y*0.5;
        }
    }
    
    
    if(length(r) < R)
    {
        
        float theta = acos(-r.y/R)/pi;  //angulo con respecto a los polos
        float phi = abs(atan(sqrt(abs(R*R - r.x*r.x - r.y*r.y))/r.x))/pi; //angulo en el plano XY de la esfera
        
        float ajusteP = resolution.y / 2.0 + resolution.x / 2.0 - angT;
        float ajusteN = resolution.x / 2.0 - resolution.y / 2.0 - angT;
        
        if(R <= resolution.y / 2.0)
        {
            posM.y = resolution.y * theta;
            posM.x = resolution.y * phi;
        }
        if(R > resolution.y / 2.0 )
        {
            float alpha = acos(resolution.y/(2.0*R))/pi;
            float A = resolution.y /(1.0-(2.0*alpha));
            
            float beta = acos(resolution.y/(2.0*R))/pi;
            float B = resolution.y /(1.0-(2.0*beta));
            
            posM.y = A * (theta - alpha);
            posM.x = B * (phi - beta);
        }
        
        if(r.x > 0.0){
            posM.x =  ajusteP - posM.x;
        } else {
            posM.x += ajusteN;
        }
    
        
        posM.x /= 10;
        posM.y /= 10;
        
        if (posM.x <= 0.0 || posM.y <= 0.0 || posM.y > resolution.y || posM.x > resolution.x){

            
            discard;
        } else {*/

            /*vec4 color = texture(tex0, posM) * vFragColor;
            color.a = 0.8;*/

            //vec4 color = texture(tex0, texCoord) * vFragColor;
    
            //vec4 color = texture(tex0, gl_FragCoord.xy);
    
            //color.a = 125;
            //outputColor = color;
            //outputColor = vec4(1.0, 1.0, 1.0, 0.5);
            //gl_FragColor = gl_Color;
        /*}
        
        
    } else {
        
        discard;
        
    }*/
    
    /*vec4 col = texture2DRect(tex0, gl_TexCoord[0].xy);
    float value = col.r;
    float low1 = 500.0;
    float high1 = 5000.0;
    float low2 = 1.0;
    float high2 = 0.0;
    float d = clamp(low2 + (value - low1) * (high2 - low2) / (high1 - low1), 0.0, 1.0);
    if (d == 1.0) {
        d = 0.0;
    }
    gl_FragColor = col; //vec4(vec3(d), 1.0);*/
    
    //gl_FragColor = gl_Color;
    
    outputColor = vec4(1.0, 1.0, 0.0, 0.5);
    
    vec4 color = texture(tex0, texCoord) * vFragColor;
    outputColor = color;
    
    
}

void main__(void)
{
    
    outputColor = vec4(1.0, 0.0, 0.0, 1.0);
}
