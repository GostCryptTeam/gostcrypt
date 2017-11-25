/*
 Copyright (c) 2008 TrueCrypt Developers Association. All rights reserved.

 Governed by the TrueCrypt License 3.0 the full text of which is contained in
 the file License.txt included in TrueCrypt binary and source code distribution
 packages.
*/


#ifndef GST_HEADER_Platform_SharedVal
#define GST_HEADER_Platform_SharedVal

#include <QMutex>

namespace GostCrypt
{
	template <class T>
	class SharedVal
	{
	public:
		SharedVal () { }
		explicit SharedVal (T value) : Value (value) { }
		virtual ~SharedVal () { }

		operator T ()
		{
			return Get ();
		}

		T Decrement ()
		{
            ValMutex.lock();
			T r = --Value;
            ValMutex.unlock();
			return r;
		}

		T Get ()
		{
            ValMutex.lock();
			T r = Value;
            ValMutex.unlock();
			return r;
		}

		T Increment ()
		{
            ValMutex.lock();
			T r = ++Value;
            ValMutex.unlock();
			return r;
		}

		void Set (T value)
		{
            ValMutex.lock();
			Value = value;
            ValMutex.unlock();
		}

	protected:
		volatile T Value;
        QMutex ValMutex;

	private:
		SharedVal (const SharedVal &);
		SharedVal &operator= (const SharedVal &);
	};
}

#endif // GST_HEADER_Platform_SharedVal
