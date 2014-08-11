
#define BGFX_SHADER_LANGUAGE_GLSL

#include "bgfx_utils.h"
#include "common.h"
#include "entry/entry.h"
#include "Layer.h"
#include "GeometryBuilder.h"
#include <bgfx.h>
#include <bx/readerwriter.h>
#include <math.h>
#include <CoreFoundation/CoreFoundation.h>

// Helper method for loading files into memory
static const bgfx::Memory* loadMem(bx::FileReaderI* _reader, const char* _filePath)
{
	if (0 == bx::open(_reader, _filePath) )
	{
		uint32_t size = (uint32_t)bx::getSize(_reader);
		const bgfx::Memory* mem = bgfx::alloc(size+1);
		bx::read(_reader, mem->data, size);
		bx::close(_reader);
		mem->data[mem->size-1] = '\0';
		return mem;
	}
    
	return NULL;
}

// Declaration of vertex structure
bgfx::VertexDecl PosNormColorVertex::ms_decl;

int _main_(int /*_argc*/, char** /*_argv*/)
{
	
    // Initialize BGFX
	// ---------------

	uint32_t width = 1280;
	uint32_t height = 720;
	uint32_t debug = BGFX_DEBUG_TEXT;
	uint32_t reset = BGFX_RESET_VSYNC;
    
	bgfx::init();
	bgfx::reset(width, height, reset);

	// Enable debug text.
	bgfx::setDebug(debug);

	// Set view 0 clear state.
	bgfx::setViewClear(0
		, BGFX_CLEAR_COLOR_BIT|BGFX_CLEAR_DEPTH_BIT
		, 0x000000ff
		, 1.0f
		, 0
		);
    
    PosNormColorVertex::init();
    
    // Load Resources
    // --------------
    
    GeometryBuilder geoBuilder;
    bgfx::ShaderHandle fragShader;
    bgfx::ShaderHandle vertShader;
    
    {
        CFBundleRef mainBundle = CFBundleGetMainBundle();
        
        CFURLRef tileURL = CFBundleCopyResourceURL(mainBundle, CFSTR("vectorTile"), CFSTR("json"), nullptr);
        CFStringRef filePathRef = CFURLCopyPath(tileURL);
        std::string tilePathString = std::string(CFStringGetCStringPtr(filePathRef, kCFStringEncodingUTF8));
        
        geoBuilder.loadGeoJsonFile(std::string(tilePathString));
        
        CFURLRef fragShaderURL = CFBundleCopyResourceURL(mainBundle, CFSTR("mesh.fsh"), CFSTR("bin"), nullptr);
        CFURLRef vertShaderURL = CFBundleCopyResourceURL(mainBundle, CFSTR("mesh.vsh"), CFSTR("bin"), nullptr);
        const char* fragShaderPath = CFStringGetCStringPtr(CFURLCopyPath(fragShaderURL), kCFStringEncodingUTF8);
        const char* vertShaderPath = CFStringGetCStringPtr(CFURLCopyPath(vertShaderURL), kCFStringEncodingUTF8);
        fragShader = bgfx::createShader(loadMem(entry::getFileReader(), fragShaderPath));
        vertShader = bgfx::createShader(loadMem(entry::getFileReader(), vertShaderPath));
        
        CFRelease(tileURL);
        CFRelease(fragShaderURL);
        CFRelease(vertShaderURL);
    }
    
    bgfx::ProgramHandle shaderProgram = bgfx::createProgram(vertShader, fragShader, true);

    // Generate map layers
    // -------------------
    
    int nLayers = 5;
    Layer layers[5] = {
        Layer(std::string("buildings"), 0xffffffff, BGFX_STATE_DEFAULT),
        Layer(std::string("earth"), 0xff00ff00, BGFX_STATE_DEFAULT),
        Layer(std::string("landuse"), 0xff00cc00, BGFX_STATE_DEFAULT),
        Layer(std::string("water"), 0xffff0000, BGFX_STATE_DEFAULT),
        Layer(std::string("roads"), 0xffcccccc, BGFX_STATE_DEFAULT)
    };
    
    // Calculate tile position based on indices
    {
        int zoom = 16;
        int tileX = 19293;
        int tileY = 24641;
        
        float n = powf(2.0f, zoom);
        float camX = geoBuilder.lon2x((tileX + 0.5) / n * 360.0f - 180.0f);
        float camY = geoBuilder.lat2y(atanf(sinhf(PI*(1-2*(tileY+0.5)/n))) * 180.0f / PI);
        
        // Generate GL geometry for tile layers, all offset by the tile location
        geoBuilder.setGeometryOffset(camX, camY, 0.0f);
    }
    
    for (int i = 0; i < nLayers; i++) {
        layers[i].buildGeometry(geoBuilder);
    }
    
    // Setup camera
    // ------------
    
    float at[3] = {0.0f, 0.0f, 0.0f };
    float eye[3] = {0.0f, -300.0f, 400.0f };
    
    int64_t startTime = bx::getHPCounter();
    double frequency = double(bx::getHPFrequency());

	// Main loop
	// ---------

	while (!entry::processEvents(width, height, debug, reset) )
	{
		float view[16];
        float proj[16];
        float model[16];
        bx::mtxIdentity(model);
        bx::mtxLookAt(view, eye, at);
        bx::mtxProj(proj, 60.0f, float(width)/float(height), 0.1f, 1000.0f);
        
        bgfx::setViewTransform(0, view, proj);
        
        // Set view 0 default viewport
		bgfx::setViewRect(0, 0, 0, width, height);
        
        float time = (float)((bx::getHPCounter() - startTime)/frequency);
        
        bgfx::setTransform(model);
        
        bgfx::setProgram(shaderProgram);
        
        for (int i = 0; i < nLayers; i++) {
            // Apply rotation around Y axis and offset each layer on Z axis to fix depth-ambiguous elements
            bx::mtxSRT(model, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.3f*time, 0.0f, 0.0f, 1.0f*i);
            layers[i].draw(0, model);
        }
        
        bgfx::submit(0);

		// Use debug font to print information about this example
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(0, 1, 0x6f, "Experimental vector tile renderer using BGFX");
        
		// Advance to next frame; Rendering thread will be kicked to
		// process submitted rendering primitives
		bgfx::frame();
	}
    
    // Clean up
    // --------
    
    for (int i = 0; i < nLayers; i++) {
        layers[i].dispose();
    }
    
    bgfx::destroyProgram(shaderProgram);

	// Shutdown bgfx
	bgfx::shutdown();

	return 0;
}
