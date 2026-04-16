#pragma once

#include <vector>
#include <algorithm>
#include <functional>

template<typename... Args>
class TDelegate
{
	struct FListener
	{
		void* Class = nullptr;
		/** Unable to capture. Can't do .Bind(this, &Class::Func) 
			void (*Signature)(void*, Args...) = nullptr;		*/
		std::function<void(Args...)> Signature;
	};

public:
	template<typename T>
	void Bind(T* Object, void (T::*FuncSignature)(Args...))
	{
		FListener Listener;
		Listener.Class = Object;

		Listener.Signature = [Object, FuncSignature](Args... args) {
				(Object->*FuncSignature)(args...);
			};

		Listeners.push_back(Listener);
	}

	template<typename T>
	void Remove(T* Object)
	{
		auto RemovePredicate = [Object](const FListener& Listener) {
				return Listener.Class == Object;
			};

		std::erase_if(Listeners, RemovePredicate); //C++20 compiler required
	}

	void Broadcast(Args... args)
	{
		for (const FListener& Listener : Listeners)
		{
			if (Listener.Signature)
			{
				Listener.Signature(args...);
			}
		}
	}

	void Clear()
	{
		Listeners.clear();
	}

	bool IfBound() { return Listeners.size() > 0; }

private:
	std::vector<FListener> Listeners;
};