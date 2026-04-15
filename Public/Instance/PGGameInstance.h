#pragma once

#include "Instance/PGInstance.h"
#include "Core/Components/StrongPtr.h"
#include "Instance/Subsystems/PGGameInstanceSubsystem.h"
#include "Game/Render.h"
#include <map>
#include <memory>

enum class EPGSubsystem : int8_t;
enum class EGamePhase : int8_t;
class APGViewport;

class UPGGameInstance : public UPGInstance
{
public:
	static UPGGameInstance* Get();
	static UPGGameInstance& GetRef();

	virtual ~UPGGameInstance() override = default;

	virtual void Initialize() override;
	virtual void Deinitialize() override;

	virtual void BeginPlay();

	template<typename T>
	T* GetSubsystem() const;

	TStrongPtr<APGViewport> GetViewport() const { return Viewport; }

	const RRender* const GetRender() const { return Renderer.get(); }

	bool bIsPendingExit = false;
	bool bIsStreaming = false;

	EGamePhase CurrentPhase = EGamePhase::Menu;

	void RequestRenderStart();

protected:
	/** Didn't figure out how to bypass this pattern. I want to allocate new objects
		only from object types, but StrongPtr won't have access in this case*/
	friend TStrongPtr<UPGGameInstance>;
	UPGGameInstance()
	{
		if (!Instance)
		{
			Instance = this;
		}
		else if (Instance != this)
		{
			assert(false && "Attemted multiple instances creation");
		}
	}

	static UPGGameInstance* Instance;

private:
	std::map<EPGSubsystem, TStrongPtr<UPGGameInstanceSubsystem>> GISubsystems;

	template<typename T>
	TStrongPtr<T> CreateInitSubsystem();

	std::unique_ptr<RRender> Renderer;
	TStrongPtr<APGViewport> Viewport;

	void ProceedPlay();
	void ProceedSettingsMenu();
	void ProceedShutDown();
};

template<typename T>
T* UPGGameInstance::GetSubsystem() const
{
	for (const std::pair<const EPGSubsystem, TStrongPtr<UPGGameInstanceSubsystem>>& Subsystem : GISubsystems)
	{
		if (T* Found = dynamic_cast<T*>(Subsystem.second.Raw()))
		{
			return Found;
		}
	}
	return nullptr;
}
