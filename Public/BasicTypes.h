#pragma once

#include <cstdint>
#include <SFML/Graphics.hpp>
#include <vector>

#define INDEX_NONE (-1)

class UPGObject;

struct FControlBlock
{
	FControlBlock(UPGObject* InObject) : Object(InObject), RefCount(1)
	{
	}
	~FControlBlock()
	{
		delete Object;
	}

	UPGObject* Object;
	int32_t RefCount;
};

struct FRenderTarget
{
	FRenderTarget() = default;
	~FRenderTarget() = default;

	FRenderTarget(/*const*/ sf::Drawable* InDrawable, const sf::Transform& InTransform)
		: Drawable(InDrawable), Transform(InTransform)
	{
	}

	sf::Transform Transform;
	/*const*/ sf::Drawable* Drawable = nullptr;
	bool bRenderOpacity = true;
};

struct FRenderData
{
	void AddTarget(const FRenderTarget& RenderTarget)
	{
		RenderTargets.push_back(RenderTarget);
	}

	const std::vector<FRenderTarget>& Get() const
	{
		return RenderTargets;
	}

	void Clear()
	{
		RenderTargets.clear();
	}

private:
	std::vector<FRenderTarget> RenderTargets;
};

struct FWindowSettings
{
	int Width = 1280;
	int Height = 720;
	bool bFullscreen = false;
	bool bVSync = true;
	int FPSCap = 60;
	std::string Title = "PongGame";
};

struct FColorSettings
{
	int R = 255;
	int G = 255;
	int B = 255;
	int A = 255;
};

inline sf::Color ToSFColor(const FColorSettings& Color)
{
	return sf::Color(Color.R, Color.G, Color.B, Color.A);
}

struct FMenuSettings
{
	float BackgroundSizeX = 1.f;
	float BackgroundSizeY = 1.f;

	float BackgroundPositionX = 0.f;
	float BackgroundPositionY = 0.f;

	int ButtonsNum = 0;

	float ButtonDefaultScaleX = 0.f;
	float ButtonDefaultScaleY = 0.f;

	float DifficaltyButtonScaleX = 0.f;
	float DifficaltyButtonScaleY = 0.f;

	float MenuButtonsDefaultPositionX = 0.f;
	float MenuButtonsDefaultPositionY = 0.f;

	float ButtonsYGap = 0.f;

	std::string ButtonContinue;
	std::string ButtonEasy;
	std::string ButtonNormal;
	std::string ButtonHard;
	std::string ButtonSettings;
	std::string ButtonQuit;

	int CharacterSize = 24;

	FColorSettings BackGroundColor;
	FColorSettings DefaultButtonColor;
	FColorSettings EasyButtonColor;
	FColorSettings NormalButtonColor;
	FColorSettings HardButtonColor;
	FColorSettings TextColor;
	FColorSettings LogoColor;
};

struct FGameSettings
{
	int PlayersNum = 2;

	float LocalPlayerDefaultPositionX = 0.f;
	float LocalPlayerDefaultPositionY = 0.f;

	float EnemyDefaultPositionX = 0.f;
	float EnemyDefaultPositionY = 0.f;

	float PlayerSpeed = 0.f;

	float EnemySpeedEasy = 150.0;
	float EnemySpeed = 0.f;
	float EnemySpeedHard = 500.0;

	float BallDefaultRadius = 1.f;

	float BallDefaultPositionX = 0.f;
	float BallDefaultPositionY = 0.f;

	float BallSpeedEasy = 250.0;
	float BallSpeed = 500.0;
	float BallSpeedHard = 750.0;

	float BallSpeedAccentionEasy = 1.01;
	float BallSpeedAccention = 1.1;
	float BallSpeedAccentionHard = 1.2;

	float BallInitialDirMinY = -0.7f;
	float BallInitialDirMaxY = 0.7f;

	float BackgroundSplit = 0.5f;
	FColorSettings LeftBGColor;
	FColorSettings RightBGColor;

	FColorSettings ScoreTextColor;
	int ScoreTextSize = 48;

	float CharacterWidth = 0.01f;
	float CharacterHeight = 0.2f;

	FColorSettings PlayerColor;
	FColorSettings EnemyColor;
	FColorSettings BallColor;

	float MiddleLineWidth = 0.005f;
	FColorSettings MiddleLineColor;
};

struct FGameDifficulty
{
	float EnemySpeed = 0;
	float BallSpeed = 0;
	float BallSpeedAccention = 0;
};

enum class EGamePhase : int8_t
{
	Menu,
	Game,
	TransitLevel
};

enum class EGameDifficulty : int8_t
{
	Easy,
	Normal,
	Hard
};

enum class EPGSubsystem : int8_t
{
	InputHandler,
	UIHandler,
	GameFlowManager,
	SaveGameManager,
	Animation2DManager,
	SoundManager
};

namespace EUserInput
{
	enum class I : int8_t
	{
		MoveForward,
		MoveRight
	};
	inline bool operator==(EUserInput::I E, int8_t I) { return static_cast<int8_t>(E) == I; }
	inline bool operator==(int8_t I, EUserInput::I E) { return I == static_cast<int8_t>(E); }
}

namespace EMouseInput
{
	enum class I : int8_t
	{
		MouseButtonClick,
		MouseButtonReleased
	};
	inline bool operator==(EMouseInput::I E, int8_t I) { return static_cast<int8_t>(E) == I; }
	inline bool operator==(int8_t I, EMouseInput::I E) { return I == static_cast<int8_t>(E); }
}