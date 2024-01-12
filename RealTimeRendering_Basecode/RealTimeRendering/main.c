#include "Settings.h"
#include "Window.h"
#include "Camera.h"
#include "Matrix.h"
#include "Vector.h"
#include "Scene.h"
#include "Graphics.h"
#include "Renderer.h"
#include "Shader.h"
#include "Tools.h"
#include "Mesh.h"
#include "Material.h"

int main(int argc, char *argv[])
{
    Window *window = NULL;
    Renderer *renderer = NULL;
    Scene *scene = NULL;
    Mesh *mesh = NULL;

    // Initialise la SDL et crée la fenêtre
    int exitStatus = Settings_InitSDL();
    if (exitStatus != EXIT_SUCCESS) goto ERROR_LABEL;

    window = Window_New();
    if (!window) goto ERROR_LABEL;

    renderer = Window_getRenderer(window);

    g_time = Timer_New();
    if (!g_time) goto ERROR_LABEL;

    // Crée la scène
    scene = Scene_New(window);
    if (!scene) goto ERROR_LABEL;

    //mesh = Scene_CreateMeshFromOBJ(scene, "../Obj/Jaxy", "Jaxy.obj");
    // mesh = Scene_CreateMeshFromOBJ(scene, "../Obj/CaptainToad", "CaptainToad.obj");
    // mesh = Scene_CreateMeshFromOBJ(scene, "../Obj/Cube", "Cube.obj");
    mesh = Scene_CreateMeshFromOBJ(scene, "../Obj/Ibijau", "Ibijau.obj");
    if (!mesh) goto ERROR_LABEL;

    // Arbre de scène
    Object *root   = Scene_GetRoot(scene);
    Camera *camera = Scene_GetCamera(scene);

    // Crée l'objet
    Object *object = Scene_CreateObject(scene, sizeof(Object));
    if (!object) goto ERROR_LABEL;

    exitStatus = Object_Init(object, scene, Mat4_Identity, root);
    if (exitStatus != EXIT_SUCCESS) goto ERROR_LABEL;

    Object_SetMesh(object, mesh);

    // Calcule une échelle normalisée pour l'objet
    Vec3 meshCenter = mesh->m_center;
    Vec3 meshMin = mesh->m_min;
    Vec3 meshMax = mesh->m_max;
    float xSize = fabsf(meshMax.x - meshMin.x);
    float ySize = fabsf(meshMax.y - meshMin.y);
    float zSize = fabsf(meshMax.z - meshMin.z);
    float objectSize = fmaxf(xSize, fmaxf(ySize, zSize));
    float scale = 3.0f / objectSize;

    // Centre l'objet en (0,0,0) et applique l'échelle
    Mat4 objectTransform = Mat4_Identity;
    objectTransform = Mat4_GetTranslationMatrix(Vec3_Neg(mesh->m_center));
    objectTransform = Mat4_MulMM(Mat4_GetScaleMatrix(scale), objectTransform);
    Object_SetLocalTransform(object, objectTransform);

    // Obtention de la première lumière
    Light *light = Scene_GetLights(scene)[0];

    // Lancement du temps global
    Timer_Start(g_time);

    // Paramètre initiaux de la caméra
    float camDistance = 7.3f;
    float cameraX = 0.0f;
    float cameraY = 0.0f;

    float lightY = 0.0f;
    float lightXZ = 0.0f;

    float fpsAccu = 0.0f;
    int frameCount = 0;
    bool quit = false;
    while (!quit)
    {
        SDL_Event evt;
        SDL_Scancode scanCode;
        const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);


        // Met à jour le temps global
        Timer_Update(g_time);

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_MOUSEWHEEL:
                // Controle de l'intensité de la lumière
                if (keyboardState[SDL_SCANCODE_LCTRL]) {
                    if(evt.wheel.y > 0) if (light->m_lightIntensity>0) light->m_lightIntensity --;
                    if(evt.wheel.y < 0) light->m_lightIntensity ++;
                }
                // Controle du zoom
                else {
                    if(evt.wheel.y > 0) camDistance = fmaxf(0, camDistance-1.f);
                    if(evt.wheel.y < 0) camDistance += 1.f;
                }

                break;
            case SDL_MOUSEMOTION:
                // Turn around the 3D object
                if (evt.button.button == SDL_BUTTON_MIDDLE || evt.button.button == SDL_BUTTON_LEFT) {
                    cameraX -= evt.motion.yrel;
                    cameraY -= evt.motion.xrel;
                }
                // Move the light direction
                if (keyboardState[SDL_SCANCODE_LCTRL]) {
                    lightY += evt.motion.yrel * 0.01;
                    lightXZ += evt.motion.xrel * 0.01;
                }
                break;
            case SDL_QUIT:
                quit = true;
                break;

            case SDL_KEYDOWN:
                scanCode = evt.key.keysym.scancode;
                if (evt.key.repeat)
                    break;

                switch (scanCode)
                {
                case SDL_SCANCODE_ESCAPE:
                    quit = true;
                    break;
                case SDL_SCANCODE_Q:
                    light = Light_Create();
                    Scene_AddLight(scene, light);
                    break;
                case SDL_SCANCODE_SPACE:
                    Scene_SetWireframe(scene, !Scene_GetWireframe(scene));
                    break;
                case SDL_SCANCODE_L:
                    Light_CycleLightType(light);
                    break;
                case SDL_SCANCODE_R:
                    Scene_SetRoughness(scene, !Scene_GetRoughness(scene));
                    printf("Roughness : %d\n", Scene_GetRoughness(scene));
                    break;
                case SDL_SCANCODE_N:
                    Scene_SetNormal(scene, !Scene_GetNormal(scene));
                    printf("Normal : %d\n", Scene_GetNormal(scene));
                    break;
                default:
                    break;
                }
                break;

            default:
                break;
            }
        }

        // Calcule la matrice locale de la caméra
        Mat4 cameraModel = Mat4_Identity;
        cameraModel = Mat4_MulMM(
            Mat4_GetTranslationMatrix(Vec3_Set(0.f, 0.f, camDistance)),
            cameraModel
        );
        cameraModel = Mat4_MulMM(
            Mat4_GetXRotationMatrix(cameraX),
            cameraModel
        );
        cameraModel = Mat4_MulMM(
            Mat4_GetYRotationMatrix(cameraY),
            cameraModel
        );


        Vec3 lightDir;

        // Met à jour la direction de la lumière
        lightDir.x = -cosf(lightXZ);
        lightDir.y = Float_Clamp(-lightY, -2, 2);
        lightDir.z = sin(lightXZ);

        Light_SetLightDirection(light,Vec3_Normalize(lightDir));

        // Applique la matrice locale de la caméra
        Object_SetTransform(((Object *)camera), Scene_GetRoot(scene), cameraModel);

        // Calcule le rendu de la scène dans un buffer
        Scene_Render(scene);

        // Met à jour le rendu (affiche le buffer précédent)
        Renderer_Update(renderer);

        // Calcule les FPS
        fpsAccu += Timer_GetDelta(g_time);
        frameCount++;
        if (fpsAccu > 1.0f)
        {
            printf("FPS = %.1f\n", (float)frameCount / fpsAccu);
            fpsAccu = 0.0f;
            frameCount = 0;
        }
    }

    Scene_Free(scene);
    Scene_FreeLights(scene);
    Timer_Free(g_time);
    Window_Free(window);

    Settings_QuitSDL();

    return EXIT_SUCCESS;

ERROR_LABEL:
    printf("ERROR - main()\n");
    assert(false);
    Scene_Free(scene);
    Timer_Free(g_time);
    Window_Free(window);
    return EXIT_FAILURE;
}