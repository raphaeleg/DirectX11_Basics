#include "textureclass.hpp"

TextureClass::TextureClass(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* filename) {
	bool success = LoadTarga32Bit(filename);
	if (!success) { return; }

	D3D11_TEXTURE2D_DESC textureDesc = SetTextureDesc();
	HRESULT result = device->CreateTexture2D(&textureDesc, NULL, &m_texture);
	if (FAILED(result)) { return; }

	unsigned int rowPitch = (m_width * 4) * sizeof(unsigned char);	// Set the row pitch of the targa image data.
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	success = SetSRVDesc(textureDesc.Format, device);
	if (!success) { return; }
	deviceContext->GenerateMips(m_textureView);

	delete[] m_targaData;
	m_targaData = 0;

	isInitialized = true;
}

D3D11_TEXTURE2D_DESC TextureClass::SetTextureDesc() const {
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Height = m_height;
	textureDesc.Width = m_width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	return textureDesc;
}

bool TextureClass::SetSRVDesc(DXGI_FORMAT format, ID3D11Device* device) {
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	HRESULT result = device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView);
	return !FAILED(result);
}

TextureClass::~TextureClass() {
	if (m_textureView) {
		m_textureView->Release();
		m_textureView = 0;
	}
	if (m_texture) {
		m_texture->Release();
		m_texture = 0;
	}
	if (m_targaData) {
		delete[] m_targaData;
		m_targaData = 0;
	}
}

TextureClass::fileData TextureClass::GetFileData(char* filename){
	fileData nll = fileData();
	FILE* filePtr;
	int error = fopen_s(&filePtr, filename, "rb");	// Open the targa file for reading in binary.
	if (error != 0) { return nll; }

	TargaHeader targaFileHeader;	// Read in the file header.
	unsigned int count = (unsigned int)fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if (count != 1) { return nll; }

	m_height = (int)targaFileHeader.height;
	m_width = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;
	if (bpp != 32) { return nll; }	// Check that it is 32 bit and not 24 bit.

	int imageSize = m_width * m_height * 4;
	unsigned char* targaImage = new unsigned char[imageSize];
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);	// Read in the targa image data.
	if (count != imageSize) { return nll; }

	error = fclose(filePtr);	// Close the file.
	if (error != 0) { return nll; }

	return fileData(imageSize, targaImage, true);
}

void TextureClass::SetTargaData(TextureClass::fileData data) {
	m_targaData = new unsigned char[data.imageSize];
	int targa_dst_data = 0;	// Initialize the index into the targa destination data array.
	int targa_img_data = (m_width * m_height * 4) - (m_width * 4);	// Initialize the index into the targa image data.

	// Now copy the targa image data into the targa destination array in the correct order since the targa format is stored upside down and also is not in RGBA order.
	for (int h = 0; h < m_height; h++) {
		for (int w = 0; w < m_width; w++) {
			m_targaData[targa_dst_data + 0] = data.targaImage[targa_img_data + 2];  // Red.
			m_targaData[targa_dst_data + 1] = data.targaImage[targa_img_data + 1];  // Green.
			m_targaData[targa_dst_data + 2] = data.targaImage[targa_img_data + 0];  // Blue
			m_targaData[targa_dst_data + 3] = data.targaImage[targa_img_data + 3];  // Alpha

			// Increment the indexes into the targa data.
			targa_img_data += 4;
			targa_dst_data += 4;
		}
		targa_img_data -= (m_width * 8);	// Set the targa image data index back to the preceding row at the beginning of the column since its reading it in upside down.
	}
}

bool TextureClass::LoadTarga32Bit(char* filename)
{
	auto data = GetFileData(filename);
	if (not data.isInitialized) { return false; }

	SetTargaData(data);
	
	delete[] data.targaImage;
	data.targaImage = 0;

	return true;
}