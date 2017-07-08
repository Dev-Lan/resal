#include "CircularMirror.hpp"

CircularMirror::CircularMirror() { }

CircularMirror::CircularMirror(float radius, const shared_ptr< Dolly >& dolly) : _radius(radius) {
	_dolly = dolly;
}

CircularMirror::~CircularMirror() { }

// todo update 
CircularMirror::CircularMirror(const CircularMirror& other) : _dolly(other._dolly) { }

// todo update 
CircularMirror& CircularMirror::operator=(const CircularMirror& other) {
	_dolly = other._dolly;
	return *this;
}


void CircularMirror::onUserInput(UserInput* ui, const shared_ptr< Camera >& camera) {

}

void CircularMirror::onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight) {
	renderQuality = DEBUG_QUALITY; // TODO - remove after impementing high quality
	if (renderQuality == DEBUG_QUALITY) {
		const int numSegs = (int)_radius + 5;
		float tau = 2*pi();
		for (int i=0; i < numSegs; i++) {
			float percent = float(i) / (float) numSegs;
			float x = _radius * sin(percent * tau) + _dolly->getCenterPoint().x;
			float y = _radius * cos(percent * tau) + _dolly->getCenterPoint().y;
			Vector3 p1(x,y,laserHeight);
			percent = float(i+1) / (float) numSegs;
			x = _radius * sin(percent * tau) + _dolly->getCenterPoint().x;
			y = _radius * cos(percent * tau) + _dolly->getCenterPoint().y;
			Vector3 p2(x,y,laserHeight);
			LineSegment ls = LineSegment::fromTwoPoints(p1,p2);
			Draw::lineSegment(ls, rd, Color3::white());
		}
		// Cylinder cyl(Vector3(_dolly.getCenterPoint(),0), Vector3(_dolly.getCenterPoint(),2*laserHeight), _radius);
		// Sphere sphere(Vector3(_dolly.getCenterPoint(), laserHeight), _radius);
		// Draw::sphere(sphere, rd, Color4(0, 1, 1,.5), Color4::clear());		
	} else if( renderQuality == HIGH_QUALITY) {
		// todo
	}
}

float CircularMirror::timeToCollision(const Lasers::Laser& laserIn) {
	float t = laserIn.ray.timeToIntersection(_dolly->getCenterPoint(), _radius);
	if (t > 0) {
		return t;
	} else {
		return -1.0f;
	}
}

Array< Lasers::Laser > CircularMirror::getResultingLasers(const Lasers::Laser& laserIn) {
	// refind first mirror side hit, could keep track of this in last call, but this is safer.
	float ttc = laserIn.ray.timeToIntersection(_dolly->getCenterPoint(), _radius);
	Vector2 v = laserIn.ray.point - _dolly->getCenterPoint();
	Vector2 normal = (laserIn.ray.getPointFromTime(ttc) - _dolly->getCenterPoint()).direction();
	const float EPS = 0.1f;
	// bool hitFromInside = true;
	// if (v.dot(v) + EPS <= _radius * _radius) {
	// 	normal *= -1.0f;
	// // 	hitFromInside = true;
	// }

	Array< Lasers::Laser > resultList;


	Point2 pos = laserIn.ray.getPointFromTime(ttc);
	Lasers::Laser reflectedLaser;
	reflectedLaser.ray = laserIn.ray.getReflectedRay(pos, normal);
	// reflectedLaser.ray = laserIn.ray.getReflectedRay(pos, normal);
	//
	reflectedLaser.intensity = Lasers::intensityDecline(laserIn.intensity, ttc);
	reflectedLaser.wavelength = laserIn.wavelength;
	reflectedLaser.ray.point = reflectedLaser.ray.getPointFromTime(RAY_NEAR_EPS);
	resultList.append(reflectedLaser);

	return resultList;
}

// Dolly& CircularMirror::getDolly() {
// 	return *_dolly;
// }