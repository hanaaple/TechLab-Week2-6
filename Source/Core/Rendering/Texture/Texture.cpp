#include "Texture.h"

// bitmapWidth : 전체 텍스처 너비, bitmapHeight : 전체 텍스처 높이
// rowSize : 한 셀의 세로 사이즈, colSize : 한 셀의 가로 사이즈
// rowNum : 셀의 행 개수, colNum : 셀의 열 개수
// cellWidthUV : 셀의 가로 너비(uv좌표계 기준) ex 0.06, CeillHeightUV : 셀의 세로 너비(uv 좌표계 기준)
void Texture::Initialize(const FTextureStaticData& TextureData)
{
    float BitmapWidth = TextureData.BitmapWidth;
    float BitmapHeight = TextureData.BitmapHeight;
    int RowCount = TextureData.RowCount;
    int ColCount = TextureData.ColCount;
    int AtlasTotalNum = TextureData.AtlasTotalCount;
    
    float CellWidthUV = (float)ColCount / BitmapWidth;
    float CellHeightUV = (float)RowCount / BitmapHeight;

    for (int idx = 0; idx < AtlasTotalNum; idx++)
    {
        int rowIndex = idx / ColCount;		// 현재 문자가 속한 행 인덱스
        int colIndex = idx % ColCount;		// 현재 문자가 속한 열 인덱스

        // col * colSize : 한 칸 가로 길이 (픽셀 단위)
        // row * rowSize : 한 칸 세로 길이 (픽셀 단위)
        // bitmapWidth, bitmapHeight : 0 ~ 1로 정규화.
        // directX는 좌표계까 좌측 상단부터(0, 0)이라서 방향 반전.
        
        float u = (colIndex * ColCount) / BitmapWidth;
        //float v = ((row * rowSize) / bitmapHeight);
        float v = 1.0f - ((rowIndex * RowCount) / BitmapHeight) - CellHeightUV;
        
        AtlasInfoMap.Add(idx, { u, v, CellWidthUV, CellHeightUV });
    }
}

CharacterInfo Texture::GetCharInfoMap(char character)
{
    return AtlasInfoMap[character];
}
