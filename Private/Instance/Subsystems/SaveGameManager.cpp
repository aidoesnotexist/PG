#include "Instance/Subsystems/SaveGameManager.h"
#include "INIReader.h"
#include <filesystem>

FWindowSettings USaveGameManager::RequestUserSettings() const
{
	FWindowSettings Settings;

	INIReader Parser(GetUserSettingsPath());

	if (Parser.ParseError() < 0)
	{
		return Settings;
	}

	Settings.Width = Parser.GetInteger("Window", "Width", Settings.Width);
	Settings.Height = Parser.GetInteger("Window", "Height", Settings.Height);

	Settings.bFullscreen = Parser.GetInteger("Window", "Fullscreen", Settings.bFullscreen ? 1 : 0) != 0;
	Settings.bVSync = Parser.GetInteger("Window", "VSync", Settings.bVSync ? 1 : 0) != 0;

	Settings.FPSCap = Parser.GetInteger("Window", "FPSCap", Settings.FPSCap);

	Settings.Title = Parser.Get("Window", "Title", Settings.Title);

	return Settings;
}

FMenuSettings USaveGameManager::RequestMenuSetupSettings() const
{
	FMenuSettings Settings;

	INIReader Reader(GetUserSettingsPath());

	Settings.BackgroundSizeX = Reader.GetReal("Menu", "BackgroundSizeX", 1.f);
	Settings.BackgroundSizeY = Reader.GetReal("Menu", "BackgroundSizeY", 1.f);

	Settings.BackgroundPositionX = Reader.GetReal("Menu", "BackgroundPositionX", 0.f);
	Settings.BackgroundPositionY = Reader.GetReal("Menu", "BackgroundPositionY", 0.f);

	Settings.ButtonDefaultScaleX = Reader.GetReal("Menu", "ButtonDefaultScaleX", 0.f);
	Settings.ButtonDefaultScaleY = Reader.GetReal("Menu", "ButtonDefaultScaleY", 0.f);

	Settings.DifficaltyButtonScaleX = Reader.GetReal("Menu", "DifficaltyButtonScaleX", 0.f);
	Settings.DifficaltyButtonScaleY = Reader.GetReal("Menu", "DifficaltyButtonScaleY", 0.f);

	Settings.MenuButtonsDefaultPositionX = Reader.GetReal("Menu", "MenuButtonsDefaultPositionX", 0.f);
	Settings.MenuButtonsDefaultPositionY = Reader.GetReal("Menu", "MenuButtonsDefaultPositionY", 0.f);

	Settings.ButtonsYGap = Reader.GetReal("Menu", "ButtonsYGap", 0.f);

	//Settings.ButtonsNum = Reader.GetInteger("Menu", "ButtonsNum", 0);

	Settings.ButtonContinue = Reader.Get("Menu", "ButtonContinue", "");
	Settings.ButtonEasy = Reader.Get("Menu", "ButtonEasy", "");
	Settings.ButtonNormal = Reader.Get("Menu", "ButtonNormal", "");
	Settings.ButtonHard = Reader.Get("Menu", "ButtonHard", "");
	Settings.ButtonSettings = Reader.Get("Menu", "ButtonSettings", "");
	Settings.ButtonQuit = Reader.Get("Menu", "ButtonQuit", "");

	Settings.CharacterSize = Reader.GetInteger("Menu", "CharacterSize", Settings.CharacterSize);

	Settings.BackGroundColor = ParseColor(Reader.Get("Menu", "BackGroundColor", "255,255,255,255"));
	Settings.DefaultButtonColor = ParseColor(Reader.Get("Menu", "DefaultButtonColor", "255,255,255,255"));
	Settings.EasyButtonColor = ParseColor(Reader.Get("Menu", "EasyButtonColor", "255,255,255,255"));
	Settings.NormalButtonColor = ParseColor(Reader.Get("Menu", "NormalButtonColor", "255,255,255,255"));
	Settings.HardButtonColor = ParseColor(Reader.Get("Menu", "HardButtonColor", "255,255,255,255"));
	Settings.TextColor = ParseColor(Reader.Get("Menu", "TextColor", "255,255,255,255"));
	Settings.LogoColor = ParseColor(Reader.Get("Menu", "LogoColor", "255,255,255,255"));

	return Settings;
}

FGameSettings USaveGameManager::RequestGameSetupSettings() const
{
	FGameSettings Settings;

	INIReader Reader(GetUserSettingsPath());

	Settings.PlayersNum = Reader.GetInteger("Game", "PlayersNum", 2);

	Settings.LocalPlayerDefaultPositionX = Reader.GetReal("Game", "LocalPlayerDefaultPositionX", 0.f);
	Settings.LocalPlayerDefaultPositionY = Reader.GetReal("Game", "LocalPlayerDefaultPositionY", 0.f);

	Settings.EnemyDefaultPositionX = Reader.GetReal("Game", "EnemyDefaultPositionX", 0.f);
	Settings.EnemyDefaultPositionY = Reader.GetReal("Game", "EnemyDefaultPositionY", 0.f);

	Settings.PlayerSpeed = Reader.GetReal("Game", "PlayerSpeed", 0.f);

	Settings.EnemySpeedEasy = Reader.GetReal("Game", "EnemySpeedEasy", 0.f);
	Settings.EnemySpeed = Reader.GetReal("Game", "EnemySpeed", 0.f);
	Settings.EnemySpeedHard = Reader.GetReal("Game", "EnemySpeedHard", 0.f);

	Settings.BallDefaultRadius = Reader.GetReal("Game", "BallDefaultRadius", 0.f);

	Settings.BallDefaultPositionX = Reader.GetReal("Game", "BallDefaultPositionX", 0.f);
	Settings.BallDefaultPositionY = Reader.GetReal("Game", "BallDefaultPositionY", 0.f);

	Settings.BallSpeedEasy = Reader.GetReal("Game", "BallSpeedEasy", 0.f);
	Settings.BallSpeed = Reader.GetReal("Game", "BallSpeed", 0.f);
	Settings.BallSpeedHard = Reader.GetReal("Game", "BallSpeedHard", 0.f);

	Settings.BallSpeedAccentionEasy = Reader.GetReal("Game", "BallSpeedAccentionEasy", 0.f);
	Settings.BallSpeedAccention = Reader.GetReal("Game", "BallSpeedAccention", 0.f);
	Settings.BallSpeedAccentionHard = Reader.GetReal("Game", "BallSpeedAccentionHard", 0.f);

	Settings.BallInitialDirMinY = Reader.GetReal("Game", "BallInitialDirMinY", -0.7f);
	Settings.BallInitialDirMaxY = Reader.GetReal("Game", "BallInitialDirMaxY", 0.7f);

	Settings.BackgroundSplit = Reader.GetReal("Game", "BackgroundSplit", 0.5f);

	Settings.LeftBGColor = ParseColor(Reader.Get("Game", "LeftBGColor", "30,30,30,255"));

	Settings.RightBGColor = ParseColor(Reader.Get("Game", "RightBGColor", "50,50,50,255"));

	Settings.ScoreTextColor = ParseColor(Reader.Get("Game", "ScoreTextColor", "255,255,255,255"));

	Settings.ScoreTextSize = Reader.GetInteger("Game", "ScoreTextSize", 48);

	Settings.CharacterWidth = Reader.GetReal("Game", "CharacterWidth", 0.01f);
	Settings.CharacterHeight = Reader.GetReal("Game", "CharacterHeight", 0.2f);

	Settings.PlayerColor = ParseColor(Reader.Get("Game", "PlayerColor", "255,255,255,255"));

	Settings.EnemyColor = ParseColor(Reader.Get("Game", "EnemyColor", "255,255,255,255"));

	Settings.BallColor = ParseColor(Reader.Get("Game", "BallColor", "255,255,255,255"));

	Settings.MiddleLineWidth = Reader.GetReal("Game", "MiddleLineWidth", 0.005f);

	Settings.MiddleLineColor = ParseColor(Reader.Get("Game", "MiddleLineColor", "255,255,255,255"));

	return Settings;
}

FColorSettings USaveGameManager::ParseColor(const std::string& Value) const
{
	FColorSettings Color;

	int r = 255, g = 255, b = 255, a = 255;

	sscanf_s(Value.c_str(), "%d,%d,%d,%d", &r, &g, &b, &a);

	Color.R = r;
	Color.G = g;
	Color.B = b;
	Color.A = a;

	return Color;
}

const sf::Font& USaveGameManager::LoadDefaultFont()
{
	DefaultFont.loadFromFile(GetDefaultFontPath());
	return DefaultFont;
}

std::string USaveGameManager::GetUserSettingsPath() const
{
	return GetRootPath() + "/Saved/DefaultSettings.ini";
}

std::string USaveGameManager::GetDefaultFontPath() const
{
	return GetRootPath() + "/Saved/one_piece_font.ttf";
}

std::string USaveGameManager::GetRootPath() const
{
	return std::filesystem::current_path().string();
}
