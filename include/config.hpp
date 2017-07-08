#pragma once
#include <G3D/G3DAll.h>

// these are set by my compiler, but if you don't set that up, at least one of these lines should be uncommented
// #define DEBUG_ENABLED=true
// #define DEBUG_ENABLED=false

#if (DEBUG_ENABLED)
	const G3D::String DATA_DIR = "./";
#else
	const G3D::String DATA_DIR = "../Resources/"; // this should work for deploying on mac
#endif

const G3D::String TEXTURE_DIR = DATA_DIR + "textures/";
const G3D::String SHADER_DIR  = DATA_DIR + "shaders/";
const G3D::String LIGHTING_PARAMETERS_DIR  = DATA_DIR + "lightingParameters/";
const G3D::String SCENE_DIR = DATA_DIR + "scenes/";

const G3D::String LEVELS_FILENAME = SCENE_DIR + "levels.xml";

const G3D::String RUSTY_DIAMOND_METAL_TEXTURE_FILENAME = TEXTURE_DIR + "rusty_diamond_metal.jpg";
const G3D::String RUSTY_DIAMOND_METAL_NRM_FILENAME = TEXTURE_DIR + "rusty_diamond_metal_NRM.png";
// const G3D::String RUSTY_DIAMOND_METAL_TEXTURE_FILENAME = TEXTURE_DIR + "rusty_diamond_metal.png";
const G3D::String SHINY_METAL_HANDLE_TEXTURE_FILENAME = TEXTURE_DIR + "metal_img_1049_l.jpg";
const G3D::String SHINY_METAL_HANDLE_NRM_FILENAME = TEXTURE_DIR + "metal_img_1049_l_NRM.png";

const G3D::String PLASTER_TEXTURE_FILENAME = TEXTURE_DIR + "metal_00054_l.jpg";
const G3D::String PLASTER_NRM_FILENAME = TEXTURE_DIR + "metal_00054_l_NRM.png";

const G3D::String PAPER_TEXTURE_FILENAME = TEXTURE_DIR + "paper_img_1854_l.jpg";
const G3D::String PAPER_NRM_FILENAME = TEXTURE_DIR + "paper_img_1854_l_NRM.png";

// this version has small medium large (s,m,l)
const G3D::String SHEET_METAL_TEXTURE_FILENAME = TEXTURE_DIR + "metal_17072010053a_m.jpg";
const G3D::String SHEET_METAL_NRM_FILENAME = TEXTURE_DIR + "metal_17072010053a_m_NRM.png";


const G3D::String NRM_TEST_FILENAME = TEXTURE_DIR + "NRM_TEST.png";

const G3D::String LIGHT_COLORS_FILENAME = LIGHTING_PARAMETERS_DIR + "rainbow.png";
const G3D::String SPECTRUM_FILENAME = LIGHTING_PARAMETERS_DIR + "spectrum.jpg";


enum RENDER_QUALITY {
	HIGH_QUALITY,
	DEBUG_QUALITY
};

const float RAY_NEAR_EPS = 0.001f;
