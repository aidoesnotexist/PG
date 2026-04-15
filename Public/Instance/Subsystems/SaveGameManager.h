#pragma once

#include "Instance/Subsystems/PGGameInstanceSubsystem.h"
#include "BasicTypes.h"

/* This should've been async load/write subsystem for user settings and game progress.
   Didn't have time to implement this. There are no user saves, it just parses static ini files.
   I'm also dropping level data(actors, their placements) here, so parsing comes from one place */
class USaveGameManager : public UPGGameInstanceSubsystem
{
public:
	~USaveGameManager() = default;

	void Initialize() override {}
	void Deinitialize() override {}

	void PostInit(UPGGameInstance* GameInstance) override {}

	//This is just render window settings. Not settings, that user would've been able to change via settings
	FWindowSettings RequestUserSettings() const;
	FMenuSettings RequestMenuSetupSettings() const;
	FGameSettings RequestGameSetupSettings() const;
	
	const sf::Font& LoadDefaultFont();

	EGameDifficulty GetSelectedGameDifficulty() const { return GameDifficulty; }
	void SetGameDifficulty(const EGameDifficulty& InGD) { GameDifficulty = InGD; }

protected:
	friend TStrongPtr<USaveGameManager>;
	USaveGameManager() = default;

	EGameDifficulty GameDifficulty = EGameDifficulty::Normal;

private:
	FColorSettings ParseColor(const std::string& Value) const;

	std::string GetUserSettingsPath() const;
	std::string GetDefaultFontPath() const;
	sf::Font DefaultFont;

	std::string GetRootPath() const;
};