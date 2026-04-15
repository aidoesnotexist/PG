#include "Instance/PGGameInstance.h"
#include "Actor/PGViewport.h"
#include "Instance/Subsystems/InputHandler.h"
#include "Instance/Subsystems/SaveGameManager.h"
#include "Instance/Subsystems/SoundHandler.h"
#include "BasicTypes.h"

UPGGameInstance* UPGGameInstance::Instance = nullptr;

UPGGameInstance* UPGGameInstance::Get()
{
	return Instance;
}

UPGGameInstance& UPGGameInstance::GetRef()
{
	return *Get();
}

void UPGGameInstance::Initialize()
{
	GISubsystems[EPGSubsystem::InputHandler] = CreateInitSubsystem<UInputHandler>();
	GISubsystems[EPGSubsystem::SaveGameManager] = CreateInitSubsystem<USaveGameManager>();
	GISubsystems[EPGSubsystem::SoundManager] = CreateInitSubsystem<USoundManager>();

	Viewport = CreateObject<APGViewport>();
	assert(Viewport && "Failed to create Viewport");
	Viewport->Init();
	Viewport->InitGameMode(CurrentPhase);

	Renderer = std::make_unique<RRender>();
	assert(Renderer && "Failed to create Render");
	Renderer->Init(Viewport.Raw(), GetSubsystem<UInputHandler>());
	Renderer->StartParallel();

	Viewport->OnMenuPlay.Bind(this, &UPGGameInstance::ProceedPlay);
	Viewport->OnMenuSettings.Bind(this, &UPGGameInstance::ProceedSettingsMenu);
	Viewport->OnMenuQuit.Bind(this, &UPGGameInstance::ProceedShutDown);

	if (UInputHandler* InputHandler = dynamic_cast<UInputHandler*>(GISubsystems[EPGSubsystem::InputHandler].Raw()))
	{
		InputHandler->PostInit(this);
	}
}

void UPGGameInstance::Deinitialize()
{
	Renderer->StopWithBlock();

	Viewport->OnMenuPlay.Remove(this);
	Viewport->OnMenuSettings.Remove(this);
	Viewport->OnMenuQuit.Remove(this);

	GISubsystems.clear();

	Viewport->Deinitialize();
}

void UPGGameInstance::BeginPlay()
{
	Renderer->bUpdateRender = true;
	GetSubsystem<USoundManager>()->PlayBackgroundAmbient(true);
	Viewport->BeginPlay();
	Viewport->Tick();

	Deinitialize();
}

void UPGGameInstance::RequestRenderStart()
{
	Renderer->bUpdateRender = true;
}

template<typename T>
TStrongPtr<T> UPGGameInstance::CreateInitSubsystem()
{
	TStrongPtr<T> SubsystemPtr;
	if (!std::is_base_of_v<UPGGameInstanceSubsystem, T>)
	{
		return SubsystemPtr;
	}
	
	SubsystemPtr = CreateObject<T>();
	if (SubsystemPtr.IsValid())
	{
		SubsystemPtr->Initialize();
		return SubsystemPtr;
	}

	return SubsystemPtr;
}

void UPGGameInstance::ProceedPlay()
{
	if (CurrentPhase == EGamePhase::Game)
	{
		Viewport->OnEscapeHandle();
		return;
	}

	CurrentPhase = EGamePhase::Game;
	bIsStreaming = true;
	Renderer->bUpdateRender = false;
	Renderer->ClearRenderCache();
	assert(GetSubsystem<UInputHandler>() && "InputHandler was NULL");
	Viewport->ClearWindowInputs();
	GetSubsystem<UInputHandler>()->ClearInputs();
}

void UPGGameInstance::ProceedSettingsMenu()
{

}

void UPGGameInstance::ProceedShutDown()
{
	bIsPendingExit = true;
}
