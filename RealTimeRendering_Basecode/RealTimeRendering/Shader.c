#include "Shader.h"
#include "Scene.h"
#include "Graphics.h"
#include "Tools.h"

VShaderOut VertexShader_Base(VShaderIn *in, VShaderGlobals *globals)
{
    // Le vertex shader est une fonction ex�cut�e pour chacun des sommets
    // d'un objet � rendre.
    // Il r�cup�re des donn�es globales � l'objet comme les matrices de conversion.
    // Il se charge principalement de projeter le sommet dans le "clip space"
    // et de convertir la normale/tangente associ�e dans le r�f�rentiel monde.
    // Le vertex shader est ex�cut� pour chacun des sommets d'un objet � rendre.
    VShaderOut out = { 0 };
    
    // Coordonn�es homog�nes du sommet (w = 1 car c'est un point)
    Vec4 vertex = Vec4_From3(in->vertex, 1.0f);

    // Coordonn�es homog�nes de la normale (w = 0 car c'est une direction)
    Vec4 normal = Vec4_From3(in->normal, 0.0f);

    // Coordonn�es homog�nes de la tangente (w = 0 car c'est une direction)
    Vec4 tangent = Vec4_From3(in->tangent, 0.0f);

    // Coordonn�es homog�nes de la bitangente (w = 0 car c'est une direction)
    Vec4 bitangent = Vec4_From3(in->bitangent, 0.0f);

    // Transformation du sommet dans le rep�re cam�ra
    Vec4 vertexCamSpace = Mat4_MulMV(globals->objToView, vertex);

    // Projection du sommet dans le "clip space"
    Vec4 vertexClipSpace = Mat4_MulMV(globals->objToClip, vertex); // OBLIGATOIRE (ne pas modifier)

    // Calcul de la bitangente
    in->bitangent = Vec3_Cross(in->tangent, in->normal);

    // Transformation de la normale dans le rep�re monde
    normal = Mat4_MulMV(globals->objToWorld, normal);

    // Transformation de la tangente dans le rep�re monde
    tangent = Mat4_MulMV(globals->objToWorld, tangent);

    // Transformation de la bitangente dans le rep�re monde
    bitangent = Mat4_MulMV(globals->objToWorld, bitangent);


    // Obtention de la position du sommet � partir de la normale
    Vec3 worldPos = Vec3_From4(Mat4_MulMV(globals->objToWorld, vertex));

    // D�finit la sortie du vertex shader
    out.clipPos = Vec3_From4(vertexClipSpace);  // OBLIGATOIRE (ne pas modifier)
    out.invDepth = vertexCamSpace.w / vertexCamSpace.z; // OBLIGATOIRE (ne pas modifier)
    out.normal = Vec3_Normalize(Vec3_From4(normal));
    out.textUV = in->textUV;
    out.worldPos = worldPos;
    out.tangent = Vec3_Normalize(Vec3_From4(tangent));
    out.bitangent = Vec3_Normalize(Vec3_From4(bitangent));

    return out;
}

Vec4 FragmentShader_Base(FShaderIn *in, FShaderGlobals *globals)
{
    // Le fragment shader est une fonction ex�cut�e pour chaque pixel
    // de tous les triangles visibles.
    // Il r�cup�re un fragment qui contient des donn�es interpol�es
    // (position, normale, tangente, uv).
    // Il retourne la couleur du pixel.
    //
    // Le vecteur (x,y,z,w) renvoy� par le fragment shader est interpr�t� comme suit :
    // - x : intensit� du rouge (entre 0 et 1)
    // - y : intensit� du vert  (entre 0 et 1)
    // - z : intensit� du bleu  (entre 0 et 1)
    // - w : opacit�            (entre 0 et 1)

    // R�cup�ration du mat�riau associ� au pixel (albedo/normal map/roughness map)
    Material *material = globals->material;
    assert(material);

    // R�cup�ration de la texture (albedo) associ�e au pixel
    MeshTexture *albedoTex = Material_GetAlbedo(material);
    assert(albedoTex);

    // R�cup�ration de la texture (normalMap) associ�e au pixel
    MeshTexture *normalTex = Material_GetNormalMap(material);

    // R�cup�ration des coordonn�es (u,v) associ�e au pixel.
    // Les coordonn�es (u,v) sont dans [0,1]^2.
    // - u repr�sente l'abscisse du point sur une texture 2D.
    // - v repr�sente l'ordonn�e du point sur une texture 2D.
    // (0,0) repr�sente le coin en bas � gauche.
    // (1,1) repr�sente le coin en haut � droite.
    float u = in->textUV.x;
    float v = in->textUV.y;

    // Recup�ration de la couleur du pixel dans la texture
    Vec3 albedo = MeshTexture_GetColorVec3(albedoTex, Vec2_Set(u, v));


    // R�cup�ration de la duret� (roughness) associ�e au pixel
    MeshTexture *roughnessTex = Material_GetRoughness(material);

    if (roughnessTex && Scene_GetRoughness(globals->scene)) {
        // Recup�ration de la valeur du pixel dans la roughnessMap
        Vec3 roughness = MeshTexture_GetColorVec3(roughnessTex, Vec2_Set(u, v));
        in->gloss = 1-roughness.x;
    }
    else in->gloss = 0.5;

    if (normalTex && Scene_GetNormal(globals->scene)) {
        Mat3 matrixTBN = {
            in->tangent.x, in->bitangent.x, in->normal.x,
            in->tangent.y, in->bitangent.y, in->normal.y,
            in->tangent.z, in->bitangent.z, in->normal.z,
        };
        Vec3 normalMap = MeshTexture_GetColorVec3(normalTex, Vec2_Set(u, v));

        in->normal = Mat3_MulMV(matrixTBN, normalMap);
    }

    // R�cup�re les lumi�res de la sc�ne
    Light **lights = Scene_GetLights(globals->scene);
    Vec3 lightColor = Light_GetLightColor(lights[0]);
    Vec3 ambiant = Scene_GetAmbiantColor(globals->scene);


    // Application de la lumi�re ambiante � l'albedo
    albedo = Vec3_Mul(albedo, ambiant);


    // Application des lumi�res diffuse/sp�culaires
    float lightCoef = 0;
    for (int i = 0; i < globals->scene->m_lighCount; ++i) {
        lightCoef += CalculateLightingCoefficient(lights[i], in, globals->cameraPos);
    }

    albedo = Vec3_Scale(albedo, lightCoef);
    albedo = Vec3_Mul(albedo, lightColor);


    // TODO
    // Pour la lumi�re sp�culaire de Blinn-Phong, il faut :
    // - r�cup�rer l'interpolation de la position dans le monde du pixel ;
    // - calculer le vecteur de vue, le vecteur moiti�
    // Vous devez donc modifier le vertex shader, puis modifier la fonction
    // Graphics_RenderTriangle() pour initialiser l'interpolation puis pour
    // calculer l'interpolation.
    // Utilisez les macros VEC3_INIT_INTERPOLATION() et VEC3_INTERPOLATE().

    //.............................................................................................
    // Quelques exemples de debug (� d�commenter)
    
    // Debug : couleur unique en RGBA (bleu ESIEA)
    //return Vec4_Set(0.21f, 0.66f, 0.88f, 1.0f);
    
    // Debug : coordonn�es (uv) de texture
    //return Vec4_From2(in->textUV, 0.0, 1.0f);

    // Debug : normales
    //return Vec4_From3(in->normal, 1.0f);
    
    // Debug : normales V2 
    //return Vec4_From3(Vec3_Scale(Vec3_Add(in->normal, Vec3_One), 0.5f), 1.0f);

    // Debug : Roughness
    // return Vec4_From3(Vec3_Scale(Vec3_Set(1, 1, 1), in->gloss), 1.0f);
    //.............................................................................................

    // Retourne la couleur (albedo) associ�e au pixel dans la texture.
    return Vec4_From3(albedo, 1.0f);
}
