#ifndef __MACRO_H__
#define __MACRO_H__

#define SAFE_DELETE(p) { delete p; p = NULL; }
#define SAFE_DELETE_ARRAY(p) { delete [] p; p = NULL; }

#define NO_COPY(ClassName)						\
private:										\
	ClassName(const ClassName&);				\
	ClassName& operator = (const ClassName&);	\


#define DECLARE_SINGLETON(ClassName)		\
	NO_COPY(ClassName)						\
private:									\
	static ClassName*	m_pInstance;		\
public:										\
	static ClassName*	GetInstance(void)	\
	{										\
	if(m_pInstance == NULL)					\
		{									\
		m_pInstance = new ClassName;		\
		}									\
		return m_pInstance;					\
	}										\
	void DestroyInstance(void)				\
	{										\
	if(m_pInstance)							\
		{									\
		delete m_pInstance;					\
		m_pInstance = NULL;					\
		}									\
	}										\



#define IMPLEMENT_SINGLETON(ClassName)		\
ClassName*	ClassName::m_pInstance = NULL;	\

#endif