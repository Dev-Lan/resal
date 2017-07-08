#include "Dolly.hpp"

bool Dolly::_anyDollySelected = false;

Dolly::Dolly(TextureSet trackTextures, TextureSet transWidgetTextures, TextureSet rotWidgetTextures, const LineSegment2D& lineSegment, float rotation, float trackSlider, float rotationMin, float rotationMax, bool rotationLocked, bool translationLocked) :
			_trackTextures(trackTextures),
			_transWidgetTextures(transWidgetTextures),
			_rotWidgetTextures(rotWidgetTextures),
			_track(lineSegment) ,
			_rotation(rotation) ,
			_trackSlider(trackSlider) ,
			_rotationMin(rotationMin) ,
			_rotationMax(rotationMax) ,
			_rotationLocked(rotationLocked) ,
			_translationLocked(translationLocked),
			_translationSelected(false),
			_rotationSelected(false),
			_translationHovered(false),
			_rotationHovered(false),
			_translationInvalid(false),
			_rotationInvalid(false)
			{
				initGeometry();
				_transformationScale = std::min((_rotationMax - _rotationMin) / 3.14f, 1.0f); // should actually be tau.
				if (_transformationScale <= 0) {
					_transformationScale = 1.0f;
				}
			}

Dolly::~Dolly() { }

Dolly::Dolly(const Dolly& other) :
	_trackTextures(other._trackTextures),
	_transWidgetTextures(other._transWidgetTextures),
	_rotWidgetTextures(other._rotWidgetTextures),
	_track(other._track),
	_rotation(other._rotation),
	_trackSlider(other._trackSlider),
	_rotationMin(other._rotationMin),
	_rotationMax(other._rotationMax),
	_rotationLocked(other._rotationLocked),
	_translationLocked(other._translationLocked),
	_translationSelected(other._translationSelected),
	_rotationSelected(other._rotationSelected),
	_translationHovered(other._translationHovered),
	_rotationHovered(other._rotationHovered),
	_translationInvalid(other._translationInvalid),
	_rotationInvalid(other._rotationInvalid),
	_transformationScale(other._transformationScale)
	{
		initGeometry();
	}

Dolly& Dolly::operator=(const Dolly& other) {
	_trackTextures = other._trackTextures;
	_transWidgetTextures = other._transWidgetTextures;
	_rotWidgetTextures = other._rotWidgetTextures;
	_track = other._track;
	_rotation = other._rotation;
	_trackSlider = other._trackSlider;
	_rotationMin = other._rotationMin;
	_rotationMax = other._rotationMax;
	_rotationLocked = other._rotationLocked;
	_translationLocked = other._translationLocked;
	_translationSelected = other._translationSelected;
	_rotationSelected = other._rotationSelected;
	_translationHovered = other._translationHovered;
	_rotationHovered = other._rotationHovered;
	_transformationScale = other._transformationScale;
	_translationInvalid = other._translationInvalid;
	_rotationInvalid = other._rotationInvalid;
	initGeometry();

	return *this;
}



void Dolly::onUserInput(UserInput* ui, const shared_ptr< Camera >& camera) {
	int x,y;
	float distToDollySquared;
	x = ui->mouseXY().x;
	y = ui->mouseXY().y;
	if (!(x == 0 && y == 0) && (!_translationLocked || !_rotationLocked)) {
		Ray selectRay = camera->worldRay(x,y, _rd->viewport());
		Plane groundPlane(Vector3(0,0,1), Point3(0,0,0));
		Point3 intersect = selectRay.intersection(groundPlane);
		Vector2 v = getCenterPoint() - intersect.xy();
		distToDollySquared = v.dot(v);		
		
		if (!_translationLocked && distToDollySquared < INNER_CLICK_RADIUS * INNER_CLICK_RADIUS) {
			_translationHovered = true;
		} else {
			_translationHovered = false;
		}

		if (!_rotationLocked && distToDollySquared > INNER_CLICK_RADIUS * INNER_CLICK_RADIUS && distToDollySquared < OUTER_CLICK_RADIUS * OUTER_CLICK_RADIUS && !_translationHovered && !_translationSelected) {
			_rotationHovered = true;
		} else {
			_rotationHovered = false;
		}
	}

	if (ui->keyPressed(GKey::LEFT_MOUSE) && !_anyDollySelected) {
		if (_translationHovered) {
			_translationSelected = true;
			_anyDollySelected = true;
		} else if (_rotationHovered) {
			_rotationSelected = true;
			_anyDollySelected = true;
		}
	}

	if (ui->keyReleased(GKey::LEFT_MOUSE)) {
		_translationSelected = false;
		_rotationSelected = false;
		_anyDollySelected = false;
		_translationInvalid = false;
		_rotationInvalid = false;
	}
	bool shiftSelected = false;
	if (DEBUG_ENABLED) {
		if (ui->keyDown(GKey::LSHIFT)) {
			shiftSelected = true;
		}
	}
	if (_translationSelected) {
		int lastX = ui->mouseXY().x - ui->mouseDX();
		int lastY = ui->mouseXY().y - ui->mouseDY();
		Ray selectRay = camera->worldRay(lastX, lastY, _rd->viewport());
		Plane groundPlane(Vector3(0,0,1), Point3(0,0,0));
		Point3 lastIntersect = selectRay.intersection(groundPlane);

		int x = ui->mouseXY().x;
		int y = ui->mouseXY().y;
		selectRay = camera->worldRay(x,y, _rd->viewport());
		Point3 thisIntersect = selectRay.intersection(groundPlane);
		Vector2 v1 = thisIntersect.xy() - lastIntersect.xy();
		if (shiftSelected) {
			_track = LineSegment2D::fromTwoPoints(_track.point(0)+v1, _track.point(1)+v1);
		} else {
			float trackLength = (_track.point(1) - _track.point(0)).length();
			float distanceTranslated = v1.dot(_track.point(1) - _track.point(0)) / trackLength;
			_trackSlider = _trackSlider + (distanceTranslated / trackLength);
			if (_trackSlider <= 0.0f || _trackSlider >= 1.0f) {
				_translationInvalid = true;
				_trackSlider = clamp(_trackSlider, 0.0f, 1.0f);
			} else {
				_translationInvalid = false;
			}
		}
	} else if (_rotationSelected) {
		int lastX = ui->mouseXY().x - ui->mouseDX();
		int lastY = ui->mouseXY().y - ui->mouseDY();
		Ray selectRay = camera->worldRay(lastX, lastY, _rd->viewport());
		Plane groundPlane(Vector3(0,0,1), Point3(0,0,0));
		Point3 lastIntersect = selectRay.intersection(groundPlane);

		int x = ui->mouseXY().x;
		int y = ui->mouseXY().y;
		selectRay = camera->worldRay(x,y, _rd->viewport());
		Point3 thisIntersect = selectRay.intersection(groundPlane);

		Vector2 v1 = lastIntersect.xy() - getCenterPoint();
		Vector2 v2 = thisIntersect.xy() - getCenterPoint();
		Vector2 v3 = Vector2(-v1.y, v1.x);
		float angleMoved = aCos( v1.dot(v2) / (v1.length() * v2.length()) );
		
		if (shiftSelected) {
			// rotmat
			// new _track
			// todo
		} else {
			if (v2.dot(v3) > 0){
				_rotation += angleMoved * _transformationScale;
			} else {
				_rotation -= angleMoved * _transformationScale;
			}
			if (_rotationMin < _rotationMax) {
				if (_rotation <= _rotationMin || _rotation >= _rotationMax) {
					_rotationInvalid = true;
					_rotation = clamp(_rotation, _rotationMin, _rotationMax);
				} else {
					_rotationInvalid = false;
				}
			}
		}
	}

}

void Dolly::onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality,float laserHeight) {
	_rd = rd;
	_rdSet = true;
	if (renderQuality == DEBUG_QUALITY) {
		// track
		if (!_translationLocked) {
			Point3 p1 = Point3(_track.point(0), laserHeight);
			Point3 p2 = Point3(_track.point(1), laserHeight);
			Vector3 offset = (p2 - p1).cross(Vector3(0,0,1)).direction();
			offset *= 0.35f;
			LineSegment ls1 = LineSegment::fromTwoPoints(p1+offset,p2+offset);
			LineSegment ls2 = LineSegment::fromTwoPoints(p1-offset,p2-offset);
			Color4 c(0.2,0.6,0.3,0.6f);
			Draw::lineSegment(ls1, rd, c);
			Draw::lineSegment(ls2, rd, c);			
		}

		// direction
		Point3 pos = Point3(getCenterPoint(), laserHeight);
		Vector2 dir = Matrix2(cos(_rotation), -sin(_rotation), sin(_rotation), cos(_rotation)) * Vector2(1,0);
		Vector3 dir3 = Vector3(dir*5, 0);
		LineSegment ls3 = LineSegment::fromTwoPoints(pos, pos+dir3);
		Color4 c2(0.8,0.2,0.3,0.6f);
		Draw::lineSegment(ls3, rd, c2);

		// click area
		Sphere s1(Point3(getCenterPoint(), 0), INNER_CLICK_RADIUS);
		if (_translationSelected) {
			Draw::sphere(s1, rd, Color4(1, 1, 0,.3f), Color4::clear());
		} else {
			Draw::sphere(s1, rd, Color4(.33, .33, 0,.1f), Color4::clear());
		}
		if (!_rotationLocked){
			Sphere s2(Point3(getCenterPoint(), 0), OUTER_CLICK_RADIUS);
			if (_rotationSelected) {
				Draw::sphere(s2, rd, Color4(1, .6, 0,.3f), Color4::clear());
			} else {
				Draw::sphere(s2, rd, Color4(.33, .2, 0,.1f), Color4::clear());
			}			
		}
	} else if (renderQuality == HIGH_QUALITY) {
		if (_anyDollySelected && !(_translationSelected || _rotationSelected)) {
			return;
		}
		Vector2 translation = _trackSlider * (_track.point(1) - _track.point(0));
		CoordinateFrame cframeTrans(Vector3(translation.x, translation.y, 0));

		if (!_translationLocked && !_rotationSelected) {
			renderWidgetHelper(rd, _trackTextures, _gpuTrackVerts, _translationInvalid);
	
			rd->pushState();
	
			rd->setObjectToWorldMatrix(rd->objectToWorldMatrix() * cframeTrans);
			renderWidgetHelper(rd, _transWidgetTextures, _gpuTransWidgetVerts, _translationInvalid);
	
			rd->popState();
		}
		if (!_rotationLocked && !_translationSelected) {
			rd->pushState();
			CoordinateFrame cframeTrans2(Vector3(-_track.point(0).x, -_track.point(0).y, 0));
			CoordinateFrame cframeRot = CoordinateFrame::fromXYZYPRRadians(_track.point(0).x, _track.point(0).y, 0, 0, 0, _rotation);

			rd->setObjectToWorldMatrix(cframeTrans * cframeRot * cframeTrans2 * rd->objectToWorldMatrix());

			renderWidgetHelper(rd, _rotWidgetTextures, _gpuRotWidgetVerts, _rotationInvalid, true);

			rd->popState();
		}
	}
}

void Dolly::renderWidgetHelper(RenderDevice* rd, const TextureSet& textureSet, const AttributeArray& gpuVerts, bool invalidState, bool rotateWidget) {
		Args args;
		if (rotateWidget) {
			if (_rotationSelected) {
				args.setUniform("tex", textureSet.onSelect, Sampler::video());
			} else if (_rotationHovered) {
				args.setUniform("tex", textureSet.onHover, Sampler::video());				
			} else {
				args.setUniform("tex", textureSet.normal, Sampler::video());
			}
		} else {
			if (_translationSelected) {
				args.setUniform("tex", textureSet.onSelect, Sampler::video());
			} else if (_translationHovered) {
				args.setUniform("tex", textureSet.onHover, Sampler::video());				
			} else {
				args.setUniform("tex", textureSet.normal, Sampler::video());
			}
		}
		args.setUniform("invalid", invalidState);

		args.setAttributeArray("vert", gpuVerts);
		args.setAttributeArray("texCoord", _gpuTexCoords);

		args.setIndexStream(_gpuIndices);

		args.setPrimitiveType(PrimitiveType::TRIANGLES);
		rd->setBlendFunc(Framebuffer::COLOR0, RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA, RenderDevice::BLENDEQ_ADD);
		// rd->setBlendFunc(Framebuffer::COLOR0, RenderDevice::BLEND_ONE, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA, RenderDevice::BLENDEQ_ADD);
		const G3D::String shaderPattern = SHADER_DIR + "dollyTexture.*";
		LAUNCH_SHADER(shaderPattern, args);
}


void Dolly::initGeometry() {

	//   x0y1 +----------------------------+ x1y1
	//        |                            |
	//        |                            |
	//     p0 +             + center       + p1
	//        |                            |
	//        |                            |
	//   x0y0 +----------------------------+ x1y0

	// TRACK SETUP
	Array< Vector3 > cpuTrackVerts;
	float h = .02f;
	Vector3 p0 = Vector3(_track.point(0).x, _track.point(0).y, h);
	Vector3 p1 = Vector3(_track.point(1).x, _track.point(1).y, h);
	Vector3 offset = (p1 - p0).cross(Vector3(0,0,1));
	const float trackHalfWidth = 0.5f;
	offset = trackHalfWidth * offset.direction();

	// verts
	Vector3 x0y0 = p0 + offset;
	Vector3 x1y0 = p1 + offset;
	Vector3 x0y1 = p0 - offset;
	Vector3 x1y1 = p1 - offset;

	cpuTrackVerts.append(x0y0, x1y0, x0y1, x1y1);
	// TRANSLATION WIDGET SETUP
	h = 0.5f;
	offset = INNER_CLICK_RADIUS * offset.direction();

	Array< Vector3 > cpuTransWidgetVerts;
	Vector2 c2 = _track.point(0);
	Vector3 center(c2.x, c2.y, h);
	Vector3 offset2 = (p1-p0).direction();
	p0 = center - INNER_CLICK_RADIUS * offset2;
	p1 = center + INNER_CLICK_RADIUS * offset2;

	x0y0 = p0 + offset;
	x1y0 = p1 + offset;
	x0y1 = p0 - offset;
	x1y1 = p1 - offset;

	cpuTransWidgetVerts.append(x0y0, x1y0, x0y1, x1y1);

	// ROTATION WIDGET SETUP
	h = 0.49f;
	Array< Vector3 > cpuRotWidgetVerts;
	x0y0 = center + Vector3(-OUTER_CLICK_RADIUS, -OUTER_CLICK_RADIUS, 0);
	x1y0 = center + Vector3( OUTER_CLICK_RADIUS, -OUTER_CLICK_RADIUS, 0);
	x0y1 = center + Vector3(-OUTER_CLICK_RADIUS,  OUTER_CLICK_RADIUS, 0);
	x1y1 = center + Vector3( OUTER_CLICK_RADIUS,  OUTER_CLICK_RADIUS, 0);
	cpuRotWidgetVerts.append(x0y0, x1y0, x0y1, x1y1);

	// COMMON SETUP
	Array< Vector2 > cpuTexCoords;
	Array< int > cpuIndices;
	cpuTexCoords.append(Vector2(0,1));
	cpuTexCoords.append(Vector2(1,1));
	cpuTexCoords.append(Vector2(0,0));
	cpuTexCoords.append(Vector2(1,0));

	cpuIndices.append(0,1,2);
	cpuIndices.append(2,1,3);

	// SEND TO GPU
	_vbuf = VertexBuffer::create( sizeof(Vector3) * cpuTrackVerts.length()
								+ sizeof(Vector3) * cpuTransWidgetVerts.length()
								+ sizeof(Vector3) * cpuRotWidgetVerts.length()
								+ sizeof(Vector3) * cpuIndices.length()
								+ sizeof(Vector3) * cpuTexCoords.length(),
								VertexBuffer::WRITE_ONCE);
	_gpuTrackVerts = AttributeArray(cpuTrackVerts, _vbuf);
	_gpuTransWidgetVerts = AttributeArray(cpuTransWidgetVerts, _vbuf);
	_gpuRotWidgetVerts = AttributeArray(cpuRotWidgetVerts, _vbuf);
	_gpuTexCoords = AttributeArray(cpuTexCoords, _vbuf);
	_gpuIndices = IndexStream(cpuIndices, _vbuf);
}


Point2 Dolly::getCenterPoint() const {
	return _track.point(0) + _trackSlider * (_track.point(1) - _track.point(0));
}
