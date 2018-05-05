#version 150 core

/*layout(triangles) in;*/
layout(lines) in;
layout(triangle_strip, max_vertices = 20) out;

// these are for the programmable pipeline system and are passed in
// by default from OpenFrameworks
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;

uniform float thickness;
uniform vec3 lightDir;

in vec2 vTexCoord[];
out vec2 texCoord;

uniform float jitterScale;

in vec4 vFragColorVs[];
out vec4 vFragColor;

/*in VertexData {
 vec2 texCoord;
 vec3 normal;
 } VertexIn[];
 
 out VertexData {
 vec2 texCoord;
 vec3 normal;
 } VertexOut;*/

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
                 43758.5453123);
}

void main()
{
    //texcoord = varyingtexcoord[0];
    
    //thickness = 20;
    
    vec3 up = vec3(0, 0, -1);	// arbitrary up vector
    
    vec3 p0 = vec3(gl_in[0].gl_Position.xyz);
    vec3 p1 = vec3(gl_in[1].gl_Position.xyz);
    //vec3 p2 = vec3(gl_in[2].gl_Position.xyz);
    
    vec3 dir = normalize(p1 - p0);			// normalized direction vector from p0 to p1
    vec3 right = normalize(cross(dir, up));	// right vector
    vec3 norm = cross(right, dir);
    float fColMult = abs(dot(norm, lightDir));
    vec4 colMult = vec4(fColMult, fColMult, fColMult, 3.0);
    
    right += thickness;
    
    
    
    p0 += random(p0.xy) * jitterScale;
    p1 += random(p1.xz) * jitterScale;
    //p2 += random(p2.yz) * jitterScale;
    
    
    
    
    texCoord = vTexCoord[0];
    
    //gl_Position = gl_in[0].gl_Position + vec4(-100.0, 100.0, 100.0, 1.0);
    gl_Position = modelViewProjectionMatrix * vec4(p0 - right, 1.0);
    vFragColor = vFragColorVs[0];
    //gl_FrontColor = gl_FrontColorIn[0] * colMult;
    //VertexOut.normal = gl_in[0].normal;
    //VertexOut.texCoord = gl_in[0].texCoord;
    EmitVertex();
    
    //gl_Position = gl_in[0].gl_Position + vec4(100.0, 100.0, 100.0, 1.0);
    gl_Position = modelViewProjectionMatrix * vec4(p0 + right, 1.0);
    vFragColor = vFragColorVs[0];
    //gl_FrontColor = gl_FrontColorIn[0] * colMult;
    //VertexOut.normal = gl_in[0].normal;
    //VertexOut.texCoord = gl_in[0].texCoord;
    EmitVertex();
    
    //gl_Position = gl_in[0].gl_Position + vec4(100.0, -100.0, 100.0, 1.0);
    gl_Position = modelViewProjectionMatrix * vec4(p1 - right, 1.0);
    vFragColor = vFragColorVs[0];
    //gl_FrontColor = gl_FrontColorIn[1] * colMult;
    //VertexOut.normal = gl_in[1].normal;
    //VertexOut.texCoord = gl_in[1].texCoord;
    EmitVertex();
    
    //gl_Position = gl_in[0].gl_Position + vec4(-100.0, -100.0, 100.0, 0.0);
    gl_Position = modelViewProjectionMatrix * vec4(p1 + right, 1.0);
    vFragColor = vFragColorVs[0];
    //gl_FrontColor = gl_FrontColorIn[1] * colMult;
    //VertexOut.normal = gl_in[1].normal;
    //VertexOut.texCoord = gl_in[1].texCoord;
    EmitVertex();
    
    /*gl_Position = modelViewProjectionMatrix * vec4(p2 - right, 1.0);
    EmitVertex();
    
    gl_Position = modelViewProjectionMatrix * vec4(p2 + right, 1.0);
    EmitVertex();*/
    
    EndPrimitive();
    
    /*
     gl_Position = gl_ModelViewProjectionMatrix * vec4(p0 - right, 1.0);
     gl_FrontColor = gl_FrontColorIn[0] * colMult;
     //gl_FrontColor = color * colMult;
     EmitVertex();
     
     gl_Position = gl_ModelViewProjectionMatrix * vec4(p0 + right, 1.0);
     gl_FrontColor = gl_FrontColorIn[0] * colMult;
     //gl_FrontColor = color * colMult;
     EmitVertex();
     
     gl_Position = gl_ModelViewProjectionMatrix * vec4(p1 - right, 1.0);
     gl_FrontColor = gl_FrontColorIn[1] * colMult;
     //gl_FrontColor = color * colMult;
     EmitVertex();
     
     gl_Position = gl_ModelViewProjectionMatrix * vec4(p1 + right, 1.0);
     gl_FrontColor = gl_FrontColorIn[1] * colMult;
     //gl_FrontColor = color * colMult;
     EmitVertex();
     */
}
