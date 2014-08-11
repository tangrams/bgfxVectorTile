//
//  Layer.h
//  testBgfxApp
//
//  Created by Matt Blair on 8/1/14.
//
//

#ifndef __bgfxVectorTile__Layer__
#define __bgfxVectorTile__Layer__

#include "GeometryBuilder.h"
#include <bgfx.h>
#include <iostream>

class Layer {
    
public:
    
    Layer(std::string name, uint32_t colorABGR, uint64_t renderState) : m_name(name), m_colorABGR(colorABGR), m_renderState(renderState) {
        m_vbh.idx = bgfx::invalidHandle;
        m_ibh.idx = bgfx::invalidHandle;
    };
    
    void draw(uint8_t view, float* modelMtx);
    void buildGeometry(GeometryBuilder &builder);
    void dispose();
    
    std::string m_name;
    uint32_t m_colorABGR;
    uint64_t m_renderState;
    
private:
    
    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle m_ibh;
    
};

#endif /* defined(__bgfxVectorTile__Layer__) */
