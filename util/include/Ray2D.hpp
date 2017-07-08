#pragma once
#include <G3D/G3DAll.h>
// could just include Vector2 and Point2
class Ray2D {
public:
	Ray2D();
	Ray2D(Point2 pos, Vector2 dir);
	~Ray2D();
	Point2 point;
	Vector2 direction;

	// ray line segment intersection
	float timeToIntersection(const LineSegment2D& lineSegment) const;

	// ray circle intersection
	float timeToIntersection(const Point2& center, float radius) const;
	
	/// ray arc intersection
	float timeToIntersection(const Point2& center, float radius, float angleWidth, const Vector2& centerDir) const;

	Ray2D getReflectedRay(const Point2& pos, const Vector2& normal) const;

	Ray2D getRefractedRay(const Point2& pos, const Vector2& normal, float fromN, float toN) const;

	// todo ray oval intersection, ray hyperbola intersection, ray arc intersection

	// not hard, but I use this all the time
	inline Point2 getPointFromTime(float t) const { return point + t * direction; }
};


