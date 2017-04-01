#pragma once

class __CEncryptResManager
{
	__CEncryptResManager();
	~__CEncryptResManager();

	__CEncryptResManager(const __CEncryptResManager&);
	__CEncryptResManager& operator=(const __CEncryptResManager&);

	ZyJSingleton(__CEncryptResManager);

public:
	void																				EncryptData(unsigned char* pData, const long lFileLength);
	void																				DecryptData(unsigned char* pData, const long lFileLength);

	std::string																			GetFileExtension(const unsigned char* pData);

private:
	unsigned short																		SeedRandMap(const unsigned short wSeed);

private:

private:
	static const unsigned char															s_bytePngHeader[4];
	static const unsigned char															s_byteBmpHeader[2];
	static const unsigned char															s_byteGifHeader[3];
	static const unsigned char															s_byteJpgHeader[10];
	static const unsigned char															s_byteWaveHeader[4];
	static const unsigned char															s_byteMp3Header[4];
	static const unsigned char															s_byteMp3StreamHeader[4];

	static const unsigned int															s_dwFirstKey;
	static const unsigned int															s_dwSecondKey;

	static const unsigned char															s_byteEncryptByteMap[256];
	static const unsigned char															s_byteDecryptByteMap[256];
};

typedef ZyJTools::Singleton<__CEncryptResManager> CEncryptResManager;