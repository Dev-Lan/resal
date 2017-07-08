#pragma once
#include <G3D/G3DALL.h>
#include "Laser.hpp"
#include "Dolly.hpp"
#include "config.hpp"

class Receiver {
public:
	struct powerReq
	{
		float wavelength;
		float intensity;
		float powerSoFar;
		bool isPowered;
	};

	Receiver();
	Receiver(float radius, const Array< powerReq >& powerReqs, const shared_ptr< Dolly >& dolly, const shared_ptr< Texture >& spectrum);
	~Receiver();
	Receiver(const Receiver& other);
	Receiver& operator= (const Receiver& other);

	void onUserInput(UserInput* ui, const shared_ptr< Camera >& camera);
	void onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight);
	float timeToCollision(const Lasers::Laser& laser);
	void resetPowerReq();
	void updatePowerReq(float wavelength, float intensity);
	bool isPowered();
	inline Color3 colorReq() { return _colorReq; }

private:
	std::shared_ptr< Dolly > _dolly;
	std::shared_ptr< Texture > _spectrumTex;
	float _radius;
	Array< powerReq > _powerReqs;
	Color3 _colorReq;
	Color3 _currentColor;
};