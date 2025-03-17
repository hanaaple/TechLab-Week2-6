#include "MeshResourceCache.h"

#include "MeshVertexData.h"
#include "VertexDataCreator.h"

const TArray<FVertexSimple>* MeshResourceCache::GetVertexData(EPrimitiveMeshType MeshType)
{
    if (!VertexDataMap.Contains(MeshType))
    {
        switch (MeshType)
        {
        case EPrimitiveMeshType::EPT_None:
            break;
        case EPrimitiveMeshType::EPT_Triangle:
            VertexDataMap.Add(MeshType, TriangleVertices);
            break;
        case EPrimitiveMeshType::EPT_Cube:
            VertexDataMap.Add(MeshType, CubeVertices);
            break;
        case EPrimitiveMeshType::EPT_Sphere:
            VertexDataMap.Add(MeshType, SphereVertices);
            break;
        case EPrimitiveMeshType::EPT_Line:
            VertexDataMap.Add(MeshType, LineVertices);
            break;
        case EPrimitiveMeshType::EPT_BoundingBox:
            VertexDataMap.Add(MeshType, BoundingBoxVertices);
            break;
        case EPrimitiveMeshType::EPT_GridLine:
            //VertexDataMap.Add(MeshType, LineGridVertices);
            break;
        case EPrimitiveMeshType::EPT_Cylinder:
            VertexDataMap.Add(MeshType, VertexDataCreator::CreateCylinderVertices());
            break;
        case EPrimitiveMeshType::EPT_Cone:
            VertexDataMap.Add(MeshType, VertexDataCreator::CreateConeVertices());
            break;
        case EPrimitiveMeshType::EPT_Torus:
            VertexDataMap.Add(MeshType, VertexDataCreator::CreateTorusVertices());
            break;
        case EPrimitiveMeshType::EPT_Max:
            break;
        default: ;
        }
    }
    
    if (VertexDataMap.Contains(MeshType))
    {
        return &VertexDataMap[MeshType];
    }

    return nullptr;
}

const TArray<uint32>* MeshResourceCache::GetIndexData(EPrimitiveMeshType MeshType)
{
    if (!IndexDataMap.Contains(MeshType))
    {
        switch (MeshType)
        {
        case EPrimitiveMeshType::EPT_None:
            break;
        case EPrimitiveMeshType::EPT_Triangle:
            break;
        case EPrimitiveMeshType::EPT_Cube:
            IndexDataMap.Add(MeshType, CubeIndecies);
            break;
        case EPrimitiveMeshType::EPT_Sphere:
            break;
        case EPrimitiveMeshType::EPT_Line:
            break;
        case EPrimitiveMeshType::EPT_BoundingBox:
            IndexDataMap.Add(MeshType, BoundingBoxIndecies);
            break;
        case EPrimitiveMeshType::EPT_GridLine:
            break;
        case EPrimitiveMeshType::EPT_Cylinder:
            break;
        case EPrimitiveMeshType::EPT_Cone:
            break;
        case EPrimitiveMeshType::EPT_Torus:
            IndexDataMap.Add(MeshType, VertexDataCreator::CreateTorusIndecies());
            break;
        case EPrimitiveMeshType::EPT_Max:
            break;
        default: ;
        }
    }

    if (IndexDataMap.Contains(MeshType))
    {
        return &IndexDataMap[MeshType];
    }

    return nullptr;
}
