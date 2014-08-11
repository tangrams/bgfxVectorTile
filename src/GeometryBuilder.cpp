//
//  GeometryBuilder.cpp
//  testBgfxApp
//
//  Created by Matt Blair on 8/1/14.
//
//

#include "GeometryBuilder.h"

GeometryBuilder::GeometryBuilder() {
    
    m_tess = tessNewTess(nullptr);
    m_geometryOffset[0] = m_geometryOffset[1] = m_geometryOffset[2] = 0.0f;
    
}

void GeometryBuilder::loadGeoJsonFile(std::string fileName) {
    
    // Open file and read into a buffer
    bx::FileReaderI* reader = entry::getFileReader();
    
    if (0 == bx::open(reader, fileName.data())) {
        
        int size = (int)bx::getSize(reader);
        char* jsonBuffer = new char[size];
        
        bx::read(reader, jsonBuffer, size);
        bx::close(reader);
        
        // Parse buffer into a JSON object
        Json::Reader jsonReader;
        jsonReader.parse(jsonBuffer, jsonBuffer + size, m_jsonRoot);
        
        delete[] jsonBuffer;
        
    } else {
        
        printf("Failed to open json file :( \n");
        
    }
    
}

void GeometryBuilder::unloadGeoJsonFile() {
    
    m_jsonRoot.clear();
    
}

void GeometryBuilder::setGeometryOffset(float x_offset, float y_offset, float z_offset) {
    m_geometryOffset[0] = x_offset;
    m_geometryOffset[1] = y_offset;
    m_geometryOffset[2] = z_offset;
}

void GeometryBuilder::buildLayerGeometry(std::string &layerName, uint32_t abgr, bgfx::VertexBufferHandle &vbh, bgfx::IndexBufferHandle &ibh) {
    
    Json::Value featureListJson = m_jsonRoot[layerName.c_str()]["features"];
    
    std::vector<PosNormColorVertex> verts;
    std::vector<uint16_t> inds;
    
    for (int i = 0; i < featureListJson.size(); i++) {
        
        Json::Value geometryJson = featureListJson[i]["geometry"];
        Json::Value propsJson = featureListJson[i]["properties"];
        
        std::string geometryType = geometryJson["type"].asString();
        
        float height  = 0.0f;
        if (propsJson.isMember("height")) {
            height = propsJson["height"].asFloat();
        }
        
        float minHeight = 0.0f;
        if (propsJson.isMember("min_height")) {
            minHeight = propsJson["min_height"].asFloat();
        }
        
        if (geometryType.compare("Point") == 0) {
            
            // TODO
            
        } else if (geometryType.compare("MultiPoint") == 0) {
            
            // TODO
            
        } else if (geometryType.compare("LineString") == 0) {
            
            lineJson2Verts(geometryJson["coordinates"], verts, inds, abgr);
            
        } else if (geometryType.compare("MultiLineString") == 0) {
            
            for (int j = 0; j < geometryJson["coordinates"].size(); j++) {
                
                lineJson2Verts(geometryJson["coordinates"][j], verts, inds, abgr);
                
            }
            
        } else if (geometryType.compare("Polygon") == 0) {
            
            polygonJson2Verts(geometryJson["coordinates"], verts, inds, minHeight, height, abgr);
            
        } else if (geometryType.compare("MultiPolygon") == 0) {
            
            for (int j = 0; j < geometryJson["coordinates"].size(); j++) {
                
                polygonJson2Verts(geometryJson["coordinates"][j], verts, inds, minHeight, height, abgr);
                
            }
            
        } else if (geometryType.compare("GeometryCollection") == 0) {
            
            // TODO
            
        }
        
    }
    
    const bgfx::Memory* vertMem = bgfx::copy(verts.data(), (int)verts.size() * sizeof(PosNormColorVertex));
    const bgfx::Memory* indMem = bgfx::copy(inds.data(), (int)inds.size() * sizeof(uint16_t));
    
    vbh = bgfx::createVertexBuffer(vertMem, PosNormColorVertex::ms_decl);
    ibh = bgfx::createIndexBuffer(indMem);
    
}

void GeometryBuilder::polygonJson2Verts(Json::Value &polygonJson, std::vector<PosNormColorVertex> &verts, std::vector<uint16_t> &inds, float minHeight, float height, uint32_t abgr) {
    
    uint16_t indexOffset = (uint16_t)verts.size();
    
    // Loop over all rings in polygon geometry
    for (int n = 0; n < polygonJson.size(); n++) {
        
        // Extract coordinates and project into mercator (x, y)
        int nRingVerts = polygonJson[0].size();
        float ringCoords[nRingVerts * 3];
        
        for (int i = 0; i < nRingVerts; i++) {
            ringCoords[3*i] = lon2x(polygonJson[0][i][0].asFloat()) - m_geometryOffset[0];
            ringCoords[3*i+1] = lat2y(polygonJson[0][i][1].asFloat()) - m_geometryOffset[1];
            ringCoords[3*i+2] = height - m_geometryOffset[2];
        }
        
        // Extrude polygon based on height
        if (height != minHeight) {
            
            float up[3] = {0.0f, 0.0f, 1.0f};
            float tan[3];
            float nor[3];
            
            for (int i = 0; i < nRingVerts - 1; i++) {
                
                //For each vertex in the polygon, make two triangles to form a quad
                float xi = ringCoords[3*i];
                float yi = ringCoords[3*i+1];
                float zi = ringCoords[3*i+2];
                float xip1 = ringCoords[3*(i+1)];
                float yip1 = ringCoords[3*(i+1)+1];
                float zip1 = ringCoords[3*(i+1)+2];
                
                bx::vec3Sub(tan, &ringCoords[3*(i+1)], &ringCoords[3*i]);
                bx::vec3Cross(nor, up, tan);
                
                PosNormColorVertex vert1 = PosNormColorVertex(xi, yi, zi, nor[0], nor[1], nor[2], abgr);
                PosNormColorVertex vert2 = PosNormColorVertex(xip1, yip1, zip1, nor[0], nor[1], nor[2], abgr);
                PosNormColorVertex vert3 = PosNormColorVertex(xi, yi, minHeight - m_geometryOffset[2], nor[0], nor[1], nor[2], abgr);
                PosNormColorVertex vert4 = PosNormColorVertex(xip1, yip1, minHeight - m_geometryOffset[2], nor[0], nor[1], nor[2], abgr);
                
                verts.push_back(vert1);
                verts.push_back(vert2);
                verts.push_back(vert3);
                verts.push_back(vert4);
                
                inds.push_back(indexOffset);
                inds.push_back(indexOffset + 2);
                inds.push_back(indexOffset + 1);
                
                inds.push_back(indexOffset + 1);
                inds.push_back(indexOffset + 2);
                inds.push_back(indexOffset + 3);
                
                indexOffset += 4;
            }
        }
        
        // Add contour to tesselator
        
        tessAddContour(m_tess, 3, ringCoords, 3 * sizeof(float), nRingVerts);
    }
    
    // Tessellate polygon into triangles
    
    tessTesselate(m_tess, TessWindingRule::TESS_WINDING_NONZERO, TessElementType::TESS_POLYGONS, 3, 3, nullptr);
    
    // Extract triangle elements from tessellator
    
    const int nElems = tessGetElementCount(m_tess);
    const TESSindex* elems = tessGetElements(m_tess);
    
    for (int i = 0; i < nElems; i++) {
        const TESSindex* poly = &elems[i * 3];
        for (int j = 0; j < 3; j++) {
            inds.push_back((uint16_t)poly[j] + indexOffset);
        }
    }
    
    const int nTessVerts = tessGetVertexCount(m_tess);
    const float* tessVerts = tessGetVertices(m_tess);
    
    for (int i = 0; i < nTessVerts; i++) {
        PosNormColorVertex vert = PosNormColorVertex(tessVerts[3*i], tessVerts[3*i + 1], tessVerts[3*i + 2], 0.0f, 0.0f, 1.0f, abgr);
        verts.push_back(vert);
    }
    
}

void GeometryBuilder::lineJson2Verts(Json::Value &lineJson, std::vector<PosNormColorVertex> &verts, std::vector<uint16_t> &inds, uint32_t abgr) {
    
    float width = 3.0f; // Hard-coded line width, for now
    
    uint16_t indexOffset = (uint16_t)verts.size();
    
    int nLineVerts = lineJson.size();
    
    float xim1, yim1, zim1; // Previous point coordinates
    float xi, yi, zi; // Current point coordinates
    float xip1, yip1, zip1; // Next point coordinates
    
    float normi[3]; // Right normal to segment between previous and current points
    float normip1[3]; // Right normal to segment between current and next points
    float rightNorm[3]; // Right "normal" at current point, scaled for miter joint
    
    // First segment creates a square "beginning" to the polyline
    xi = lon2x(lineJson[0][0].asFloat()) - m_geometryOffset[0];
    yi = lat2y(lineJson[0][1].asFloat()) - m_geometryOffset[1];
    zi = -m_geometryOffset[2];
    
    xip1 = lon2x(lineJson[1][0].asFloat()) - m_geometryOffset[0];
    yip1 = lat2y(lineJson[1][1].asFloat()) - m_geometryOffset[1];
    zip1 = -m_geometryOffset[2];
    
    normip1[0] = yip1 - yi;
    normip1[1] = xi - xip1;
    normip1[2] = 0;
    bx::vec3Norm(normip1, normip1);
    
    bx::vec3Mul(rightNorm, normip1, width);
    
    PosNormColorVertex startRight = PosNormColorVertex(xi + rightNorm[0], yi + rightNorm[1], -m_geometryOffset[2], 0.0f, 0.0f, 1.0f, abgr);
    PosNormColorVertex startLeft = PosNormColorVertex(xi - rightNorm[0], yi - rightNorm[0], -m_geometryOffset[2], 0.0f, 0.0f, 1.0f, abgr);
    verts.push_back(startRight);
    verts.push_back(startLeft);
    
    // Loop over intermediate points in the polyline
    for (int i = 1; i < nLineVerts - 1; i++) {
        
        xim1 = xi;
        yim1 = yi;
        zim1 = zi;
        
        xi = xip1;
        yi = yip1;
        zi = zip1;
        
        xip1 = lon2x(lineJson[i+1][0].asFloat()) - m_geometryOffset[0];
        yip1 = lat2y(lineJson[i+1][1].asFloat()) - m_geometryOffset[1];
        zip1 = -m_geometryOffset[2];
        
        bx::vec3Move(normi, normip1);
        
        normip1[0] = yip1 - yi;
        normip1[1] = xi - xip1;
        normip1[2] = 0.0f;
        bx::vec3Norm(normip1, normip1);
        
        bx::vec3Add(rightNorm, normi, normip1);
        float scale = sqrtf(2 / (1 + bx::vec3Dot(normi, normip1))) * width / 2;
        bx::vec3Mul(rightNorm, rightNorm, scale);
        
        PosNormColorVertex vertRight = PosNormColorVertex(xi + rightNorm[0], yi + rightNorm[1], -m_geometryOffset[2], 0.0f, 0.0f, 1.0f, abgr);
        PosNormColorVertex vertLeft = PosNormColorVertex(xi - rightNorm[0], yi - rightNorm[1], -m_geometryOffset[2], 0.0f, 0.0f, 1.0f, abgr);
        
        verts.push_back(vertRight);
        verts.push_back(vertLeft);
        
    }
    
    // Last segment creates a square "end" to the polyline
    PosNormColorVertex endRight = PosNormColorVertex(xip1 + normip1[0]*width, yip1 + normip1[1]*width , -m_geometryOffset[2], 0.0f, 0.0f, 1.0f, abgr);
    PosNormColorVertex endLeft = PosNormColorVertex(xip1 - normip1[0]*width, yip1 - normip1[1]*width, -m_geometryOffset[2], 0.0f, 0.0f, 1.0f, abgr);
    verts.push_back(endRight);
    verts.push_back(endLeft);
    
    // Assign indices
    for (int i = 0; i < nLineVerts - 1; i++) {
        
        inds.push_back(indexOffset + 2*i+3);
        inds.push_back(indexOffset + 2*i+2);
        inds.push_back(indexOffset + 2*i);
        
        inds.push_back(indexOffset + 2*i);
        inds.push_back(indexOffset + 2*i+1);
        inds.push_back(indexOffset + 2*i+3);
        
    }
    
    
}
