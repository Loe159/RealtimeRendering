//
// Created by loe on 24/12/23.
//

#include "Light.h"


float CalculateLightingCoefficient(Light *light, FShaderIn *in, Vec3 cameraPos) {
    Vec3 lightVector = Light_GetLightDirection(light);
    LightType lightType = Light_GetLightType(light);
    float lightCoef;

    Vec3 worldPos = in->worldPos;
    float gloss = in->gloss;

    // Transforme le coefficient de reflexion (de 0 à 1) en exposant spéculaire (de 2 à 128).
    float specularExponent = exp2(gloss * 6 + 1);

    Vec3 normal = in->normal;
    Vec3 view, half, reflect;

    switch (lightType) {
        case LIGHT_TYPE_DIFFUSE:
            // Normalize(L . V)
            lightCoef = Float_Clamp01(Vec3_Dot(lightVector, normal));
        break;

        case LIGHT_TYPE_SPECULAR_BLINN:
        // Normalize(V . H)
        view = Vec3_Normalize(Vec3_Sub(cameraPos, worldPos));
        reflect = Vec3_Reflect(Vec3_Opposite(lightVector), Vec3_Normalize(normal));
        lightCoef = Float_Clamp01(Vec3_Dot(view, reflect));
        lightCoef = powf(lightCoef, specularExponent);
        break;

        case LIGHT_TYPE_SPECULAR_BLINN_PHONG:
        // Normalize(N . H)
        view = Vec3_Normalize(Vec3_Sub(cameraPos, worldPos));
        half = Vec3_Normalize(Vec3_Add(lightVector, view));
        lightCoef = Float_Clamp01(Vec3_Dot(Vec3_Normalize(normal), half));
        lightCoef = powf(lightCoef, specularExponent);
        break;
    }

    lightCoef *= light->m_lightIntensity;

    return lightCoef;
}

void Light_Free(Light* light)
{
    free(light);
}

Light *Light_Create()
{
    Light *light = (Light*) calloc(1, sizeof(Light));
    Light_SetLightDirection(light, Vec3_Set(0.5, 1.0, 0.5));
    Light_SetLightColor(light, Vec3_Set(0.7f, 0.6f, 0.5f));
    Light_SetLightType(light, LIGHT_TYPE_DIFFUSE);
    Light_SetLightIntensity(light, 10);
    return light;
}