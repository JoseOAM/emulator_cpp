#ifndef __Instructions_h
#define __Instructions_h

// TODO, Need to have constraint related to Register or consider receiving Register structure
template<typename T>
void add(T* a0, T* a2, T* a3)
{
	a0->setRegister(a2->getRegister() + a3->getRegister());
}

#endif