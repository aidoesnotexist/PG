#include "Instance/Subsystems/SoundHandler.h"

void USoundManager::Initialize()
{
	SFXPool.resize(MAX_SOUNDS);
	LoadSound("WallBounce", GetWallBouncePath());
	LoadSound("CharacterBounce", GetCollisionBouncePath());
}

void USoundManager::Deinitialize()
{
	BackgroundMusic.stop();

	for (auto& Sound : SFXPool)
	{
		Sound.stop();
		Sound.resetBuffer();
	}

	SoundBuffers.clear();
}

void USoundManager::ToggleAmbientPause()
{
	if (BackgroundMusic.getStatus() == sf::Music::Playing)
	{
		BackgroundMusic.pause();
	}
	else if (BackgroundMusic.getStatus() == sf::Music::Paused)
	{
		BackgroundMusic.play();
	}
}

void USoundManager::LoadSound(const std::string& Name, const std::string& Path)
{
	auto Buffer = std::make_unique<sf::SoundBuffer>();
	if (!Buffer->loadFromFile(Path))
	{
		printf("Error");
		return;
	}
	SoundBuffers[Name] = std::move(Buffer);
}

std::string USoundManager::GetMusicPath() const
{
	return GetRootPath() + "/Saved/MRAmbient.ogg";
}

std::string USoundManager::GetWallBouncePath() const
{
	return GetRootPath() + "/Saved/WallBounce.wav";
}

std::string USoundManager::GetCollisionBouncePath() const
{
	return GetRootPath() + "/Saved/CharacterBounce.wav";
}

std::string USoundManager::GetRootPath() const
{
	return std::filesystem::current_path().string();
}

void USoundManager::PlayBackgroundAmbient(bool bLoop)
{
	if (BackgroundMusic.openFromFile(GetMusicPath()))
	{
		BackgroundMusic.setLoop(bLoop);
		BackgroundMusic.setVolume(30.f);
		BackgroundMusic.play();
	}
}

void USoundManager::PlaySFX(const std::string& SoundName)
{
	if (SoundBuffers.find(SoundName) == SoundBuffers.end())
	{
		return;
	}

	for (auto& Sound : SFXPool)
	{
		if (Sound.getStatus() != sf::Sound::Playing)
		{
			Sound.setBuffer(*SoundBuffers[SoundName]);
			Sound.play();
			return;
		}
	}
}