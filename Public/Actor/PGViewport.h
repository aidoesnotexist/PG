#pragma once

#include "Core/Object.h"
#include "Core/Components/Ticker.h"
#include "Core/Components/TDelegate.h"
#include <shared_mutex>
#include <vector>
#include <memory>

class APGActor;
class APGWidget;
class APGCharacter;
class APGPlayerController;
class TTicker;
class UPGGameInstance;
class UInputHandler;
enum class EGamePhase : int8_t;

/** This is not how the real viewport should be treated. It should be created and tied
to a specific actor with local player's PlayerController. 
Since it's just a simple 2D game APGViewport will represent the level(scene) */
class APGViewport : public UPGObject
{
public:
	~APGViewport() = default;

	virtual void Init();
	virtual void Deinitialize();

	//This mimics grabbing actors from level by just initializing them here
	virtual void InitGameMode(const EGamePhase& GamePhase);

	virtual void BeginPlay();

	virtual void Tick();

	void Exit();

	sf::RenderWindow* GetRenderWindow() const { return RenderWindow.get(); }
	std::unique_lock<std::shared_mutex> LockWindow() const;

	const std::vector<TStrongPtr<APGActor>>& GetViewportActors() const { return ViewportActors; }

	TDelegate<> OnMenuPlay;
	TDelegate<> OnMenuSettings;
	TDelegate<> OnMenuQuit;
	TDelegate<const FGameDifficulty&> OnGameDifficultyChanged;

	void OnEscapeHandle();

	const TStrongPtr<APGActor> GetBounceBall() { return BounceBall; }

	void ClearWindowInputs();

	sf::Vector2f GetRWindowSize() const { return RenderWindow ? sf::Vector2f(RenderWindow->getSize().x, RenderWindow->getSize().y) : sf::Vector2f(); }

	void OpenMenuMidGame();

protected:
	friend TStrongPtr<APGViewport>;
	APGViewport() = default;

	TTicker Ticker;

	void HandleButtonReleased(APGWidget* Button);

	TStrongPtr<APGActor> BounceBall;

	void HandleBounce(APGCharacter* Obstacle);
	void HandlePlayerScored();
	void HandleEnemyScored();

	sf::Text* PlayerScore = nullptr;
	sf::Text* EnemyScore = nullptr;

	void ResetGameDifficulty();
	FGameDifficulty SelectedGameDifficulty;

private:
	//RenderWindow needs to be created on game thread
	std::unique_ptr<sf::RenderWindow> RenderWindow;

	mutable std::shared_mutex RWMutex;

	std::vector<TStrongPtr<APGActor>> ViewportActors;

	void CreateRenderWindow(const FWindowSettings& WindowSettings);
	APGPlayerController* CreatePlayerController();

	UPGGameInstance* GameInstance = nullptr;
	UInputHandler* InputHandler = nullptr;

	void UpdateScoreText(sf::Text* ScoreText, int& ScoreValue, int Delta);
	int PlayerScoreValue, EnemyScoreValue;

	bool bGamePaused = false;

	void InitMenu(bool bVisible = true);
	void InitGame();
};