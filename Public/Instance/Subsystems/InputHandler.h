#pragma once

#include "Instance/Subsystems/PGGameInstanceSubsystem.h"
#include "Actor/PGPlayerController.h"
#include "Core/Components/StrongPtr.h"

class UPGGameInstance;
class APGViewport;

/*
	GI Subsystem that works with render window and polls input events.
	Delegates all events to Playercontroller
*/
class UInputHandler : public UPGGameInstanceSubsystem
{
public:
	void Initialize() override;
	void Deinitialize() override;

	void PostInit(UPGGameInstance* GameInstance) override;

	void RegisterInputs(const APGViewport* Viewport);
	void ClearInputs();

	void SetAttachedPlayerController(TStrongPtr<APGPlayerController>& InPlayerController) { PlayerController = InPlayerController; }
	APGPlayerController* GetAttachedPlayerController() const { return PlayerController.Raw(); }

	bool DestroyPlayerController();

protected:
	friend TStrongPtr<UInputHandler>;
	UInputHandler() = default;

private:
	TStrongPtr<APGPlayerController> PlayerController;
	UPGGameInstance* GameInstance = nullptr;
};