#pragma once
#include <G3D/G3DALL.h>
#include "config.hpp"

class Dolly {
public:
	struct TextureSet
	{
		shared_ptr< Texture > normal;
		shared_ptr< Texture > onHover;
		shared_ptr< Texture > onSelect;
	};

	Dolly(TextureSet trackTextures, TextureSet transWidgetTextures, TextureSet rotWidgetTextures, const LineSegment2D& lineSegment=LineSegment2D::fromTwoPoints(Point2(0,-10), Vector2(0,10)), float rotation=0.0f, float trackSlider=0.5f, float rotationMin=0.0f, float rotationMax=0.0f, bool rotationLocked=false, bool translationLocked=false);
	~Dolly();
	Dolly(const Dolly& other);
	Dolly& operator=(const Dolly& other);

	void onUserInput(UserInput* ui, const shared_ptr< Camera >& camera);
	void onGraphics3D(RenderDevice* rd, RENDER_QUALITY renderQuality, float laserHeight);
	Point2 getCenterPoint() const;
	inline float getRotation() const { return _rotation; }
	inline const Matrix2 getRotMat() const { return Matrix2(cos(_rotation), -sin(_rotation), sin(_rotation), cos(_rotation)); }

	static inline void lockDollies() { _anyDollySelected = true; }
	static inline void unlockDollies() { _anyDollySelected = false; }
	static inline bool anyDollySelected() { return _anyDollySelected; }

private:
	void initGeometry();
	void renderWidgetHelper(RenderDevice* rd, const TextureSet& textureSet, const AttributeArray& gpuVerts, bool invalidState, bool rotateWidget=false);
	TextureSet _trackTextures;
	TextureSet _transWidgetTextures;
	TextureSet _rotWidgetTextures;

	LineSegment2D _track;
	float _rotation; // in radians
	float _trackSlider;
	float _rotationMin;
	float _rotationMax;
	bool _rotationLocked;
	bool _translationLocked;
	const float INNER_CLICK_RADIUS = 4.5f;
	const float OUTER_CLICK_RADIUS = 9.0f;
	float _transformationScale;
	RenderDevice* _rd;

	shared_ptr< VertexBuffer > _vbuf;
	AttributeArray _gpuTrackVerts;
	AttributeArray _gpuTransWidgetVerts;
	AttributeArray _gpuRotWidgetVerts;
	AttributeArray _gpuTexCoords;
	IndexStream _gpuIndices;

	bool _translationSelected;
	bool _rotationSelected;

	bool _translationHovered;
	bool _rotationHovered;

	bool _translationInvalid;
	bool _rotationInvalid;

	static bool _anyDollySelected;

	bool _rdSet;
};