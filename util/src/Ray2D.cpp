#include "Ray2D.hpp"

Ray2D::Ray2D() { }

Ray2D::Ray2D(Point2 pos, Vector2 dir) : point(pos), direction(dir) { }

Ray2D::~Ray2D() { }

float Ray2D::timeToIntersection(const LineSegment2D& lineSegment) const {
	// https://rootllama.wordpress.com/2014/06/20/ray-line-segment-intersection-test-in-2d/
	// http://allenchou.net/2013/07/cross-product-of-2d-vectors/
	Vector2 v1 = point - lineSegment.point(0);
	Vector2 v2 = lineSegment.point(1) - lineSegment.point(0);
	Vector2 v3 = Vector2(-direction.y, direction.x);
	float v2Dotv3 = v2.dot(v3);
	float t2 =  v1.dot(v3) / v2Dotv3;
	if (t2 < 0 || t2 > 1) { 
		return -1;
	}
	// a.x * b.y - a.y * b.x;
	// magnitude of v2 x v1
	float crossMag = v2.x * v1.y - v2.y * v1.x;
	return crossMag / v2Dotv3;
}

float Ray2D::timeToIntersection(const Point2& center, float radius) const {
	// http://math.stackexchange.com/questions/311921/get-location-of-vector-circle-intersection
	// float a = direction.dot(direction); // should be 1.0 so omitting
	Vector2 v = point - center;
	float b = 2 * direction.dot(v);
	float c = v.dot(v) - radius * radius;
	float discrm = b*b - 4*c;
	if (discrm < 0) {
		return -1;
	}
	float t1 = (-b + sqrt(discrm)) / 2.0f;
	float t2 = (-b - sqrt(discrm)) / 2.0f;
	if (t1 <= 0 && t2 <= 0) {
		// no hit
		return -1.0f;
	} else if (t1 <= 0 && t2 > 0) {
		// hit from inside
		return t2;
	} else if (t2 <= 0 && t1 > 0) {
		// hit from inside
		return t1;
	} else {
		// return closest hit
		return min(t1,t2);
	}
}

float Ray2D::timeToIntersection(const Point2& center, float radius, float angleWidth, const Vector2& centerDir) const {
	Vector2 v = point - center;
	float b = 2 * direction.dot(v);
	float c = v.dot(v) - radius * radius;
	float discrm = b*b - 4*c;
	if (discrm < 0) {
		return -1;
	}
	float t1 = (-b + sqrt(discrm)) / 2.0f;
	float t2 = (-b - sqrt(discrm)) / 2.0f;
	if (t1 <= 0 && t2 <= 0) {
		// no hit
		return -1.0f;
	} else if (t1 <= 0 && t2 > 0) {
		// hit from inside
		Vector2 v1 = (getPointFromTime(t2) - center).direction();
		float angle = aCos(v1.dot(centerDir));
		if (angle < angleWidth / 2.f) { 
			return t2;
		} else {
			return -1.0f;
		}
	} else if (t2 <= 0 && t1 > 0) {
		// hit from inside
		Vector2 v1 = (getPointFromTime(t1) - center).direction();
		float angle = aCos(v1.dot(centerDir));
		if (angle < angleWidth / 2.f) {
			return t1;
		} else {
			return -1.0f;
		}
	} else {
		float firstT  = min(t1, t2);
		float secondT = max(t1, t2);
		
		Vector2 v1 = (getPointFromTime(firstT) - center).direction();
		float angle = aCos(v1.dot(centerDir));
		if (angle < angleWidth / 2.f) {
			return firstT;
		} else {
			Vector2 v1 = (getPointFromTime(secondT) - center).direction();
			float angle = aCos(v1.dot(centerDir));
			if (angle < angleWidth / 2.f) {
				return secondT;
			} else {
				return -1.0f;
			}

		}
	}

}


Ray2D Ray2D::getReflectedRay(const Point2& pos, const Vector2& normal) const {
	// assume this ray hits at pos and gets reflected about normal
	Vector2 directionOut = direction - 2 * direction.dot(normal) * normal;
	return Ray2D(pos, directionOut);
}

Ray2D Ray2D::getRefractedRay(const Point2& pos, const Vector2& normal, float fromN, float toN) const {
	// https://www.cs.unc.edu/~rademach/xroads-RT/RTarticle.html // mathematically sound slower version
	// if I want to try to optimize this looks like a good resource
	//		http://www.hugi.scene.org/online/coding/hugi%2023%20-%20torefrac.htm
	float n;
	Vector2 norm = normal;
	if (norm.dot(direction) > 0) {
		n = toN / fromN;
		norm *= -1.0f;
	} else {
		n = fromN / toN;
	}

	float c1 = -norm.dot(direction);
	float c2_squared = 1 - n * n * (1.0f - c1 * c1);
	Ray2D outRay;
	outRay.point = pos;
	if (c2_squared > 0) {
		float c2 = sqrt(c2_squared);
		outRay.direction = (n * direction) + (n * c1 - c2) * norm;
	} else {
		// total internal reflection
		outRay.direction = direction - 2 * direction.dot(norm) * norm;
		// outRay.direction = 2 * norm.dot(direction) * norm - direction;
	}
	return outRay;
}
