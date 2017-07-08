#pragma once
#include <G3D/G3DALL.h>
#include "config.hpp"
#include "Ray2D.hpp"
// for all of my calculations I  cheat and define the light with wavelength instead of frequency
// even though wavelength changes in different materials I don't change it,
// and use whatever the corresponding frequency would be in a vacuum.

namespace Lasers {

struct Laser {
	Ray2D ray;
	float intensity;
	float wavelength;
};

struct LaserPoint {
	Point2 point;
	Color3 color;
	float intensity;
};

enum SPECTRUM_BOUNDS {
	// visible spectrum wavelength in nanometers
	SPECTRUM_MIN = 400,
	SPECTRUM_MAX = 700,
	SPECTRUM_RANGE = 300
};

const float DISTANCE_PER_ONE_INTENSITY = 1000; // 10 meters

std::pair<LaserPoint, LaserPoint> getLaserPoints(const Laser& laser, float ttc, const shared_ptr< Texture >& spectrum);

Color3 getColorFromWavelength(float wavelength, const shared_ptr< Texture >& spectrum);

// returns intensity after a laser has traved for time ttc
float intensityDecline(float intensity, float ttc);

// returns time until intensity falls to zero
float timeTillIntensityFallsToZero(float intensity);
	
// uses Sellmeier equation
// https://en.wikipedia.org/wiki/Sellmeier_equation
// http://refractiveindex.info/
float getIndexOfRefractionFromWavelength(float wavelength); // todo add material options

}
