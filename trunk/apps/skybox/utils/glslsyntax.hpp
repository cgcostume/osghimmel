
// Copyright (c) 2011-2012, Daniel Müller <dm@g4t3.de>
// Computer Graphics Systems Group at the Hasso-Plattner-Institute, Germany
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright 
//     notice, this list of conditions and the following disclaimer in the 
//     documentation and/or other materials provided with the distribution.
//   * Neither the name of the Computer Graphics Systems Group at the 
//     Hasso-Plattner-Institute (HPI), Germany nor the names of its 
//     contributors may be used to endorse or promote products derived from 
//     this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE.

#pragma once
#ifndef __GLSL_SYNTAX_H__
#define __GLSL_SYNTAX_H__

#include <QStringList>


namespace
{
    // From the "The OpenGL Shading Language 1.50 Quick Reference Card"

    const QString glsl_syntax_TransparentTypes = (
        "\\b("
            "void|"
            "bool|"
            "(u)?int|" 
            "float|"
            "(b|i|u)?vec[2-4]|"
            "mat[2-4](x[2-4])?"
        ")\\b");

    const QString glsl_syntax_SamplerTypes = (
        "\\b("
            "(i|u)?sampler[1-3]D|"
            "(i|u)?samplerCube|"
            "(i|u)?sampler2DRect|"
            "sampler2DRectShadow|"
            "sampler[1-2]DShadow|"
            "(i|u)?sampler[1-2]DArray|"
            "sampler[1-2]DArrayShadow|"
            "comparison|"
            "(i|u)?samplerBuffer|"
            "(i|u)?sampler2DMS(Array)?"
        ")\\b");

    const QString glsl_syntax_PreprocessorDirectives = (
        "^(\\s)*#("
            "define|"
            "undef|"
            "if|"
            "ifdef|"
            "ifndef|"
            "else|"
            "elif|"
            "endif|"
            "error|"
            "pragma|"
            "extension|"
            "version|"
            "line"
        ")\\b");

    const QString glsl_syntax_PredefinedMacros = (
        "\\b("
            "__LINE__|"
            "__FILE__|"
            "__VERSION__"
        ")\\b");

    const QString glsl_syntax_Qualifiers = (
        "\\b("
            "const|"
            "(centroid )?(in|out)|"
            "uniform|"
            "inout"
        ")\\b");

    const QString glsl_syntax_InterpolationQualifier = (
        "\\b("
            "smooth|"
            "flat|"
            "noperspective"
        ")\\b");

    const QString glsl_syntax_CompatibilityUniforms = (
        "\\bgl_("
            "ModelViewMatrix|"
            "ModelViewProjectionMatrix|"
            "ProjectionMatrix|"
            "TextureMatrix|"
            "ModelViewMatrixInverse|"
            "ModelViewProjectionMatrixInverse|"
            "ProjectionMatrixInverse|"
            "TextureMatrixInverse|"
            "ModelViewMatrixTranspose|"
            "ModelViewProjectionMatrixTranspose|"
            "ProjectionMatrixTranspose|"
            "TextureMatrixTranspose|"
            "ModelViewMatrixInverseTranspose|"
            "ModelViewProjectionMatrixInverseTranspose|"
            "ProjectionMatrixInverseTranspose|"
            "TextureMatrixInverseTranspose|"
            "NormalMatrix|"
            "NormalScale|"
            "DepthRangeParameters|"
            "FogParameters|"
            "LightSourceParameters|"
            "LightModelParameters|"
            "LightModelProducts|"
            "FrontLightModelProduct|"
            "BackLightModelProduct|"
            "LightProducts|"
            "FrontLightProduct|"
            "BackLightProduct|"
            "MaterialParameters|"
            "FrontMaterial|"
            "BackMaterial|"
            "PointParameters|"
            "Point|"
            "TextureEnvColor|"
            "ClipPlane|"
            "EyePlane(S|T|R|Q)|"
            "ObjectPlane(S|T|R|Q)"
        ")\\b");

    const QString glsl_syntax_BuiltIn_v = (
        "\\bgl_("
            "VertexID|"
            "InstanceID|"
            "Color|"
            "SecondaryColor|"
            "Normal|"
            "Vertex|"
            "MultiTexCoord[0-7]|"
            "FogCoord|"
            "Position|"
            "PointSize|"
            "ClipDistance|"
            "ClipVertex|"
            "Front(Secondary)?Color|"
            "Back(Secondary)?Color|"
            "TexCoord|"
            "FogFragCoord"
        ")\\b");

    const QString glsl_syntax_BuiltIn_g = (
        "\\bgl_("
            "PerVertex|"
            "Position|"
            "PointSize|"
            "ClipDistance|"
            "Primitive(IDIn)?"
        ")\\b");

    const QString glsl_syntax_BuiltIn_f = (
        "\\bgl_("
            "Frag(Color|Coord|Depth)|"
            "FrontFacing|"
            "ClipDistance|"
            "PointCoord|"
            "PrimitiveID"
        ")\\b");

    const QString glsl_syntax_ConstantsBuiltIn = (
        "\\bgl_Max("
            "ClipDistances|"
            "ClipPlanes|"
            "DrawBuffers|"
            "TextureUnits|"
            "TextureCoords|"
            "GeometryTextureImageUnits|"
            "TextureImageUnits|"
            "VertexAttribs|"
            "VertexTextureImageUnits|"
            "CombinedTextureImageUnits|"
            "GeometryVaryingComponents|"
            "VaryingComponents|"
            "VaryingFloats|"
            "GeometryOutputVertices|"
            "FragmentUniformComponents|"
            "GeometryTotalOutputComponents|"
            "GeometryUniformComponents|"
            "VertexUniformComponents"
        ")\\b");

    const QString glsl_syntax_IterationAndJumps = (
        "(^(?!tbd_)(if|else)|"
        "\\b("
            "return|"
            "for|"
            "break|"
            "continue|"
            "while|"
            "do|"
            "switch|"
            "case"
        ")\\b)");

    const QString glsl_syntax_IterationAndJumps_f = (
        "\\b("
            "discard"
        ")\\b");

    const QString glsl_syntax_Functions = (
        "\\b("
            "main"
        ")\\b");

    const QString glsl_syntax_AngleAndTrigonometryFunctions = (
        "\\b("
            "radians|"
            "degrees|"
            "(a)?sin(h)?|"
            "(a)?cos(h)?|"
            "(a)?tan(h)?"
        ")\\b");

    const QString glsl_syntax_ExponentialFunctions = (
        "\\b("
            "pow|"
            "exp(2)?|"
            "log(2)?|"
            "(inverse)?sqrt"
        ")\\b");

    const QString glsl_syntax_CommonFunctions = (
        "\\b("
            "abs|"
            "sign|"
            "floor|"
            "trunc|"
            "round(Even)?|"
            "ceil|"
            "fract|"
            "mod|"
            "modf|"
            "min|"
            "max|"
            "clamp|"
            "mix|"
            "step|"
            "smoothstep|"
            "is(nan|inf)"
        ")\\b");

    const QString glsl_syntax_GeometricFunctions = (
        "\\b("
            "length|"
            "distance|"
            "dot|"
            "cross|"
            "normalize|"
            "ftransform|"
            "faceforward|"
            "reflect|"
            "refract"
        ")\\b");

    const QString glsl_syntax_MatrixFunctions = (
        "\\b("
            "matrixCompMult|"
            "outerProduct|"
            "transpose|"
            "determinant|"
            "inverse"
        ")\\b");

    const QString glsl_syntax_VectorRelationalFunctions = (
        "\\b("
            "lessThan(Equal)?|"
            "greaterThan(Equal)?|"
            "equal|"
            "notEqual|"
            "any|"
            "all|"
            "not"
        ")\\b");


    const QString glsl_syntax_DerivativeFunctions_f = (
        "\\b("
            "dFd(x|y)|"
            "fwidth"
        ")\\b");


    const QString glsl_syntax_NoiseFunctions = (
        "\\b("
            "noise[1-4]"
        ")\\b");


    const QString glsl_syntax_Functions_g = (
        "\\b("
            "EmitVertex|"
            "EndPrimitive"
        ")\\b");


    const QString glsl_syntax_TextureLookupFunctions = (
        "\\b("
            "textureSize|"
            "texture(Offset)?|"
            "textureProj(Offset)?|"
            "textureLod(Offset)?|"
            "texelFetch(Offset)?|"
            "textureProjLod(Offset)?|"
            "textureGrad(Offset)?|"
            "textureProjGrad(Offset)?"
        ")\\b");



    // Strings for completion.

    const QStringList glsl_complete_common = (QStringList()
        // TransparentTypes
        << "void"
        << "bool"
        << "int" 
        << "uint"
        << "float"
        << "vec2"
        << "vec3"
        << "vec4"
        << "bvec2"
        << "bvec3"
        << "bvec4"
        << "ivec2"
        << "ivec3"
        << "ivec4"
        << "uvec2"
        << "uvec3"
        << "uvec4"
        << "mat2"
        << "mat3"
        << "mat4"
        << "mat2x2"
        << "mat2x3"
        << "mat2x4"
        << "mat3x2"
        << "mat3x3"
        << "mat3x4"
        << "mat4x2"
        << "mat4x3"
        << "mat4x4"
        // SamplerTypes    
        << "sampler1D"
        << "sampler2D"
        << "sampler3D"
        << "isampler1D"
        << "isampler2D"
        << "isampler3D"
        << "uampler1D"
        << "usampler2D"
        << "usampler3D"
        << "samplerCube"
        << "isamplerCube"
        << "usamplerCube"
        << "sampler2DRect"
        << "isampler2DRect"
        << "usampler2DRect"
        << "sampler2DRectShadow"
        << "sampler1DShadow"
        << "sampler2DShadow"
        << "sampler1DArray"
        << "sampler2DArray"
        << "isampler1DArray"
        << "isampler2DArray"
        << "usampler1DArray"
        << "usampler2DArray"
        << "sampler1DArrayShadow"
        << "sampler2DArrayShadow"
        << "comparison"
        << "samplerBuffer"
        << "isamplerBuffer"
        << "usamplerBuffer"
        << "sampler2DMS"
        << "isampler2DMS"
        << "usampler2DMS"
        << "sampler2DMSArray"
        << "isampler2DMSArray"
        << "usampler2DMSArray"
        // PreprocessorDirectives    
        << "#define"
        << "#undef"
        << "#if"
        << "#ifdef"
        << "#ifndef"
        << "#else"
        << "#elif"
        << "#endif"
        << "#error"
        << "#pragma"
        << "#extension"
        << "#version"
        << "#line"
        // PredefinedMacros    
        << "__LINE__"
        << "__FILE__"
        << "__VERSION__"
        // Qualifiers
        << "const"
        << "centroid"
        << "in"
        << "out"
        << "uniform"
        << "inout"
        // InterpolationQualifier
        << "smooth"
        << "flat"
        << "noperspective"
        // CompatibilityUniforms
        << "gl_ModelViewMatrix"
        << "gl_ModelViewProjectionMatrix"
        << "gl_ProjectionMatrix"
        << "gl_TextureMatrix"
        << "gl_ModelViewMatrixInverse"
        << "gl_ModelViewProjectionMatrixInverse"
        << "gl_ProjectionMatrixInverse"
        << "gl_TextureMatrixInverse"
        << "gl_ModelViewMatrixTranspose"
        << "gl_ModelViewProjectionMatrixTranspose"
        << "gl_ProjectionMatrixTranspose"
        << "gl_TextureMatrixTranspose"
        << "gl_ModelViewMatrixInverseTranspose"
        << "gl_ModelViewProjectionMatrixInverseTranspose"
        << "gl_ProjectionMatrixInverseTranspose"
        << "gl_TextureMatrixInverseTranspose"
        << "gl_NormalMatrix"
        << "gl_NormalScale"
        << "gl_DepthRangeParameters"
        << "gl_FogParameters"
        << "gl_LightSourceParameters"
        << "gl_LightModelParameters"
        << "gl_LightModelProducts"
        << "gl_FrontLightModelProduct"
        << "gl_BackLightModelProduct"
        << "gl_LightProducts"
        << "gl_FrontLightProduct"
        << "gl_BackLightProduct"
        << "gl_MaterialParameters"
        << "gl_FrontMaterial"
        << "gl_BackMaterial"
        << "gl_PointParameters"
        << "gl_Point"
        << "gl_TextureEnvColor"
        << "gl_ClipPlane"
        << "gl_EyePlaneS"
        << "gl_EyePlaneT"
        << "gl_EyePlaneR"
        << "gl_EyePlaneQ"
        << "gl_ObjectPlaneS"
        << "gl_ObjectPlaneT"
        << "gl_ObjectPlaneR"
        << "gl_ObjectPlaneQ"
        // ConstantsBuiltIn
        << "gl_MaxClipDistances"
        << "gl_MaxClipPlanes"
        << "gl_MaxDrawBuffers"
        << "gl_MaxTextureUnits"
        << "gl_MaxTextureCoords"
        << "gl_MaxGeometryTextureImageUnits"
        << "gl_MaxTextureImageUnits"
        << "gl_MaxVertexAttribs"
        << "gl_MaxVertexTextureImageUnits"
        << "gl_MaxCombinedTextureImageUnits"
        << "gl_MaxGeometryVaryingComponents"
        << "gl_MaxVaryingComponents"
        << "gl_MaxVaryingFloats"
        << "gl_MaxGeometryOutputVertices"
        << "gl_MaxFragmentUniformComponents"
        << "gl_MaxGeometryTotalOutputComponents"
        << "gl_MaxGeometryUniformComponents"
        << "gl_MaxVertexUniformComponents"
        // IterationAndJumps
        << "if"
        << "else"
        << "return"
        << "for"
        << "break"
        << "continue"
        << "while"
        << "do"
        << "switch"
        << "case"
        // Functions
        << "main"
        // AngleAndTrigonometryFunctions
        << "radians"
        << "degrees"
        << "sin"
        << "cos"
        << "tan"
        << "asin"
        << "acos"
        << "atan"
        << "sinh"
        << "cosh"
        << "tanh"
        << "asinh"
        << "acosh"
        << "atanh"
        // ExponentialFunctions
        << "pow"
        << "exp"
        << "exp2"
        << "log"
        << "log2"
        << "sqrt"
        << "inversesqrt"
        // CommonFunctions
        << "abs"
        << "sign"
        << "floor"
        << "trunc"
        << "round"
        << "roundEven"
        << "ceil"
        << "fract"
        << "mod"
        << "modf"
        << "min"
        << "max"
        << "clamp"
        << "mix"
        << "step"
        << "smoothstep"
        << "isnan"
        << "isinf"
        // GeometricFunctions
        << "length"
        << "distance"
        << "dot"
        << "cross"
        << "normalize"
        << "ftransform"
        << "faceforward"
        << "reflect"
        << "refract"
        // MatrixFunctions
        << "matrixCompMult"
        << "outerProduct"
        << "transpose"
        << "determinant"
        << "inverse"
        // VectorRelationalFunctions
        << "lessThan"
        << "lessThanEqual"
        << "greaterThan"
        << "greaterThanEqual"
        << "equal"
        << "notEqual"
        << "any"
        << "all"
        << "not"
        // NoiseFunctions
        << "noise1"
        << "noise2"
        << "noise3"
        << "noise4"
        // TextureLookupFunctions
        << "textureSize"
        << "texture"
        << "textureOffset"
        << "textureProj"
        << "textureProjOffset"
        << "textureLod"
        << "textureLodOffset"
        << "texelFetch"
        << "texelFetchOffset"
        << "textureProjLod"
        << "textureProjLodOffset"
        << "textureGrad"
        << "textureGradOffset"
        << "textureProjGrad"
        << "textureProjGradOffset"
        );

    const QStringList glsl_complete_v = (QStringList()
        // BuiltIn
        << "gl_VertexID"
        << "gl_InstanceID"
        << "gl_Color"
        << "gl_SecondaryColor"
        << "gl_Normal"
        << "gl_Vertex"
        << "gl_MultiTexCoord0"
        << "gl_MultiTexCoord1"
        << "gl_MultiTexCoord2"
        << "gl_MultiTexCoord3"
        << "gl_MultiTexCoord4"
        << "gl_MultiTexCoord5"
        << "gl_MultiTexCoord6"
        << "gl_MultiTexCoord7"
        << "gl_FogCoord"
        << "gl_Position"
        << "gl_PointSize"
        << "gl_ClipDistance"
        << "gl_ClipVertex"
        << "gl_FrontColor"
        << "gl_FrontSecondaryColor"
        << "gl_BackColor"
        << "gl_BackSecondaryColor"
        << "gl_TexCoord"
        << "gl_FogFragCoord"
        );

    const QStringList glsl_complete_g = (QStringList()
        // BuiltIn
        << "gl_PerVertex"
        << "gl_Position"
        << "gl_PointSize"
        << "gl_ClipDistance"
        << "gl_Primitive"
        << "gl_PrimitiveIDIn"
        // Functions
        << "EmitVertex"
        << "EndPrimitive"
        );

    const QStringList glsl_complete_f = (QStringList()
        // BuiltIn
        << "gl_FragColor"
        << "gl_FragCoord"
        << "gl_FragDepth"
        << "gl_FrontFacing"
        << "gl_ClipDistance"
        << "gl_PointCoord"
        << "gl_PrimitiveID"
        // IterationAndJump
        << "discard"
        // DerivativeFunctions
        << "dFdx"
        << "dFdy"
        << "fwidth"
        );

}

#endif // __GLSL_SYNTAX_H__