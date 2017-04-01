#pragma once

namespace ZyJTools
{

	template<class TSingleton>
	class __Singleton
	{
		__Singleton();
		__Singleton(const __Singleton&);
		__Singleton& operator = (const __Singleton&);

	public:
		static TSingleton& GetSingleton()
		{
			static std::unique_ptr<TSingleton> m_Singleton(new TSingleton);
			return *m_Singleton;
		}
	};

	template<class TSingleton>
	class Singleton : public __Singleton <TSingleton>
	{
	};

}

#define ZyJSingleton($x) \
	friend class ::ZyJTools::__Singleton<$x>; \
	friend struct ::std::default_delete<$x>