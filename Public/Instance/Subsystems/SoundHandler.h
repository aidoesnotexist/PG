#pragma once

#include "Instance/Subsystems/PGGameInstanceSubsystem.h"
#include <SFML/Audio.hpp>
#include <filesystem>

class USoundManager : public UPGGameInstanceSubsystem
{
public:
	virtual void Initialize() override;
	virtual void Deinitialize() override;

	virtual void PostInit(UPGGameInstance* GameInstance) {}

	void ToggleAmbientPause();

	void PlayBackgroundAmbient(bool bLoop = true);

	void PlaySFX(const std::string& SoundName);

protected:
	friend TStrongPtr<USoundManager>;
	USoundManager() = default;

private:
	sf::Music BackgroundMusic;

	std::map<std::string, std::unique_ptr<sf::SoundBuffer>> SoundBuffers;

	static const int MAX_SOUNDS = 10;
	std::vector<sf::Sound> SFXPool;

	void LoadSound(const std::string& Name, const std::string& Path);

	std::string GetMusicPath() const;
	std::string GetWallBouncePath() const;
	std::string GetCollisionBouncePath() const;

	std::string GetRootPath() const;
};