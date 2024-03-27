#pragma once

#include <cmath>
#include "Vector3.h"

class Matrix4 {
public:
    float m[4][4];

    Matrix4() {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    static Matrix4 Identity() {
        return Matrix4();
    }

    Matrix4 Translate(const Vector3& translation) const {
        Matrix4 result = *this;
        result.m[3][0] += translation.x;
        result.m[3][1] += translation.y;
        result.m[3][2] += translation.z;
        return result;
    }

    Matrix4 Scale(const Vector3& scale) const {
        Matrix4 result = *this;
        result.m[0][0] *= scale.x;
        result.m[1][1] *= scale.y;
        result.m[2][2] *= scale.z;
        return result;
    }

    Matrix4 Rotate(const Vector3& angles) const {
        // Convert angles to radians
        float radX = angles.x * Calc::PI / 180.0f;
        float radY = angles.y * Calc::PI / 180.0f;
        float radZ = angles.z * Calc::PI / 180.0f;

        // Rotation matrices for each axis
        Matrix4 rotX, rotY, rotZ;

        rotX.m[1][1] = cos(radX); rotX.m[1][2] = -sin(radX);
        rotX.m[2][1] = sin(radX); rotX.m[2][2] = cos(radX);

        rotY.m[0][0] = cos(radY); rotY.m[0][2] = sin(radY);
        rotY.m[2][0] = -sin(radY); rotY.m[2][2] = cos(radY);

        rotZ.m[0][0] = cos(radZ); rotZ.m[0][1] = -sin(radZ);
        rotZ.m[1][0] = sin(radZ); rotZ.m[1][1] = cos(radZ);

        // Combine rotations and apply to this matrix
        return (*this) * rotZ * rotY * rotX;
    }

    Matrix4 operator*(const Matrix4& rhs) const {
        Matrix4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = 0;
                for (int k = 0; k < 4; ++k) {
                    result.m[i][j] += m[i][k] * rhs.m[k][j];
                }
            }
        }
        return result;
    }

    static Matrix4 Perspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
        Matrix4 projection;
        float radFov = fov * Calc::PI / 180.0f; // Not sure we want that here
        float tanHalfFovy = tan(radFov / 2.0f);

        projection.m[0][0] = 1.0f / (aspectRatio * tanHalfFovy);
        projection.m[1][1] = 1.0f / tanHalfFovy;
        projection.m[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
        projection.m[2][3] = -1.0f;
        projection.m[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
        projection.m[3][3] = 0.0f;

        return projection;
    }

    static Matrix4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
        Vector3 f = (center - eye).normalized();
        Vector3 s = f.cross(up).normalized();
        Vector3 u = s.cross(f);

        Matrix4 result = Identity();
        result.m[0][0] = s.x;
        result.m[1][0] = s.y;
        result.m[2][0] = s.z;
        result.m[0][1] = u.x;
        result.m[1][1] = u.y;
        result.m[2][1] = u.z;
        result.m[0][2] = -f.x;
        result.m[1][2] = -f.y;
        result.m[2][2] = -f.z;
        result.m[3][0] = -s.dot(eye);
        result.m[3][1] = -u.dot(eye);
        result.m[3][2] = f.dot(eye);

        return result;
    }

    const float* Pointer() const {
        return &m[0][0];
    }

    void CopyToArray(float(&out)[16]) const {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                out[i * 4 + j] = m[i][j];
            }
        }
    }
};
