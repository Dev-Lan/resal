#include "Laser.hpp"

// using namespace Lasers;

std::pair<Lasers::LaserPoint, Lasers::LaserPoint> Lasers::getLaserPoints(const Laser& laser, float ttc, const shared_ptr< Texture >& spectrum) {
	Lasers::LaserPoint p1;
	p1.point = laser.ray.point;
	p1.color = getColorFromWavelength(laser.wavelength, spectrum);
	p1.intensity = laser.intensity;

	if (ttc <= 0) {
		ttc = Lasers::timeTillIntensityFallsToZero(laser.intensity);
	}
	Lasers::LaserPoint p2;
	p2.point = laser.ray.point + laser.ray.direction * ttc;
	p2.color = p1.color;
	p2.intensity = Lasers::intensityDecline(laser.intensity, ttc);

	std::pair< Lasers::LaserPoint, Lasers::LaserPoint > result = std::make_pair(p1, p2);
	return result;
}

Color3 Lasers::getColorFromWavelength(float wavelength, const shared_ptr< Texture >& spectrum) {
	float wavelengthLookup = spectrum->width() * (float) (wavelength - SPECTRUM_MIN) / (float) SPECTRUM_RANGE;
	return spectrum->readTexel(wavelengthLookup, 0).rgb();
}


float Lasers::intensityDecline(float intensity, float ttc) {
	return intensity - ttc / DISTANCE_PER_ONE_INTENSITY;
}

float Lasers::timeTillIntensityFallsToZero(float intensity) {
	return DISTANCE_PER_ONE_INTENSITY * intensity;
}

// uses Sellmeier equation
// https://en.wikipedia.org/wiki/Sellmeier_equation
float Lasers::getIndexOfRefractionFromWavelength(float wavelength) {
	// wavelength in nanometers
	// convert from nanometers to micrometers
	wavelength *= 0.001f;
	float B1, B2, B3, C1, C2, C3;
	// todo get these from table
	// http://refractiveindex.info/
	B1 = 1.03961212;
	B2 = 0.231792344;
	B3 = 1.01046945;
	C1 = 0.00600069867;
	C2 = 0.0200179144;
	C3 = 103.560653;

	// something with high ior
	// B1 = 4.1120387;
	// B2 = -0.01822767;
	// B3 = 0.089852105;
	// C1 = 0.00787144;
	// C2 = -0.00052228;
	// C3 = 0.000117731;

	float wlSqrd = wavelength * wavelength;
	float n2 = 1 + (B1 * wlSqrd / (wlSqrd - C1))
				 + (B2 * wlSqrd / (wlSqrd - C2))
				 + (B3 * wlSqrd / (wlSqrd - C3));
	return sqrt(n2);

}