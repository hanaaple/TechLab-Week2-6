﻿#include "Texture.h"

// bitmapWidth : 전체 텍스처 너비, bitmapHeight : 전체 텍스처 높이
// rowSize : 한 셀의 세로 사이즈, colSize : 한 셀의 가로 사이즈
// rowNum : 셀의 행 개수, colNum : 셀의 열 개수
// cellWidthUV : 셀의 가로 너비(uv좌표계 기준) ex 0.06, CeillHeightUV : 셀의 세로 너비(uv 좌표계 기준)
void Texture::Initialize(const FTextureStaticData& TextureData)
{
    this->TextureData = TextureData;
    int RowCount = TextureData.RowCount;
    int ColCount = TextureData.ColCount;
    int AtlasTotalNum = TextureData.AtlasTotalCount;
    
    float CellWidthUV = 1.f / ColCount;
    float CellHeightUV = 1.f / RowCount;

    for (int idx = 0; idx < AtlasTotalNum; idx++)
    {
        int rowIndex = idx / ColCount;		// 현재 문자가 속한 행 인덱스
        int colIndex = idx % ColCount;		// 현재 문자가 속한 열 인덱스

        float u = colIndex * CellWidthUV;
        float v = (RowCount - rowIndex - 1) * CellHeightUV;
        
        AtlasInfoMap.Add(idx, { u, v, CellWidthUV, CellHeightUV });
    }
}

TextureInfo Texture::GetCharInfoMap(char character)
{
    return AtlasInfoMap[character - 32];
}
