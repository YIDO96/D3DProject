#include "Framework.h"

//////////////////////////////////////////////////////////////////////////

BinaryWriter::BinaryWriter()
	: fileHandle(NULL), size(0)
{

}

BinaryWriter::~BinaryWriter()
{

}

void BinaryWriter::Open(wstring filePath, UINT openOption)
{
	assert(filePath.length() > 0);
	fileHandle = CreateFile
	(
		filePath.c_str()
		, GENERIC_WRITE
		, 0
		, NULL
		, openOption
		, FILE_ATTRIBUTE_NORMAL
		, NULL
	);


	bool isChecked = fileHandle != INVALID_HANDLE_VALUE;
	assert(isChecked);
}

void BinaryWriter::Close()
{
	if (fileHandle != NULL)
	{
		CloseHandle(fileHandle);
		fileHandle = NULL;
	}
}

void BinaryWriter::Bool(bool data)
{
	WriteFile(fileHandle, &data, sizeof(bool), &size, NULL);
}

void BinaryWriter::Word(WORD data)
{
	WriteFile(fileHandle, &data, sizeof(WORD), &size, NULL);
}

void BinaryWriter::Int(int data)
{
	WriteFile(fileHandle, &data, sizeof(int), &size, NULL);
}

void BinaryWriter::UInt(UINT data)
{
	WriteFile(fileHandle, &data, sizeof(UINT), &size, NULL);
}

void BinaryWriter::Float(float data)
{
	WriteFile(fileHandle, &data, sizeof(float), &size, NULL);
}

void BinaryWriter::Double(double data)
{
	WriteFile(fileHandle, &data, sizeof(double), &size, NULL);
}

void BinaryWriter::vector2(const Vector2& data)
{
	WriteFile(fileHandle, &data, sizeof(Vector2), &size, NULL);
}

void BinaryWriter::vector3(const Vector3& data)
{
	WriteFile(fileHandle, &data, sizeof(Vector3), &size, NULL);
}

void BinaryWriter::vector4(const Vector4& data)
{
	WriteFile(fileHandle, &data, sizeof(Vector4), &size, NULL);
}

void BinaryWriter::color3f(const Color& data)
{
	WriteFile(fileHandle, &data, sizeof(Color) - 4, &size, NULL);
}

void BinaryWriter::color4f(const Color& data)
{
	WriteFile(fileHandle, &data, sizeof(Color), &size, NULL);
}

void BinaryWriter::matrix(const Matrix& data)
{
	WriteFile(fileHandle, &data, sizeof(Matrix), &size, NULL);
}

void BinaryWriter::String(const string& data)
{
	UInt((UINT)data.size());

	const char* str = data.c_str();
	WriteFile(fileHandle, str, (DWORD)data.size(), &size, NULL);
}

void BinaryWriter::Byte(void* data, UINT dataSize)
{
	WriteFile(fileHandle, data, dataSize, &size, NULL);
}

//////////////////////////////////////////////////////////////////////////

BinaryReader::BinaryReader()
	: fileHandle(NULL), size(0)
{

}

BinaryReader::~BinaryReader()
{

}

bool BinaryReader::Open(wstring filePath)
{
	if (filePath.length() <= 0)
	{
		return false;
	}
	assert(filePath.length() > 0);
	fileHandle = CreateFile
	(
		filePath.c_str()
		, GENERIC_READ
		, FILE_SHARE_READ
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL
	);
	

	bool isChecked = fileHandle != INVALID_HANDLE_VALUE;
	assert(isChecked);
	return !isChecked;
}

void BinaryReader::Close()
{
	if (fileHandle != NULL)
	{
		CloseHandle(fileHandle);
		fileHandle = NULL;
	}
}

bool BinaryReader::Bool()
{
	bool temp = false;
	ReadFile(fileHandle, &temp, sizeof(bool), &size, NULL);

	return temp;
}

unsigned char BinaryReader::Byte()
{
	unsigned char temp = 0;
	ReadFile(fileHandle, &temp, sizeof(unsigned char), &size, NULL);

	return temp;
}

WORD BinaryReader::Word()
{
	WORD temp = 0;
	ReadFile(fileHandle, &temp, sizeof(WORD), &size, NULL);

	return temp;
}

int BinaryReader::Int()
{
	int temp = 0;
	ReadFile(fileHandle, &temp, sizeof(int), &size, NULL);

	return temp;
}

UINT BinaryReader::UInt()
{
	UINT temp = 0;
	ReadFile(fileHandle, &temp, sizeof(UINT), &size, NULL);

	return temp;
}

float BinaryReader::Float()
{
	float temp = 0.0f;
	ReadFile(fileHandle, &temp, sizeof(float), &size, NULL);

	return temp;
}

double BinaryReader::Double()
{
	double temp = 0.0f;
	ReadFile(fileHandle, &temp, sizeof(double), &size, NULL);

	return temp;
}

Vector2 BinaryReader::vector2()
{
	float x = Float();
	float y = Float();

	return Vector2(x, y);
}

Vector3 BinaryReader::vector3()
{
	float x = Float();
	float y = Float();
	float z = Float();

	return Vector3(x, y, z);
}

Vector4 BinaryReader::vector4()
{
	float x = Float();
	float y = Float();
	float z = Float();
	float w = Float();

	return Vector4(x, y, z, w);
}

Color BinaryReader::color3f()
{
	float r = Float();
	float g = Float();
	float b = Float();

	return Color(r, g, b, 1.0f);
}

Color BinaryReader::color4f()
{
	float r = Float();
	float g = Float();
	float b = Float();
	float a = Float();

	return Color(r, g, b, a);
}

Matrix BinaryReader::matrix()
{
	Matrix matrix;
	matrix._11 = Float();	matrix._12 = Float();	matrix._13 = Float();	matrix._14 = Float();
	matrix._21 = Float();	matrix._22 = Float();	matrix._23 = Float();	matrix._24 = Float();
	matrix._31 = Float();	matrix._32 = Float();	matrix._33 = Float();	matrix._34 = Float();
	matrix._41 = Float();	matrix._42 = Float();	matrix._43 = Float();	matrix._44 = Float();

	return matrix;
}

string BinaryReader::String()
{
	UINT size = Int();

	char* temp = new char[size + 1];
	ReadFile(fileHandle, temp, sizeof(char) * size, &this->size, NULL); //데이터 읽기
	temp[size] = '\0';

	return temp;
}

void BinaryReader::Byte(void** data, UINT dataSize)
{
	ReadFile(fileHandle, *data, dataSize, &size, NULL);
}
