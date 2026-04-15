#pragma once

#include "Core/Components/StrongPtr.h"

/** Base class for every game instance, except Renderer */
class UPGObject
{
public:
	virtual ~UPGObject() = default;

	template<typename T, typename... Args>
	friend TStrongPtr<T> CreateObject(Args&&... args);

protected:
	friend TStrongPtr<UPGObject>;
	UPGObject() = default;

	template<typename T, typename... Args>
	static TStrongPtr<T> Create(Args&&... args)
	{
		static_assert(std::is_base_of_v<UPGObject, T>, "T must inherit from UPGObject");
		return TStrongPtr<T>::MakeStrong(std::forward<Args>(args)...);
	}
};

template<typename T, typename... Args>
TStrongPtr<T> CreateObject(Args&&... args)
{
	return UPGObject::Create<T>(std::forward<Args>(args)...);
}