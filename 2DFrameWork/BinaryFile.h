#pragma once

class BinaryWriter
{
public:
	BinaryWriter();
	~BinaryWriter();

	void Open(wstring filePath, UINT openOption = CREATE_ALWAYS);
	void Close();

	void Bool(bool data);
	void Word(WORD data);
	void Int(int data);
	void UInt(UINT data);
	void Float(float data);
	void Double(double data);

	void vector2(const Vector2& data);
	void vector3(const Vector3& data);
	void vector4(const Vector4& data);
	void color3f(const Color& data);
	void color4f(const Color& data);
	void matrix(const  Matrix& data);

	void String(const string& data);
	void Byte(void* data, UINT dataSize);

protected:
	HANDLE fileHandle;
	DWORD size;
};

//////////////////////////////////////////////////////////////////////////

class BinaryReader
{
public:
	BinaryReader();
	~BinaryReader();

	bool Open(wstring filePath);
	void Close();

	//데이터를 원하는 타입으로 반환
	bool Bool();
	unsigned char Byte();
	WORD Word();
	int Int();
	UINT UInt();
	float Float();
	double Double();

	Vector2 vector2();
	Vector3 vector3();
	Vector4 vector4();
	Color color3f();
	Color color4f();
	Matrix matrix();

	string String();
	void Byte(void** data, UINT dataSize);

protected:
	HANDLE fileHandle;
public:
	DWORD size;
};