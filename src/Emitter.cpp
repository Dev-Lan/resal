#include "Emitter.hpp"


Emitter::Emitter(const shared_ptr< Dolly >& dolly, float wavelength, float intensity, const Ray2D& ray) {
	this->dolly = dolly;
	laser.wavelength = wavelength;
	laser.intensity = intensity;
	_initRay = ray;
	laser.ray = ray;
	// laser.ray.point = dolly->getCenterPoint();
	// laser.ray.direction = dolly->getRotMat() * Vector2(1,0);
}

Emitter::~Emitter() { }

Emitter::Emitter(const Emitter& other) : laser(other.laser), dolly(other.dolly), _initRay(other._initRay) { }

Emitter& Emitter::operator=(const Emitter& other) {
	laser = other.laser;
	dolly = other.dolly;
	_initRay = other._initRay;
	return *this;
}

void Emitter::onUserInput(UserInput* ui, const shared_ptr< Camera >& camera) {
	// todo, fix this so lasers can be offset from Dolly
	// std::cout << "(" << _initRay.point.x << "," << _initRay.point.y << ")" <<std::endl;
	laser.ray.point = dolly->getCenterPoint() + dolly->getRotMat() * Vector2(_initRay.point);
	laser.ray.direction = dolly->getRotMat() * _initRay.direction;
}

void Emitter::onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight) {
	// todo
}