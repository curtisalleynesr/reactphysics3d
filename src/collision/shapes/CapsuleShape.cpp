/********************************************************************************
* ReactPhysics3D physics library, http://code.google.com/p/reactphysics3d/      *
* Copyright (c) 2010-2013 Daniel Chappuis                                       *
*********************************************************************************
*                                                                               *
* This software is provided 'as-is', without any express or implied warranty.   *
* In no event will the authors be held liable for any damages arising from the  *
* use of this software.                                                         *
*                                                                               *
* Permission is granted to anyone to use this software for any purpose,         *
* including commercial applications, and to alter it and redistribute it        *
* freely, subject to the following restrictions:                                *
*                                                                               *
* 1. The origin of this software must not be misrepresented; you must not claim *
*    that you wrote the original software. If you use this software in a        *
*    product, an acknowledgment in the product documentation would be           *
*    appreciated but is not required.                                           *
*                                                                               *
* 2. Altered source versions must be plainly marked as such, and must not be    *
*    misrepresented as being the original software.                             *
*                                                                               *
* 3. This notice may not be removed or altered from any source distribution.    *
*                                                                               *
********************************************************************************/

// Libraries
#include "CapsuleShape.h"
#include "../../configuration.h"
#include <cassert>

using namespace reactphysics3d;

// Constructor
CapsuleShape::CapsuleShape(decimal radius, decimal height)
            : CollisionShape(CAPSULE, radius), mRadius(radius), mHalfHeight(height * decimal(0.5)) {
    assert(radius > decimal(0.0));
    assert(height > decimal(0.0));
}

// Private copy-constructor
CapsuleShape::CapsuleShape(const CapsuleShape& shape)
             : CollisionShape(shape), mRadius(shape.mRadius), mHalfHeight(shape.mHalfHeight) {

}

// Destructor
CapsuleShape::~CapsuleShape() {

}

// Return a local support point in a given direction with the object margin.
/// A capsule is the convex hull of two spheres S1 and S2. The support point in the direction "d"
/// of the convex hull of a set of convex objects is the support point "p" in the set of all
/// support points from all the convex objects with the maximum dot product with the direction "d".
/// Therefore, in this method, we compute the support points of both top and bottom spheres of
/// the capsule and return the point with the maximum dot product with the direction vector. Note
/// that the object margin is implicitly the radius and height of the capsule.
Vector3 CapsuleShape::getLocalSupportPointWithMargin(const Vector3& direction) const {

    // If the direction vector is not the zero vector
    if (direction.lengthSquare() >= MACHINE_EPSILON * MACHINE_EPSILON) {

        Vector3 unitDirection = direction.getUnit();

        // Support point top sphere
        Vector3 centerTopSphere(0, mHalfHeight, 0);
        Vector3 topSpherePoint = centerTopSphere + unitDirection * mRadius;
        decimal dotProductTop = topSpherePoint.dot(direction);

        // Support point bottom sphere
        Vector3 centerBottomSphere(0, -mHalfHeight, 0);
        Vector3 bottomSpherePoint = centerBottomSphere + unitDirection * mRadius;
        decimal dotProductBottom = bottomSpherePoint.dot(direction);

        // Return the point with the maximum dot product
        if (dotProductTop > dotProductBottom) {
            return topSpherePoint;
        }
        else {
            return bottomSpherePoint;
        }
    }

    // If the direction vector is the zero vector we return a point on the
    // boundary of the capsule
    return Vector3(0, mRadius, 0);
}

// Return a local support point in a given direction without the object margin.
Vector3 CapsuleShape::getLocalSupportPointWithoutMargin(const Vector3& direction) const {

    // If the dot product of the direction and the local Y axis (dotProduct = direction.y)
    // is positive
    if (direction.y > 0.0) {

        // Return the top sphere center point
        return Vector3(0, mHalfHeight, 0);
    }
    else {

        // Return the bottom sphere center point
        return Vector3(0, -mHalfHeight, 0);
    }
}

// Return the local inertia tensor of the capsule
void CapsuleShape::computeLocalInertiaTensor(Matrix3x3& tensor, decimal mass) const {

	// The inertia tensor formula for a capsule can be found in : Game Engine Gems, Volume 1
	
    decimal height = mHalfHeight + mHalfHeight;
	decimal radiusSquare = mRadius * mRadius;
	decimal heightSquare = height * height;
	decimal radiusSquareDouble = radiusSquare + radiusSquare;
	decimal factor1 = decimal(2.0) * mRadius / (decimal(4.0) * mRadius + decimal(3.0) * height);
	decimal factor2 = decimal(3.0) * height / (decimal(4.0) * mRadius + decimal(3.0) * height);
	decimal sum1 = decimal(0.4) * radiusSquareDouble;
	decimal sum2 = decimal(0.75) * height * mRadius + decimal(0.5) * heightSquare;
	decimal sum3 = decimal(0.25) * radiusSquare + decimal(1.0 / 12.0) * heightSquare;
	decimal IxxAndzz = factor1 * mass * (sum1 + sum2) + factor2 * mass * sum3;
	decimal Iyy = factor1 * mass * sum1 + factor2 * mass * decimal(0.25) * radiusSquareDouble;
    tensor.setAllValues(IxxAndzz, 0.0, 0.0,
                        0.0, Iyy, 0.0,
                        0.0, 0.0, IxxAndzz);
}
