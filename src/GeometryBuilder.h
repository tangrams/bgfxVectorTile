//
//  GeometryBuilder.h
//  testBgfxApp
//
//  Created by Matt Blair on 8/1/14.
//
//

#ifndef __bgfxVectorTile__GeometryBuilder__
#define __bgfxVectorTile__GeometryBuilder__

#define R_EARTH 6378137
#define PI 3.14159265

#include "PosNormColorVertex.h"
#include "json.h"
#include "tesselator.h"
#include "common.h"
#include <bx/readerwriter.h>
#include <bgfx.h>
#include <iostream>

class GeometryBuilder {

public:
    
    GeometryBuilder();
    
    void loadGeoJsonFile(std::string fileName);
    void unloadGeoJsonFile();
    
    void setGeometryOffset(float x_offset, float y_offset, float z_offset);
    void buildLayerGeometry(std::string &layerName, uint32_t abgr, bgfx::VertexBufferHandle &vbh, bgfx::IndexBufferHandle &ibh);
    
    float y2lat(float y) { return bx::toDeg(2 * atan(exp(bx::toRad(y))) - PI / 2); }
    float x2lon(float x) { return bx::toDeg(x / R_EARTH); }
    float lat2y(float lat) { return R_EARTH * log(tan(PI / 4 + bx::toRad(lat) / 2)); }
    float lon2x(float lon) { return bx::toRad(lon) * R_EARTH; }
    
private:
    
    TESStesselator* m_tess;
    Json::Value m_jsonRoot;
    float m_geometryOffset[3];
    
    void polygonJson2Verts(Json::Value &polygonJson, std::vector<PosNormColorVertex> &verts, std::vector<uint16_t> &inds, float minHeight, float height, uint32_t abgr);
    void lineJson2Verts(Json::Value &lineJson, std::vector<PosNormColorVertex> &verts, std::vector<uint16_t> &inds, uint32_t abgr);
    
};

#endif /* defined(__bgfxVectorTile__GeometryBuilder__) */
