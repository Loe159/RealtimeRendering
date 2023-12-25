//
// Created by loe on 24/12/23.
//

#ifndef LIGHT_H
#define LIGHT_H

#endif //LIGHT_H

#include "Vector.h"
#include "Tools.h"
#include "Shader.h"

typedef enum {
    LIGHT_TYPE_DIFFUSE,
    LIGHT_TYPE_SPECULAR_BLINN,
    LIGHT_TYPE_SPECULAR_BLINN_PHONG,
} LightType;

/// @brief Structure représentant une lumière.
typedef struct Light_s {
    Vec3 m_lightDirection;
    Vec3 m_lightColor;
    LightType m_lightType;
    float m_lightIntensity;
} Light;

INLINE void Light_SetLightDirection(Light *light, Vec3 direction)
{
    light->m_lightDirection = Vec3_Normalize(direction);
}

INLINE Vec3 Light_GetLightDirection(Light *light)
{
    return light->m_lightDirection;
}

INLINE void Light_SetLightType(Light *light, LightType type)
{
    light->m_lightType = type;
}

INLINE LightType Light_GetLightType(Light *light)
{
    return light->m_lightType;
}

INLINE void Light_CycleLightType(Light *light)
{
    light->m_lightType = (1+light->m_lightType)%3;
    switch (light->m_lightType) {
        case LIGHT_TYPE_DIFFUSE:
            printf("Light type : Diffuse\n");
        break;
        case LIGHT_TYPE_SPECULAR_BLINN:
            printf("Light type : Blinn\n");
        break;
        case LIGHT_TYPE_SPECULAR_BLINN_PHONG:
            printf("Light type : Blinn-Phong\n");
        break;
    }
}

INLINE void Light_SetLightColor(Light *light, Vec3 color)
{
    light->m_lightColor = Vec3_Clamp01(color);
}

INLINE Vec3 Light_GetLightColor(Light *light)
{
    return light->m_lightColor;
}

INLINE void Light_SetLightIntensity(Light *light, float intensity)
{
    light->m_lightIntensity = intensity;
}

INLINE float Light_GetLightIntensity(Light *light)
{
    return light->m_lightIntensity;
}

/// @brief Calculate the ligthing coefficient depending on the light type and some parameters
float CalculateLightingCoefficient(Light *light, FShaderIn *in, Vec3 cameraPos);

/// @brief Libération de la mémoire
void Light_Free(Light *light);