#pragma once
#include "Core/Container/Array.h"
#include "DataTypes/Structs.h"

class VertexDataCreator
{
public:
    static TArray<FVertexSimple> CreateConeVertices();
    static TArray<FVertexSimple> CreateCylinderVertices();
    static TArray<FVertexSimple> CreateTorusVertices();
    static TArray<uint32> CreateTorusIndecies();


private:
    
    // Torus
private:
    static constexpr uint32 majorSegments = 32;
    static constexpr uint32 minorSegments = 16;
    static constexpr float majorRadius = 5;
    static constexpr float minorRadius = 1;		
};
