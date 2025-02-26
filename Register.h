#ifndef __Register_h
#define __Register_h

// TODO, needs to have constraint related to datatype or force int
template <typename T>
class Register
{
private:
	T* value;

public:
	Register()
	{
		value = new T;
		*value = 0;
	}

	~Register()
	{
		delete value;
	}

	void setRegister(T _value)
	{
		*value = _value;
	}

	T getRegister()
	{
		return *value;
	}
};

#endif