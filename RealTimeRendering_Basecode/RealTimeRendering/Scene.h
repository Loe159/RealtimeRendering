﻿#pragma once

/// @file Scene.h
/// @defgroup Scene
/// @{

#include "Settings.h"
#include "Camera.h"
#include "Window.h"
#include "Renderer.h"
#include "Tools.h"
#include "Vector.h"
#include "Object.h"
#include "Graphics.h"
#include "Shader.h"
#include "Light.h"

/// @brief Structure représentant une scène 3D.
/// Contient la racine de l'arbre de scène ainsi qu'une caméra par laquelle la scène sera rendue.
typedef struct Scene_s
{
    Renderer *m_renderer;
    Camera *m_camera;
    Object *m_root;

    Mesh **m_meshes;
    int m_meshCount;
    int m_meshCapacity;

    Light **m_lights;
    int m_lighCount;
    Vec3 m_ambiantColor;

    VertexShader *m_defaultVShader;
    FragmentShader *m_defaultFShader;

    bool m_wireframe;
    bool m_roughness;
    bool m_normal;
} Scene;

//-------------------------------------------------------------------------------------------------
// Création de la scène et des objets

/// @brief Crée une nouvelle scène vide avec une racine et une caméra.
/// @param window la fenêtre contenant le contexte de rendu.
/// @return La scène créée ou NULL en cas d'erreur.
Scene *Scene_New(Window *window);

/// @brief Détruit une scène.
/// Les objets présents dans l'arbre de scène sont supprimés récursivement.
/// Attention, cette fonction ne libère pas les meshs associés puisqu'un mesh peut être
/// associé à plusieurs objets.
/// @param scene La scène à détruire.
void Scene_Free(Scene *scene);

/// @brief Alloue un objet.
/// L'objet doit ensuite être initialiser.
/// @param scene la scène.
/// @param size la taille de l'objet en octets.
/// @return Un pointeur vers l'objet alloué.
Object *Scene_CreateObject(Scene *scene, int size);

/// @brief Supprime un objet de la scène.
/// Supprime également récursivement tous les enfants de l'objet.
/// Il est conseillé de mettre le pointeur de l'objet à NULL après l'appel à cette fonction.
/// @param[in] scene la scène.
/// @param[in,out] object l'objet à supprimer.
void Scene_RemoveObject(Scene *scene, Object *object);

/// @brief Charge un mesh (et les matériaux associés) depuis un fichier OBJ.
/// @param scene la scène
/// @param folderPath le chemin du dossier contenant le fichier OBJ.
/// @param fileName le nom du fichier OBJ.
/// @return Un pointeur vers le mesh créé ou NULL en cas d'erreur.
Mesh* Scene_CreateMeshFromOBJ(Scene *scene, char *folderPath, char *fileName);

/// @brief Renvoie la racine de l'arbre d'une scène.
/// @param scene la scène.
/// @return L'objet à la racine de la scène.
Object *Scene_GetRoot(Scene *scene);

/// @brief Renvoie la caméra présente dans une scène.
/// @param scene la scène.
/// @return La caméra de la scène.
Camera *Scene_GetCamera(Scene *scene);

/// @brief Ajoute une lumière à la scène
void Scene_AddLight(Scene *scene, Light *light);

//-------------------------------------------------------------------------------------------------
// Paramètres de rendu

/// @brief Définit le vertex shader à utiliser par défaut lors du rendu d'un objet.
/// @param[in,out] scene la scène.
/// @param[in] defaultVShader le vertex shader.
INLINE void Scene_SetDefaultVertexShader(
    Scene *scene, VertexShader *defaultVShader)
{
    scene->m_defaultVShader = defaultVShader;
}

/// @brief Définit le fragment shader à utiliser par défaut lors du rendu d'un objet.
/// @param[in,out] scene la scène.
/// @param[in] defaultFShader le fragment shader.
INLINE void Scene_SetDefaultFragmentShader(
    Scene *scene, FragmentShader *defaultFShader)
{
    scene->m_defaultFShader = defaultFShader;
}

/// @brief Définit si la scène doit être rendue en "fil de fer" ou avec un fragement shader.
/// @param[in,out] scene la scène.
/// @param wireframe booléen indiquant si la scène doit être rendue en fil de fer.
INLINE void Scene_SetWireframe(Scene *scene, bool wireframe)
{
    scene->m_wireframe = wireframe;
}

/// @brief Renvoie un booléen indiquant si la scène doit être rendue en fil de fer.
/// @param[in] scene la scène.
/// @return Un booléen indiquant si la scène doit être rendue en fil de fer.
INLINE bool Scene_GetWireframe(Scene *scene)
{
    return scene->m_wireframe;
}

/// @brief Définit si la scène doit être rendue avec la roughness map.
/// @param[in,out] scene la scène.
/// @param roughness booléen indiquant si la scène doit être rendue avec la roughness map.
INLINE void Scene_SetRoughness(Scene *scene, bool roughness)
{
    scene->m_roughness = roughness;
}

/// @brief Renvoie un booléen indiquant si la scène doit être rendue avec la roughness map.
/// @param[in] scene la scène.
/// @return Un booléen indiquant si la scène doit être rendue avec la roughness map.
INLINE bool Scene_GetRoughness(Scene *scene)
{
    return scene->m_roughness;
}

/// @brief Définit si la scène doit être rendue avec la normal map.
/// @param[in,out] scene la scène.
/// @param normal booléen indiquant si la scène doit être rendue avec la normal map.
INLINE void Scene_SetNormal(Scene *scene, bool normal)
{
    scene->m_normal = normal;
}

/// @brief Renvoie un booléen indiquant si la scène doit être rendue avec la normal map.
/// @param[in] scene la scène.
/// @return Un booléen indiquant si la scène doit être rendue avec la normal map.
INLINE bool Scene_GetNormal(Scene *scene)
{
    return scene->m_normal;
}

/// @brief Calcul le rendu de la scène vue par sa caméra.
/// @param scene la scène dont il faut calculer le rendu.
void Scene_Render(Scene *scene);

//-------------------------------------------------------------------------------------------------
INLINE Light **Scene_GetLights(Scene *scene)
{
    return scene->m_lights;
}

void Scene_FreeLights(Scene *scene);


INLINE void Scene_SetAmbiantColor(Scene *scene, Vec3 color)
{
    scene->m_ambiantColor = Vec3_Clamp01(color);
}

INLINE Vec3 Scene_GetAmbiantColor(Scene *scene)
{
    return scene->m_ambiantColor;
}
/// @}
