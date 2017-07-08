#include "Receiver.hpp"

Receiver::Receiver() {

}

Receiver::Receiver(float radius, const Array< powerReq >& powerReqs, const shared_ptr< Dolly >& dolly, const shared_ptr< Texture >& spectrum) : _dolly(dolly), _radius(radius){
	_powerReqs = powerReqs;
	_spectrumTex = spectrum;
	_currentColor = Color3(0,0,0);
	_colorReq = Color3(0,0,0);
	Color3 c;
	for (int i = 0; i < _powerReqs.length(); i++) {
		c = Lasers::getColorFromWavelength(_powerReqs[i].wavelength, spectrum);
		_colorReq += c;
	}
}

Receiver::~Receiver() { }

Receiver::Receiver(const Receiver& other) : _dolly(other._dolly), _radius(other._radius), _colorReq(other._colorReq), _spectrumTex(other._spectrumTex) {
	_powerReqs = other._powerReqs;
}

Receiver& Receiver::operator=(const Receiver& other) {
	_dolly = other._dolly;
	_radius = other._radius;
	_powerReqs = other._powerReqs;
	_colorReq = other._colorReq;
	_spectrumTex = other._spectrumTex;
	return *this;
}


void Receiver::onUserInput(UserInput* ui, const shared_ptr< Camera >& camera) {
	// todo
}

void Receiver::onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight) {
	renderQuality = DEBUG_QUALITY; // TODO - remove after impementing high quality
	if (renderQuality == DEBUG_QUALITY) {
		int numSegs = (int)_radius + 5;
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
			if (isPowered()) {
				Draw::lineSegment(ls, rd, _colorReq);
			} else {
				glLineWidth(0.25);
				Draw::lineSegment(ls, rd, _colorReq);
			}
		}
		numSegs = 5;
		for (int i=0; i < numSegs; i++) {
			float percent = float(i) / (float) numSegs;
			float x = _radius * 0.9f * sin(percent * tau) + _dolly->getCenterPoint().x;
			float y = _radius * 0.9f * cos(percent * tau) + _dolly->getCenterPoint().y;
			Vector3 p1(x,y,laserHeight);
			percent = float(i+1) / (float) numSegs;
			x = _radius * 0.9f * sin(percent * tau) + _dolly->getCenterPoint().x;
			y = _radius * 0.9f * cos(percent * tau) + _dolly->getCenterPoint().y;
			Vector3 p2(x,y,laserHeight);
			LineSegment ls = LineSegment::fromTwoPoints(p1,p2);
			if (isPowered()) {
				Draw::lineSegment(ls, rd, _colorReq);
			} else {
				glLineWidth(0.1);
				Draw::lineSegment(ls, rd, _currentColor);
			}
		}
	glLineWidth(1);

	} else if( renderQuality == HIGH_QUALITY) {
		// todo
	}}

float Receiver::timeToCollision(const Lasers::Laser& laser) {
	float t = laser.ray.timeToIntersection(_dolly->getCenterPoint(), _radius);
	if (t > 0) {
		return t;
	} else {
		return -1.0f;
	}
}

void Receiver::resetPowerReq() {
	for (int i = 0; i < _powerReqs.length(); ++i) {
		_powerReqs[i].isPowered = false;
		_powerReqs[i].powerSoFar = 0.0f;
	}
	_currentColor = Color3(0,0,0);
}

void Receiver::updatePowerReq(float wavelength, float intensity) {
	for (int i = 0; i < _powerReqs.length(); ++i) {
		if (wavelength == _powerReqs[i].wavelength) {
			_powerReqs[i].powerSoFar += intensity;
			if (_powerReqs[i].powerSoFar > _powerReqs[i].intensity) {
				_powerReqs[i].isPowered = true;
				_currentColor += Lasers::getColorFromWavelength(wavelength, _spectrumTex);
			}
		}
	}
}

bool Receiver::isPowered() {
	for (int i = 0; i < _powerReqs.length(); ++i) {
		if (!_powerReqs[i].isPowered) {
			return false;
		}
	}
	return true;
}
