#include "Vector.h"

const Vec2 Vec2_Right = { +1.0f, +0.0f };
const Vec2 Vec2_Left  = { -1.0f, +0.0f };
const Vec2 Vec2_Up    = { +0.0f, +1.0f };
const Vec2 Vec2_Down  = { +0.0f, -1.0f };
const Vec2 Vec2_Zero  = { +0.0f, +0.0f };
const Vec2 Vec2_One   = { +1.0f, +1.0f };

const Vec3 Vec3_Right = { +1.0f, +0.0f, +0.0f };
const Vec3 Vec3_Left  = { -1.0f, +0.0f, +0.0f };
const Vec3 Vec3_Up    = { +0.0f, +1.0f, +0.0f };
const Vec3 Vec3_Down  = { +0.0f, -1.0f, +0.0f };
const Vec3 Vec3_Front = { +0.0f, +0.0f, +1.0f };
const Vec3 Vec3_Back  = { +0.0f, +0.0f, -1.0f };
const Vec3 Vec3_Zero  = { +0.0f, +0.0f, +0.0f };
const Vec3 Vec3_One   = { +1.0f, +1.0f, +1.0f };

const Vec4 Vec4_RightH = { +1.0f, +0.0f, +0.0f, +1.0f };
const Vec4 Vec4_LeftH  = { -1.0f, +0.0f, +0.0f, +1.0f };
const Vec4 Vec4_UpH    = { +0.0f, +1.0f, +0.0f, +1.0f };
const Vec4 Vec4_DownH  = { +0.0f, -1.0f, +0.0f, +1.0f };
const Vec4 Vec4_FrontH = { +0.0f, +0.0f, +1.0f, +1.0f };
const Vec4 Vec4_BackH  = { +0.0f, +0.0f, -1.0f, +1.0f };
const Vec4 Vec4_ZeroH  = { +0.0f, +0.0f, +0.0f, +1.0f };
const Vec4 Vec4_Zero   = { +0.0f, +0.0f, +0.0f, +0.0f };
const Vec4 Vec4_One    = { +1.0f, +1.0f, +1.0f, +1.0f };

Vec2 Vec2_Add(Vec2 v1, Vec2 v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    return v1;
}

Vec2 Vec2_Sub(Vec2 v1, Vec2 v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    return v1;
}

float Vec2_SignedArea(const Vec2 a, const Vec2 b, const Vec2 c)
{
    return (b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y);
}

bool Vec2_Barycentric(Vec2 *vertices, Vec2 p, float *bary)
{
    Vec2 a = vertices[0];
    Vec2 b = vertices[1];
    Vec2 c = vertices[2];

    float det = Vec2_Sub(b, a).x * Vec2_Sub(c, a).y - Vec2_Sub(b, a).y * Vec2_Sub(c, a).x;

    float x = (Vec2_Sub(b, p).x * Vec2_Sub(c, p).y - Vec2_Sub(b, p).y * Vec2_Sub(c, p).x) / det;
    float y = (Vec2_Sub(c, p).x * Vec2_Sub(a, p).y - Vec2_Sub(c, p).y * Vec2_Sub(a, p).x) / det;
    float z = 1.f-x-y;

    bary[0] = x;
    bary[1] = y;
    bary[2] = z;

    return x >= 0 &&  y >= 0 &&  z >= 0;
}

Vec3 Vec3_Add(Vec3 v1, Vec3 v2)
{
    v1.x += v2.x;
    v1.y += v2.y;
    v1.z += v2.z;
    return v1;
}

Vec3 Vec3_Sub(Vec3 v1, Vec3 v2)
{
    v1.x -= v2.x;
    v1.y -= v2.y;
    v1.z -= v2.z;
    return v1;
}

Vec3 Vec3_Scale(Vec3 v, float s)
{
    v.x *= s;
    v.y *= s;
    v.z *= s;
    return v;
}

float Vec3_Dot(Vec3 v1, Vec3 v2)
{
    return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

Vec3 Vec3_Cross(Vec3 v1, Vec3 v2)
{
    Vec3 res;
    res.x = v1.y * v2.z - v1.z * v2.y;
    res.y = - (v1.x * v2.z - v1.z * v2.x);
    res.z = v1.x * v2.y - v1.y * v2.x;
    return res;
}

float Vec3_Length(Vec3 v)
{
    return sqrtf(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

Vec3 Vec3_Normalize(Vec3 v)
{
    float length = Vec3_Length(v);
    return Vec3_Scale(v, 1/length);
}

Vec3 Vec3_Mul(Vec3 v1, Vec3 v2)
{
    v1.x *= v2.x;
    v1.y *= v2.y;
    v1.z *= v2.z;
    return v1;
}

Vec3 Vec3_Max(Vec3 v1, Vec3 v2)
{
    v1.x = fmaxf(v1.x, v2.x);
    v1.y = fmaxf(v1.y, v2.y);
    v1.z = fmaxf(v1.z, v2.z);
    return v1;
}

Vec3 Vec3_Min(Vec3 v1, Vec3 v2)
{
    v1.x = fminf(v1.x, v2.x);
    v1.y = fminf(v1.y, v2.y);
    v1.z = fminf(v1.z, v2.z);
    return v1;
}

Vec3 Vec3_From4(Vec4 v)
{
    Vec3 res;
    if (v.w) {
        res.x = v.x/v.w;
        res.y = v.y/v.w;
        res.z = v.z/v.w;
    }
    else {
        res.x = v.x;
        res.y = v.y;
        res.z = v.z;
    }
    return res;
}
