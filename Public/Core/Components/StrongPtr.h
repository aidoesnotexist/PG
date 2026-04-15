#pragma once

#include <cstdint>
#include <utility>
#include "BasicTypes.h"

template<typename T>
class TStrongPtr
{
public:
	TStrongPtr() : ControlBlock(nullptr)
	{
	}
	TStrongPtr(std::nullptr_t) : ControlBlock(nullptr)
	{
	}
	~TStrongPtr()
	{
		Release();
	}

	TStrongPtr(const TStrongPtr& Other) : ControlBlock(Other.ControlBlock)
	{
		if (ControlBlock)
		{
			++ControlBlock->RefCount;
		}
	}
	TStrongPtr(TStrongPtr&& Other) noexcept : ControlBlock(Other.ControlBlock)
	{
		Other.ControlBlock = nullptr;
	}

	template<typename U>
	friend class TStrongPtr;

	template<typename U>
	TStrongPtr(const TStrongPtr<U>& Other)
	{
		static_assert(std::is_base_of_v<T, U>, "U must be derived from T");

		ControlBlock = Other.ControlBlock;

		if (ControlBlock)
		{
			++ControlBlock->RefCount;
		}
	}

	TStrongPtr& operator=(const TStrongPtr& Other)
	{
		if (this != &Other)
		{
			Release();
			ControlBlock = Other.ControlBlock;
			if (ControlBlock)
			{
				++ControlBlock->RefCount;
			}
		}
		return *this;
	}
	TStrongPtr& operator=(TStrongPtr&& Other) noexcept
	{
		if (this != &Other)
		{
			Release();
			ControlBlock = Other.ControlBlock;
			Other.ControlBlock = nullptr;
		}
		return *this;
	}
	TStrongPtr& operator=(std::nullptr_t)
	{
		Release();
		ControlBlock = nullptr;
		return *this;
	}

	explicit TStrongPtr(FControlBlock* NewControlBlock) : ControlBlock(NewControlBlock)
	{
	}

	explicit operator bool() const { return ControlBlock != nullptr; }

	bool operator ==(decltype(nullptr)) const { return ControlBlock == nullptr; }
	bool operator !=(decltype(nullptr)) const { return ControlBlock != nullptr; }

	bool IsValid() const { return ControlBlock; };
	T* Raw() const; //{ return ControlBlock ? static_cast<T*>(ControlBlock->Object) : nullptr; }
	bool CheckDelete();

	T* operator ->() const { return Raw(); }
	T& operator *() const { return *Raw(); }

	template<typename... Args>
	static TStrongPtr<T> MakeStrong(Args&&... args)
	{
		TStrongPtr<T> Ptr = nullptr;
		T* Allocation = new T(std::forward<Args>(args)...);
		Ptr.ControlBlock = new FControlBlock(Allocation);
		return Ptr;
	}

private:
	FControlBlock* ControlBlock;
	void Release();
};

template<typename T>
void TStrongPtr<T>::Release()
{
	if (ControlBlock && --ControlBlock->RefCount == 0)
	{
		delete ControlBlock;
	}
	ControlBlock = nullptr;
}

template<typename T>
inline T* TStrongPtr<T>::Raw() const
{
	if (ControlBlock && ControlBlock->Object)
	{
		return static_cast<T*>(ControlBlock->Object);
	}
	return nullptr;
}

template<typename T>
bool TStrongPtr<T>::CheckDelete()
{
	if (ControlBlock && ControlBlock->RefCount - 1 == 0)
	{
		Release();
		return true;
	}
	return false;
}