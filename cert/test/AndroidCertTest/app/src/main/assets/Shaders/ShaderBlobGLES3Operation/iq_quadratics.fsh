#version 300 es

//-------------------------------------------------------
// Source: https://www.shadertoy.com/view/tdKSRR

// The MIT License
// Copyright © 2019 Inigo Quilez
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions: The above copyright
// notice and this permission notice shall be included in all copies or
// substantial portions of the Software. THE SOFTWARE IS PROVIDED "AS IS",
// WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// Something I toyed with a decade ago was mapping all possible
// quadratic polynomials to the surface of a sphere:
//
// http://www.iquilezles.org/blog/?p=1089
//
// All possible quadratic equations (parabolas) can be mapped to
// the surface of a sphere. p(x)=ax²+bx+c becomes a point
// (a,b,c) in 3D space, and since all quadratics of the form
// (k·a, k·b, k·c) has the same solutions, all space can be
// collapsed into a unit sphere through vector normalization.
//
// In blue are complex solutions.
// In yellow are real possitive solutions
// In purple are real negative solutions

//-------------------------------------------------------

precision mediump float;

uniform vec2 uResolution;
uniform float uTime;

out vec4 frag_color;

//------------------------------------------------------------------

// For a point in the sphere's surface p, return a color based on
// the solutions of the associate quadratic polynomial
vec3 getColor( in vec3 p )
{
    // rotate the solution space (the sphere mapping)
    float an = 0.5*uTime;
    float si = sin(an), co = cos(an);
    p.xz = mat2(co,-si,si,co)*p.xz;

    vec3 col = vec3(0.0);
    float m = 0.0;

    // solve quadratic
    float h = p.y*p.y - 4.0*p.x*p.z;
    if( h<0.0 )
    {
        // Complex solution. Make it blue
        m = length(vec2(-p.y,sqrt(-h)))*0.5/abs(p.x);
        col = vec3(0.0,0.5,1.0);
    }
    else
    {
        // Real solution. Yellow if possitive and purple if negative
        float t = (-p.y-sqrt(h))*0.5/p.x;
        col = (t>0.0) ? vec3(1.0,0.5,0.0) : vec3(1.0,0.0,0.5);
        m = abs(t);
    }

    // shader base color based on log of root size
    col *= clamp(log(1.0+m),0.0,1.0); // brightness modulation

    // discriminant isolines
    h = pow(abs(h),1.0/4.0);
    col *= 0.7 + 0.3*smoothstep(-0.1,0.1,sin(abs(120.0*h)));

    return col;
}

//-------------------------------------------------------

float sphIntersect( in vec3 ro, in vec3 rd, in vec4 sph )
{
    vec3 oc = ro - sph.xyz;
    float b = dot( oc, rd );
    float c = dot( oc, oc ) - sph.w*sph.w;
    float h = b*b - c;
    if( h<0.0 ) return -1.0;
    return -b - sqrt( h );
}

float sphSoftShadow( in vec3 ro, in vec3 rd, in vec4 sph, in float k )
{
    vec3 oc = ro - sph.xyz;
    float b = dot( oc, rd );
    float c = dot( oc, oc ) - sph.w*sph.w;
    float h = b*b - c;
    return (b>0.0) ? step(-0.0001,c) : smoothstep( 0.0, 1.0, h*k/b );
}

float sphOcclusion( in vec3 pos, in vec3 nor, in vec4 sph )
{
    vec3  r = sph.xyz - pos;
    float l = length(r);
    return dot(nor,r)*(sph.w*sph.w)/(l*l*l);
}

vec3 sphNormal( in vec3 pos, in vec4 sph )
{
    return normalize(pos-sph.xyz);
}

float iPlane( in vec3 ro, in vec3 rd )
{
    return (-1.0 - ro.y)/rd.y;
}

//=====================================================

vec3 plot2D( in vec2 px )
{
    vec2 p = px/uResolution.xy;

    vec2 a = p.yx*vec2(3.141593, 6.283185);

    vec3 q = vec3( cos(a.x),
    sin(a.x)*cos(a.y),
    sin(a.x)*sin(a.y) );

    return getColor(q);
}

//=====================================================

vec3 plot3D( in vec2 px )
{
    vec2 p = (-uResolution.xy + 2.0*px)/uResolution.y;

    // camera
    vec3 ro = vec3(0.0, 0.0, 3.0 );
    vec3 rd = normalize( vec3(p,-2.0) );

    // sphere
    vec4 sph = vec4( 0.0, 0.0, 0.0, 1.0 );

    vec3 col = vec3(0.0);

    // intersect geometry
    float tmin = 1e10;
    vec3 nor;
    float occ = 1.0;
    vec3 mate = vec3(1.0);

    // plane/floor
    float t1 = iPlane( ro, rd );
    if( t1>0.0 )
    {
        tmin = t1;
        vec3 pos = ro + t1*rd;
        nor = vec3(0.0,1.0,0.0);
        occ = 1.0-sphOcclusion( pos, nor, sph );
        mate = vec3(0.2);
    }

    // sphere
    float t2 = sphIntersect( ro, rd, sph );
    if( t2>0.0 && t2<tmin )
    {
        tmin = t2;
        vec3 pos = ro + t2*rd;
        nor = sphNormal( pos, sph );
        occ = 0.5 + 0.5*nor.y;
        mate = getColor(nor);
    }

    // apply color and lighting
    if( tmin<1000.0 )
    {
        vec3 pos = ro + tmin*rd;

        vec3 lig = normalize( vec3(0.6,0.3,0.4) );
        float sha = sphSoftShadow( pos, lig, sph, 2.0 );

        vec3 lin = vec3(1.5)*clamp(dot(nor,lig),0.0,1.0)*sha;
        lin += 0.5*occ;
        lin += 0.5*occ*pow(clamp(1.0+dot(nor,rd),0.0,1.0),3.0);

        col = mate*lin;
        // fog
        col *= exp( -0.05*tmin );
    }
    return col;
}

void main()
{
    vec2 fragCoord = gl_FragCoord.xy;

    vec3 tot = vec3(0.0);

    int AA = 1;
    for( int n=0; n<AA; n++ )
    {
        // pixel coordinates
        vec2 p = fragCoord;

        // draw stuff
        vec3 col = (sin(0.5*uTime)<-0.5) ? plot2D(p) : plot3D(p);

        // gamma correction
        col = pow(col,vec3(0.4545));

        tot += col;
    }
    tot /= float(AA*AA);

    // cheap dithering
    tot += sin(fragCoord.x*114.0)*sin(fragCoord.y*211.1)/512.0;

    // output color
    frag_color = vec4( tot, 1.0 );
}
