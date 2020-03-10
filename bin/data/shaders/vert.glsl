#version 150 core

// these are for the programmable pipeline system and are passed in
// by default from OpenFrameworks
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;

in vec4 position;
in vec4 color;
in vec3 normal;
in vec2 texcoord;
// this is the end of the default functionality

// this is something we're creating for this shader
out vec2 vTexCoord;

// this is coming from our C++ code
uniform float mouseX;

out vec4 vFragColorVs;

vec4 pos;

/*out VertexData {
 vec2 texCoord;
 vec3 normal;
 } VertexOut;*/

void main()
{
    // here we move the texture coordinates
    //varyingtexcoord = vec2(texcoord.x, texcoord.y);
    //varyingtexcoord = texcoord;
    
    /*VertexOut.texCoord = texCoord;
     VertexOut.normal = normal;*/
    
    
    pos = vec4(-position.x, -position.y, position.z, 0);
    //pos += 5;
    
    vTexCoord = texcoord;
    
    vFragColorVs = vec4(1.0, 1.0, 1.0, 1.0);
    
    // send the vertices to the fragment shader
    //gl_Position = modelViewProjectionMatrix * position;
    
    gl_Position = pos * modelViewProjectionMatrix;
}
