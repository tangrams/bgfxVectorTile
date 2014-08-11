//
//  Layer.cpp
//  testBgfxApp
//
//  Created by Matt Blair on 8/1/14.
//
//

#include "Layer.h"

void Layer::buildGeometry(GeometryBuilder &builder) {
    
    builder.buildLayerGeometry(m_name, m_colorABGR, m_vbh, m_ibh);
    
}

void Layer::draw(uint8_t view, float *modelMtx) {
    
    bgfx::setTransform(modelMtx);
    bgfx::setState(m_renderState);
    bgfx::setVertexBuffer(m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    bgfx::submit(view);
    
}

void Layer::dispose() {
    
    bgfx::destroyVertexBuffer(m_vbh);
    bgfx::destroyIndexBuffer(m_ibh);
    
}