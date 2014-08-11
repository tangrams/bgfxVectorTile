//
//  PosNormColorVertex.h
//  testBgfxApp
//
//  Created by Matt Blair on 8/1/14.
//
//

#ifndef __bgfxVectorTile__PosNormColorVertex__
#define __bgfxVectorTile__PosNormColorVertex__

#include <bgfx.h>
#include <iostream>

struct PosNormColorVertex
{
    float m_x;
    float m_y;
    float m_z;
    
    float m_nx;
    float m_ny;
    float m_nz;
    
    uint32_t m_abgr;
    
    static bgfx::VertexDecl ms_decl;
    
    static void init()
    {
        ms_decl
        .begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Normal, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
        .end();
    };
    
    PosNormColorVertex(float x, float y, float z, float nx, float ny, float nz, uint32_t abgr) :
        m_x(x), m_y(y), m_z(z), m_nx(nx), m_ny(ny), m_nz(nz), m_abgr(abgr) { };
};

#endif /* defined(__bgfxVectorTile__PosNormColorVertex__) */
