#include "Matrix.h"
#include "Vector.h"
#include "Tools.h"


const Mat3 Mat3_Identity = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f
};
const Mat4 Mat4_Identity = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

Mat3 Mat3_MulMM(Mat3 mat1, Mat3 mat2)
{
    Mat3 res;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            res.data[i][j] = mat1.data[i][0]*mat2.data[0][j] + mat1.data[i][1]*mat2.data[1][j] + mat1.data[i][2]*mat2.data[2][j];
        }
    }
    return res;
}

Vec3 Mat3_MulMV(Mat3 mat, Vec3 v)
{
    Vec3 res;
    for (int i = 0; i < 3; ++i) {
            res.data[i] = mat.data[i][0] * v.x + mat.data[i][1] * v.y + mat.data[i][2] * v.z;
    }
    return res;
}

float Mat3_Det(Mat3 mat)
{
    float det = mat.data[0][0] * mat.data[1][1] * mat.data[2][2] +
           mat.data[0][1] * mat.data[1][2] * mat.data[2][0] +
           mat.data[0][2] * mat.data[1][0] * mat.data[2][1] -
           mat.data[2][0] * mat.data[1][1] * mat.data[0][2] -
           mat.data[2][1] * mat.data[1][2] * mat.data[0][0] -
           mat.data[2][2] * mat.data[1][0] * mat.data[0][1];
    return det;
}

Mat4 Mat4_Transpose(Mat4 matrix)
{
    Mat4 result;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.data[i][j] = matrix.data[j][i];
        }
    }

    return result;
}

Mat4 Mat4_MulMM(Mat4 mat1, Mat4 mat2)
{
    Mat4 result;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.data[i][j] =
                    mat1.data[i][0] * mat2.data[0][j] +
                    mat1.data[i][1] * mat2.data[1][j] +
                    mat1.data[i][2] * mat2.data[2][j] +
                    mat1.data[i][3] * mat2.data[3][j];
        }
    }

    return result;
}

Vec4 Mat4_MulMV(Mat4 mat, Vec4 v) {
    Vec4 result;
    result.x = mat.data[0][0] * v.x + mat.data[0][1] * v.y + mat.data[0][2] * v.z + mat.data[0][3] * v.w;
    result.y = mat.data[1][0] * v.x + mat.data[1][1] * v.y + mat.data[1][2] * v.z + mat.data[1][3] * v.w;
    result.z = mat.data[2][0] * v.x + mat.data[2][1] * v.y + mat.data[2][2] * v.z + mat.data[2][3] * v.w;
    result.w = mat.data[3][0] * v.x + mat.data[3][1] * v.y + mat.data[3][2] * v.z + mat.data[3][3] * v.w;
    return result;
}

Mat4 Mat4_Scale(Mat4 mat, float s) {
    Mat4 result;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.data[i][j] = mat.data[i][j] * s;
        }
    }
    return result;
}

float Mat4_Cofactor(Mat4 mat, int i, int j)
{
    Mat3 temp;
    int tempI = 0, tempJ;
    for (int k = 0; k < 4; ++k) {
        if (k == i) continue;
        tempJ = 0;
        for (int l = 0; l < 4; ++l) {
            if (l == j) continue;
            temp.data[tempI][tempJ] = mat.data[k][l];
            tempJ++;
        }
        tempI++;
    }

    return pow(-1, i+j) * Mat3_Det(temp);
}

Mat4 Mat4_CofactorMatrix(Mat4 mat)
{
    Mat4 result;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result.data[i][j] = Mat4_Cofactor(mat, i, j);
        }
    }

    return result;
}


float Mat4_Det(Mat4 mat) {
    float det = 0.0f;

    for (int i = 0; i < 4; ++i) {
        det += mat.data[0][i] * Mat4_Cofactor(mat, 0, i);
    }

    return det;
}

Mat4 Mat4_Inv(Mat4 mat)
{
    float det = Mat4_Det(mat);

    if (det <= 1e-5 && det >= -1e-5) return Mat4_Identity;

    mat = Mat4_CofactorMatrix(mat);
    mat = Mat4_Transpose(mat);
    mat = Mat4_Scale(mat, 1.0f / det);

    return mat;
}

Mat4 Mat4_GetProjectionMatrix(float l, float r, float b, float t, float n, float f)
{
    Mat4 projectionMatrix = Mat4_Identity;

    if (r == l || t == b || f == n)
        return projectionMatrix;

    n = -n;
    f = -f;

    projectionMatrix.data[0][0] = 2.f * n / (r - l);
    projectionMatrix.data[1][1] = 2.f * n / (t - b);
    projectionMatrix.data[2][2] = (f + n) / (f - n);
    projectionMatrix.data[3][3] = 0.f;

    projectionMatrix.data[0][2] = -(r + l) / (r - l);
    projectionMatrix.data[1][2] = -(t + b) / (t - b);
    projectionMatrix.data[3][2] = 1.f;

    projectionMatrix.data[2][3] = -2.f * f * n / (f - n);

    return projectionMatrix;
}


Mat4 Mat4_GetScaleMatrix(float s)
{
    Mat4 scaleMatrix = Mat4_Identity;

    for (int i = 0; i < 3; i++)
        scaleMatrix.data[i][i] = s;

    return scaleMatrix;
}

Mat4 Mat4_GetTranslationMatrix(Vec3 v)
{
    Mat4 translationMatrix = Mat4_Identity;

    for (int i = 0; i < 3; i++)
    {
        translationMatrix.data[i][3] = v.data[i];
    }
    translationMatrix.data[3][3] = 1.f;

    return translationMatrix;
}

Mat4 Mat4_GetXRotationMatrix(float angle)
{
    Mat4 rotationMatrix = Mat4_Identity;

    angle = (float)(angle * M_PI / 180.f);
    float c = cosf(angle);
    float s = sinf(angle);

    rotationMatrix.data[1][1] = c;
    rotationMatrix.data[2][2] = c;

    rotationMatrix.data[1][2] = -s;
    rotationMatrix.data[2][1] = s;

    return rotationMatrix;
}

Mat4 Mat4_GetYRotationMatrix(float angle)
{
    Mat4 rotationMatrix = Mat4_Identity;

    angle = (float)(angle * M_PI / 180.f);
    float c = cosf(angle);
    float s = sinf(angle);

    rotationMatrix.data[0][0] = c;
    rotationMatrix.data[2][2] = c;

    rotationMatrix.data[2][0] = -s;
    rotationMatrix.data[0][2] = s;

    return rotationMatrix;
}

Mat4 Mat4_GetZRotationMatrix(float angle)
{
    Mat4 rotationMatrix = Mat4_Identity;

    angle = (float)(angle * M_PI / 180.f);
    float c = cosf(angle);
    float s = sinf(angle);

    rotationMatrix.data[0][0] = c;
    rotationMatrix.data[1][1] = c;

    rotationMatrix.data[0][1] = -s;
    rotationMatrix.data[1][0] = s;

    return rotationMatrix;
}

Mat4 Mat4_Print(Mat4 mat) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            printf("%f ", mat.data[i][j]);
        }
        printf("\n");
    }
}