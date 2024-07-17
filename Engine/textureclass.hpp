#pragma once
#include <d3d11.h>
#include <stdio.h>

class TextureClass {
public:
    TextureClass(ID3D11Device*, ID3D11DeviceContext*, char*);
    TextureClass(const TextureClass&) { isInitialized = true; }
    ~TextureClass();

    ID3D11ShaderResourceView* GetTexture() { return m_textureView; }
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    bool isInitialized = false;

private:
    struct TargaHeader {
        unsigned char data1[12];
        unsigned short width;
        unsigned short height;
        unsigned char bpp;
        unsigned char data2;
    };
    D3D11_TEXTURE2D_DESC SetTextureDesc() const;
    bool SetSRVDesc(DXGI_FORMAT format, ID3D11Device* device);
    bool LoadTarga32Bit(char*);
    
    unsigned char* m_targaData = 0;
    ID3D11Texture2D* m_texture = 0;
    ID3D11ShaderResourceView* m_textureView = 0;
    int m_width = 0;
    int m_height = 0;

    struct fileData {
        int imageSize = 0;
        unsigned char* targaImage{};
        bool isInitialized = false;

        fileData() {};
        fileData(int i, unsigned char* t, bool init) : imageSize(i), targaImage(t), isInitialized(init) {};
    };

    fileData GetFileData(char* fileName);
    void SetTargaData(fileData data);
};
