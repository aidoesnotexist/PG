#include "Actor/PGViewport.h"
#include "Instance/PGGameInstance.h"
#include "Instance/Subsystems/SaveGameManager.h"
#include "Instance/Subsystems/InputHandler.h"
#include "Instance/Subsystems/SoundHandler.h"
#include "Actor/PGControlledCharacter.h"
#include "Actor/PGEnemyCharacter.h"
#include "Actor/PGBounceBall.h"
#include "Actor/PGActor.h"
#include "Actor/PGWidget.h"

void APGViewport::Init()
{
	GameInstance = UPGGameInstance::Get();
	assert(GameInstance && "GameInstance was NULLPTR");

	FWindowSettings WindowSettings;
	USaveGameManager* SaveGameManager = GameInstance->GetSubsystem<USaveGameManager>();
	if (SaveGameManager)
	{
		WindowSettings = SaveGameManager->RequestUserSettings();
	}

	InputHandler = GameInstance->GetSubsystem<UInputHandler>();

	CreateRenderWindow(WindowSettings);
	assert(RenderWindow && "Window was not created");
}

void APGViewport::InitGameMode(const EGamePhase& GamePhase)
{
	if (APGPlayerController* PlayerController = CreatePlayerController())
	{
		if (GamePhase == EGamePhase::Menu)
		{
			InitMenu();
		}
		else if (GamePhase == EGamePhase::Game)
		{
			InitGame();
			InitMenu(false);
		}
		else if (GamePhase == EGamePhase::TransitLevel)
		{
			//For viewport which is rendered while async streaming, but we don't do this here
		}
	}
}

void APGViewport::Deinitialize()
{
	for (auto It = ViewportActors.begin(); It != ViewportActors.end(); )
	{
		TStrongPtr<APGActor>& Actor = *It;

		if (Actor.IsValid())
		{
			APGActor* ActorPtr = Actor.Raw();
			if (APGWidget* Widget = dynamic_cast<APGWidget*>(ActorPtr))
			{
				Widget->OnMouseReleased.Remove(this);
			}
			if (APGBounceBall* BounceBall = dynamic_cast<APGBounceBall*>(ActorPtr))
			{
				BounceBall->OnBounce.Remove(this);
				BounceBall->OnPlayerScored.Remove(this);
				BounceBall->OnEnemyScored.Remove(this);
			}
		}

		It = ViewportActors.erase(It);
	}
}

void APGViewport::BeginPlay()
{
	for (TStrongPtr<APGActor>& Actor : ViewportActors)
	{
		if (Actor.IsValid())
		{
			Actor->BeginPlay();
		}
	}
}

void APGViewport::Tick()
{
	while (RenderWindow->isOpen())
	{
		if (GameInstance->bIsStreaming && InputHandler->GetAttachedPlayerController() == nullptr)
		{
			Deinitialize();
			InitGameMode(GameInstance->CurrentPhase);
			GameInstance->bIsStreaming = false;
			GameInstance->RequestRenderStart();
			Ticker.Reset();
			BeginPlay();
			continue;
		}

		if (GameInstance->bIsStreaming)
		{
			if (InputHandler->DestroyPlayerController())
			{
				continue;
			}
		}

		if ((GameInstance->bIsPendingExit || GameInstance->bIsStreaming) && InputHandler->GetAttachedPlayerController())
		{
			continue;
		}

		if (!Ticker.IsTicking())
		{
			Ticker.Start();
		}

		const float FrameDelta = Ticker.GetElapsedTime();

		if (InputHandler)
		{
			InputHandler->RegisterInputs(this);
			if (APGPlayerController* PlayerController = InputHandler->GetAttachedPlayerController())
			{
				PlayerController->Tick(FrameDelta);
			}

		}

		for (TStrongPtr<APGActor>& Actor : ViewportActors)
		{
			if (Actor.IsValid() && Actor->GetActorRenderTarget().bRenderOpacity)
			{
				Actor->Tick(FrameDelta);
			}
		}

		if (const RRender* const Render = GameInstance->GetRender())
		{
			if (Render->bUpdateRender)
			{
				Render->CacheRenderData();
			}
		}

		if (GameInstance->bIsPendingExit)
		{
			Exit();
		}
	}
}

void APGViewport::Exit()
{
	if (InputHandler && InputHandler->GetAttachedPlayerController())
	{
		APGPlayerController* PlayerController = InputHandler->GetAttachedPlayerController();
		PlayerController->OnEscape.Remove(this);
	}

	if (RenderWindow)
	{
		RenderWindow->close();
	}
}

std::unique_lock<std::shared_mutex> APGViewport::LockWindow() const
{
	/*RenderWindow->setActive(false);*/
	return std::unique_lock<std::shared_mutex>(RWMutex);
}

void APGViewport::OnEscapeHandle()
{
	if (GameInstance && GameInstance->CurrentPhase == EGamePhase::Game)
	{
		bGamePaused = !bGamePaused;
		ResetGameDifficulty();
		OpenMenuMidGame();
		if (USoundManager* SM = GameInstance->GetSubsystem<USoundManager>())
		{
			SM->ToggleAmbientPause();
		}
	}
}

void APGViewport::ClearWindowInputs()
{
	sf::Event ClearEvents;
	while (RenderWindow->pollEvent(ClearEvents)) {}
}

void APGViewport::OpenMenuMidGame()
{
	for (TStrongPtr<APGActor>& Actor : ViewportActors)
	{
		if (Actor.IsValid())
		{
			FRenderTarget& RT = Actor->GetActorRenderTarget();
			FRenderTarget& RText = Actor->GetActorRenderText();
			if (RT.Drawable)
			{
				RT.bRenderOpacity = !RT.bRenderOpacity;
			}
			if (RText.Drawable)
			{
				RText.bRenderOpacity = !RText.bRenderOpacity;
			}
		}
	}
}

void APGViewport::HandleButtonReleased(APGWidget* Button)
{
	if (!Button)
	{
		return;
	}

	//Hardcode unhover other buttons
	for (TStrongPtr<APGActor>& Widget : ViewportActors)
	{
		if (APGWidget* WidgetPtr = dynamic_cast<APGWidget*>(Widget.Raw()))
		{
			if (WidgetPtr != Button)
			{
				WidgetPtr->SetUnhovered();
			}
		}
	}

	USaveGameManager* SaveGameManager = GameInstance->GetSubsystem<USaveGameManager>();
	if (!SaveGameManager)
	{
		return;
	}

	const FMenuSettings& MenuSettings = SaveGameManager->RequestMenuSetupSettings();

	const std::string& ButtonText = Button->GetWidgetText();
	if (ButtonText == MenuSettings.ButtonContinue)
	{
		OnMenuPlay.Broadcast();
	}
	if (ButtonText == MenuSettings.ButtonSettings)
	{
		OnMenuSettings.Broadcast();
	}
	if (ButtonText == MenuSettings.ButtonQuit)
	{
		OnMenuQuit.Broadcast();
	}

	if (ButtonText == MenuSettings.ButtonEasy)
	{
		SaveGameManager->SetGameDifficulty(EGameDifficulty::Easy);
	}
	if (ButtonText == MenuSettings.ButtonNormal)
	{
		SaveGameManager->SetGameDifficulty(EGameDifficulty::Normal);
	}
	if (ButtonText == MenuSettings.ButtonHard)
	{
		SaveGameManager->SetGameDifficulty(EGameDifficulty::Hard);
	}

	if (ButtonText == MenuSettings.ButtonEasy || ButtonText == MenuSettings.ButtonNormal || ButtonText == MenuSettings.ButtonHard)
	{
		ResetGameDifficulty();
	}
}

void APGViewport::HandleBounce(APGCharacter* Obstacle)
{
	if (dynamic_cast<APGControlledCharacter*>(Obstacle))
	{
		UpdateScoreText(PlayerScore, PlayerScoreValue, 1);
	}
	else if (dynamic_cast<APGEnemyCharacter*>(Obstacle))
	{
		UpdateScoreText(EnemyScore, EnemyScoreValue, 1);
	}
	if (USoundManager* SM = GameInstance->GetSubsystem<USoundManager>())
	{
		//SM->PlaySFX("CharacterBounce");
	}
}

void APGViewport::HandlePlayerScored()
{
	UpdateScoreText(EnemyScore, EnemyScoreValue, -10);
	UpdateScoreText(PlayerScore, PlayerScoreValue, 1);
	if (USoundManager* SM = GameInstance->GetSubsystem<USoundManager>())
	{
		//SM->PlaySFX("CharacterBounce");
	}
}

void APGViewport::HandleEnemyScored()
{
	UpdateScoreText(PlayerScore, PlayerScoreValue, -10);
	UpdateScoreText(EnemyScore, EnemyScoreValue, 1);
	if (USoundManager* SM = GameInstance->GetSubsystem<USoundManager>())
	{
		//SM->PlaySFX("CharacterBounce");
	}
}

void APGViewport::ResetGameDifficulty()
{
	if (!GameInstance)
	{
		return;
	}
	
	if (USaveGameManager* SaveGameManager = GameInstance->GetSubsystem<USaveGameManager>())
	{
		const EGameDifficulty& SelectedDefficuclty = SaveGameManager->GetSelectedGameDifficulty();
		const FGameSettings& Settings = SaveGameManager->RequestGameSetupSettings();
		switch (SelectedDefficuclty)
		{
		case EGameDifficulty::Easy:
			SelectedGameDifficulty.EnemySpeed = Settings.EnemySpeedEasy;
			SelectedGameDifficulty.BallSpeed = Settings.BallSpeedEasy;
			SelectedGameDifficulty.BallSpeedAccention = Settings.BallSpeedAccentionEasy;
			break;
		case EGameDifficulty::Normal:
			SelectedGameDifficulty.EnemySpeed = Settings.EnemySpeed;
			SelectedGameDifficulty.BallSpeed = Settings.BallSpeed;
			SelectedGameDifficulty.BallSpeedAccention = Settings.BallSpeedAccention;
			break;
		case EGameDifficulty::Hard:
			SelectedGameDifficulty.EnemySpeed = Settings.EnemySpeedHard;
			SelectedGameDifficulty.BallSpeed = Settings.BallSpeedHard;
			SelectedGameDifficulty.BallSpeedAccention = Settings.BallSpeedAccentionHard;
			break;
		}

		OnGameDifficultyChanged.Broadcast(SelectedGameDifficulty);
	}
}

void APGViewport::CreateRenderWindow(const FWindowSettings& WindowSettings)
{
	std::unique_lock<std::shared_mutex> Lock(RWMutex);

	sf::Uint32 Style = WindowSettings.bFullscreen ? sf::Style::Fullscreen : sf::Style::Default;
	RenderWindow = std::make_unique<sf::RenderWindow>(sf::VideoMode(WindowSettings.Width, WindowSettings.Height), WindowSettings.Title, Style);
	RenderWindow->setVerticalSyncEnabled(WindowSettings.bVSync);
	RenderWindow->setActive(false);
}

APGPlayerController* APGViewport::CreatePlayerController()
{
	TStrongPtr<APGPlayerController> PlayerController = CreateObject<APGPlayerController>();
	assert(PlayerController && "Failed to create PlayerController");
	if (GameInstance->CurrentPhase == EGamePhase::Game)
	{
		PlayerController->OnEscape.Bind(this, &APGViewport::OnEscapeHandle);
	}
	PlayerController->GetActorRenderTarget().bRenderOpacity = false;
	PlayerController->GetActorRenderText().bRenderOpacity = false;
	if (GameInstance && InputHandler)
	{
		InputHandler->SetAttachedPlayerController(PlayerController);
	}

	return PlayerController.Raw();
}

void APGViewport::UpdateScoreText(sf::Text* ScoreText, int& ScoreValue, int Delta)
{
	if (!ScoreText) return;

	ScoreValue += Delta;
	if (ScoreValue < 0) ScoreValue = 0;

	ScoreText->setString(std::to_string(ScoreValue));
}

void APGViewport::InitMenu(bool bVisible)
{
	USaveGameManager* SaveGameManager = GameInstance->GetSubsystem<USaveGameManager>();
	if (!SaveGameManager)
	{
		return;
	}

	FMenuSettings MenuSettings = SaveGameManager->RequestMenuSetupSettings();
	const sf::Vector2u RWSize = RenderWindow->getSize();

	TStrongPtr<APGWidget> Background = CreateObject<APGWidget>();
	if (Background)
	{
		Background->bIsFocusable = false;

		sf::RectangleShape* BGShape = new sf::RectangleShape();
		BGShape->setSize(sf::Vector2f(MenuSettings.BackgroundSizeX * RWSize.x, MenuSettings.BackgroundSizeY * RWSize.y));
		sf::Color BGColor = sf::Color(MenuSettings.BackGroundColor.R, MenuSettings.BackGroundColor.G,
			MenuSettings.BackGroundColor.B, MenuSettings.BackGroundColor.A);
		BGShape->setFillColor(BGColor);

		sf::Transform BGTransform;
		BGTransform = BGTransform.translate(MenuSettings.BackgroundPositionX * RWSize.x, MenuSettings.BackgroundPositionY * RWSize.y);

		FRenderTarget& RTarget = Background->GetActorRenderTarget();
		RTarget = FRenderTarget(BGShape, BGTransform);
		RTarget.bRenderOpacity = bVisible;
		ViewportActors.push_back(Background);
	}
	sf::Vector2f BDefaultPosition = sf::Vector2f(MenuSettings.MenuButtonsDefaultPositionX * RWSize.x, MenuSettings.MenuButtonsDefaultPositionY * RWSize.y);
	sf::Vector2f BDefaultSize = sf::Vector2f(MenuSettings.ButtonDefaultScaleX * RWSize.x, MenuSettings.ButtonDefaultScaleY * RWSize.y);
	sf::Vector2f BDfcltSize = sf::Vector2f(MenuSettings.DifficaltyButtonScaleX * RWSize.x, MenuSettings.DifficaltyButtonScaleY * RWSize.y);
	const float Skip = MenuSettings.ButtonsYGap * RWSize.y;
	int CharSize = MenuSettings.CharacterSize;
	const sf::Font& Fnt = SaveGameManager->LoadDefaultFont();
	sf::Color LogoColor = sf::Color(MenuSettings.LogoColor.R, MenuSettings.LogoColor.G, MenuSettings.LogoColor.B, MenuSettings.LogoColor.A);

	sf::Vector2f LogoDefaultSize = sf::Vector2f(MenuSettings.ButtonDefaultScaleX * RWSize.x, MenuSettings.ButtonDefaultScaleY * RWSize.y);

	sf::Vector2f LogoDefaultPosition = sf::Vector2f(BDefaultPosition.x, BDefaultPosition.y - LogoDefaultSize.y - Skip);

	TStrongPtr<APGWidget> Logo = CreateObject<APGWidget>();
	if (Logo)
	{
		Logo->SetUnhovered();

		sf::Text* LogoText = new sf::Text();
		LogoText->setString("PongGame");
		LogoText->setFont(Fnt);
		LogoText->setFillColor(LogoColor);
		LogoText->setCharacterSize(CharSize * 2.f);

		sf::FloatRect LogoBounds = LogoText->getLocalBounds();

		sf::Vector2f LogoTextPos;
		LogoTextPos.x = LogoDefaultPosition.x + (LogoDefaultSize.x - LogoBounds.width) * 0.5f - LogoBounds.left;
		LogoTextPos.y = LogoDefaultPosition.y + (LogoDefaultSize.y - LogoBounds.height) * 0.5f - LogoBounds.top;

		sf::Transform LogoTextTransform;
		LogoTextTransform.translate(LogoTextPos);

		FRenderTarget& LogoRenderText = Logo->GetActorRenderText();
		LogoRenderText = FRenderTarget(LogoText, LogoTextTransform);

		LogoRenderText.bRenderOpacity = bVisible;

		ViewportActors.push_back(Logo);
	}

	sf::Color BtnColor = sf::Color(MenuSettings.DefaultButtonColor.R, MenuSettings.DefaultButtonColor.G,
		MenuSettings.DefaultButtonColor.B, MenuSettings.DefaultButtonColor.A);
	sf::Color TextColor = sf::Color(MenuSettings.TextColor.R, MenuSettings.TextColor.G,
		MenuSettings.TextColor.B, MenuSettings.TextColor.A);

	auto CreateButton = [this, &BDefaultPosition, &BDefaultSize, Skip, &Fnt, &BtnColor, &TextColor, CharSize, bVisible]
	(const std::string& Name, const bool bDoubleSkip = false, const bool bBindDelegate = true) {
		TStrongPtr<APGWidget> Btn = CreateObject<APGWidget>();

		Btn->SetUnhovered();

		sf::RectangleShape* BtnShape = new sf::RectangleShape();
		BtnShape->setSize(BDefaultSize);
		BtnShape->setFillColor(BtnColor);

		sf::Transform BtnTransform;
		BtnTransform.translate(BDefaultPosition);

		FRenderTarget& RTargen = Btn->GetActorRenderTarget();
		RTargen = FRenderTarget(BtnShape, BtnTransform);

		sf::Text* BtnText = new sf::Text();
		BtnText->setString(Name);
		BtnText->setFont(Fnt);
		BtnText->setFillColor(TextColor);
		BtnText->setCharacterSize(CharSize);

		sf::FloatRect TextBounds = BtnText->getLocalBounds();

		sf::Vector2f TextPos;
		TextPos.x = BDefaultPosition.x + (BDefaultSize.x - TextBounds.width) * 0.5f - TextBounds.left;
		TextPos.y = BDefaultPosition.y + (BDefaultSize.y - TextBounds.height) * 0.5f - TextBounds.top;

		sf::Transform TextTransform;
		TextTransform.translate(TextPos);

		FRenderTarget& RenderText = Btn->GetActorRenderText();
		RenderText = FRenderTarget(BtnText, TextTransform);

		Btn->SetUnhovered();
		ViewportActors.push_back(Btn);

		if (bBindDelegate)
		{
			Btn->OnMouseReleased.Bind(this, &APGViewport::HandleButtonReleased);
		}

		BDefaultPosition.y += Skip + (bDoubleSkip ? (Skip + BDefaultSize.y) : 0.f);

		RTargen.bRenderOpacity = bVisible;
		RenderText.bRenderOpacity = bVisible;
		};

	auto CreateDifficultyButton = [this, &BDefaultPosition, &BDfcltSize, Skip, &Fnt, &TextColor, CharSize, bVisible](const std::string& Name)->TStrongPtr<APGWidget> {
		TStrongPtr<APGWidget> Btn = CreateObject<APGWidget>();

		sf::RectangleShape* BtnShape = new sf::RectangleShape();
		BtnShape->setSize(BDfcltSize);

		sf::Transform BtnTransform;
		BtnTransform.translate(BDefaultPosition);

		FRenderTarget& RTargen = Btn->GetActorRenderTarget();
		RTargen = FRenderTarget(BtnShape, BtnTransform);

		sf::Text* BtnText = new sf::Text();
		BtnText->setString(Name);
		BtnText->setFont(Fnt);
		BtnText->setFillColor(TextColor);
		BtnText->setCharacterSize(CharSize);

		sf::FloatRect TextBounds = BtnText->getLocalBounds();

		sf::Vector2f TextPos;
		TextPos.x = BDefaultPosition.x + (BDfcltSize.x - TextBounds.width) * 0.5f - TextBounds.left;
		TextPos.y = BDefaultPosition.y + (BDfcltSize.y - TextBounds.height) * 0.5f - TextBounds.top;

		sf::Transform TextTransform;
		TextTransform.translate(TextPos);

		FRenderTarget& RenderText = Btn->GetActorRenderText();
		RenderText = FRenderTarget(BtnText, TextTransform);

		Btn->SetUnhovered();
		ViewportActors.push_back(Btn);

		Btn->OnMouseReleased.Bind(this, &APGViewport::HandleButtonReleased);

		BDefaultPosition.x += BDfcltSize.x + Skip;

		RTargen.bRenderOpacity = bVisible;
		RenderText.bRenderOpacity = bVisible;

		return Btn;
		};

	CreateButton(MenuSettings.ButtonContinue, true);
	CreateButton(MenuSettings.ButtonSettings);
	CreateButton(MenuSettings.ButtonQuit);

	BDefaultPosition = sf::Vector2f(MenuSettings.MenuButtonsDefaultPositionX * RWSize.x,
		MenuSettings.MenuButtonsDefaultPositionY * RWSize.y + MenuSettings.MenuButtonsDefaultPositionY + Skip);

	TStrongPtr<APGWidget> DBtnE = CreateDifficultyButton(MenuSettings.ButtonEasy);
	TStrongPtr<APGWidget> DBtnN = CreateDifficultyButton(MenuSettings.ButtonNormal);
	TStrongPtr<APGWidget> DBtnH = CreateDifficultyButton(MenuSettings.ButtonHard);

	/** Hardcode different colors */
	if (DBtnE && DBtnN && DBtnH)
	{
		FRenderTarget& RTE = DBtnE->GetActorRenderTarget();
		FRenderTarget& RTN = DBtnN->GetActorRenderTarget();
		FRenderTarget& RTH = DBtnH->GetActorRenderTarget();

		sf::Drawable* DE = RTE.Drawable;
		sf::Drawable* DN = RTN.Drawable;
		sf::Drawable* DH = RTH.Drawable;

		if (DE && DN && DH)
		{
			sf::RectangleShape* RSE = dynamic_cast<sf::RectangleShape*>(DE);
			sf::RectangleShape* RSN = dynamic_cast<sf::RectangleShape*>(DN);
			sf::RectangleShape* RSH = dynamic_cast<sf::RectangleShape*>(DH);

			if (RSE)
			{
				RSE->setFillColor(sf::Color(MenuSettings.EasyButtonColor.R, MenuSettings.EasyButtonColor.G,
					MenuSettings.EasyButtonColor.B, MenuSettings.EasyButtonColor.A));
			}
			if (RSN)
			{
				RSN->setFillColor(sf::Color(MenuSettings.NormalButtonColor.R, MenuSettings.NormalButtonColor.G,
					MenuSettings.NormalButtonColor.B, MenuSettings.NormalButtonColor.A));
			}
			if (RSH)
			{
				RSH->setFillColor(sf::Color(MenuSettings.HardButtonColor.R, MenuSettings.HardButtonColor.G,
					MenuSettings.HardButtonColor.B, MenuSettings.HardButtonColor.A));
			}
		}
	}
	const EGameDifficulty& GameDifficulty = SaveGameManager->GetSelectedGameDifficulty();
	switch (GameDifficulty)
	{
	case EGameDifficulty::Easy:
		DBtnE->SetHovered();
		break;
	case EGameDifficulty::Normal:
		DBtnN->SetHovered();
		break;
	case EGameDifficulty::Hard:
		DBtnH->SetHovered();
		break;
	}
}

void APGViewport::InitGame()
{
	USaveGameManager* SaveGameManager = GameInstance->GetSubsystem<USaveGameManager>();
	if (!SaveGameManager)
	{
		return;
	}

	const FGameSettings& Settings = SaveGameManager->RequestGameSetupSettings();
	const sf::Vector2u RWSize = RenderWindow->getSize();

	TStrongPtr<APGWidget> LeftBG = CreateObject<APGWidget>();
	if (LeftBG)
	{
		LeftBG->bIsFocusable = false;

		float HalfWidth = RWSize.x * Settings.BackgroundSplit;

		sf::RectangleShape* Shape = new sf::RectangleShape();
		Shape->setSize(sf::Vector2f(HalfWidth, (float)RWSize.y));
		Shape->setFillColor(ToSFColor(Settings.LeftBGColor));

		sf::Transform LBGT;
		LBGT.translate(0.f, 0.f);

		LeftBG->GetActorRenderTarget() = FRenderTarget(Shape, LBGT);

		sf::Text* Text = new sf::Text();
		Text->setFont(SaveGameManager->LoadDefaultFont());
		Text->setString("0");
		Text->setCharacterSize(Settings.ScoreTextSize);
		Text->setFillColor(ToSFColor(Settings.ScoreTextColor));

		sf::FloatRect Bounds = Text->getLocalBounds();

		sf::Vector2f Pos;
		Pos.x = (HalfWidth - Bounds.width) * 0.5f - Bounds.left;
		Pos.y = 10.f;

		sf::Transform TextT;
		TextT.translate(Pos);

		PlayerScore = Text;
		LeftBG->GetActorRenderText() = FRenderTarget(Text, TextT);
	}
	
	TStrongPtr<APGWidget> RightBG = CreateObject<APGWidget>();
	if (RightBG)
	{
		float HalfWidth = RWSize.x * Settings.BackgroundSplit;

		sf::RectangleShape* Shape = new sf::RectangleShape();
		Shape->setSize(sf::Vector2f(HalfWidth, (float)RWSize.y));
		Shape->setFillColor(ToSFColor(Settings.RightBGColor));

		sf::Transform RBGT;
		RBGT.translate(HalfWidth, 0.f);

		RightBG->GetActorRenderTarget() = FRenderTarget(Shape, RBGT);

		sf::Text* Text = new sf::Text();
		Text->setFont(SaveGameManager->LoadDefaultFont());
		Text->setString("0");
		Text->setCharacterSize(Settings.ScoreTextSize);
		Text->setFillColor(ToSFColor(Settings.ScoreTextColor));

		sf::FloatRect Bounds = Text->getLocalBounds();

		sf::Vector2f Pos;
		Pos.x = HalfWidth + (HalfWidth - Bounds.width) * 0.5f - Bounds.left;
		Pos.y = 10.f;

		sf::Transform TextT;
		TextT.translate(Pos);

		EnemyScore = Text;
		RightBG->GetActorRenderText() = FRenderTarget(Text, TextT);
	}
	
	TStrongPtr<APGControlledCharacter> Player = CreateObject<APGControlledCharacter>();
	if (Player)
	{
		sf::RectangleShape* Shape = new sf::RectangleShape();

		sf::Vector2f Size(Settings.CharacterWidth * RWSize.x, Settings.CharacterHeight * RWSize.y);

		Shape->setSize(Size);
		Shape->setFillColor(ToSFColor(Settings.PlayerColor));

		sf::Vector2f Pos(Settings.LocalPlayerDefaultPositionX * RWSize.x, Settings.LocalPlayerDefaultPositionY * RWSize.y);

		sf::Transform PT;
		PT.translate(Pos);

		Player->GetActorRenderTarget() = FRenderTarget(Shape, PT);

		Player->SetMovementBounds(sf::FloatRect(0.f, 0.f, RWSize.x * Settings.BackgroundSplit / 2.f, (float)RWSize.y));
		Player->SetMovementSpeed(Settings.PlayerSpeed);
	}

	TStrongPtr<APGEnemyCharacter> Enemy = CreateObject<APGEnemyCharacter>();
	if (Enemy)
	{
		Enemy->BindWorldDelegates(this);

		sf::RectangleShape* Shape = new sf::RectangleShape();

		sf::Vector2f Size(Settings.CharacterWidth * RWSize.x,Settings.CharacterHeight * RWSize.y);

		Shape->setSize(Size);
		Shape->setFillColor(ToSFColor(Settings.EnemyColor));

		sf::Vector2f Pos(Settings.EnemyDefaultPositionX * RWSize.x, Settings.EnemyDefaultPositionY * RWSize.y);

		sf::Transform ET;
		ET.translate(Pos);

		Enemy->GetActorRenderTarget() = FRenderTarget(Shape, ET);
		Enemy->SetMovementBounds(sf::FloatRect(RWSize.x * Settings.BackgroundSplit, 0.f, RWSize.x * Settings.BackgroundSplit, (float)RWSize.y));
	}

	TStrongPtr<APGBounceBall> BounceBall = CreateObject<APGBounceBall>();
	if (BounceBall)
	{
		BounceBall->InitializeBallProperties(this, Settings, RWSize);
		BounceBall->AddTrackedObstacle(Player.Raw());
		BounceBall->AddTrackedObstacle(Enemy.Raw());
		BounceBall->BindWorldDelegates(this);

		float Radius = Settings.BallDefaultRadius * RWSize.x / 100.f * RWSize.y / 100.f;

		sf::CircleShape* Shape = new sf::CircleShape();
		Shape->setRadius(Radius);
		Shape->setFillColor(ToSFColor(Settings.BallColor));

		Shape->setOrigin(Radius, Radius);

		sf::Vector2f Pos(Settings.BallDefaultPositionX* RWSize.x, Settings.BallDefaultPositionY* RWSize.y);

		sf::Transform BBT;
		BBT.translate(Pos);

		BounceBall->GetActorRenderTarget() = FRenderTarget(Shape, BBT);
	}

	TStrongPtr<APGWidget> Splitter = CreateObject<APGWidget>();
	if (Splitter)
	{
		Splitter->bIsFocusable = false;

		float Width = Settings.MiddleLineWidth * RWSize.x;

		sf::RectangleShape* Shape = new sf::RectangleShape();
		Shape->setSize(sf::Vector2f(Width, (float)RWSize.y ));
		Shape->setFillColor(ToSFColor(Settings.MiddleLineColor));

		sf::Transform ST;

		float X = RWSize.x * Settings.BackgroundSplit - Width * 0.5f;

		ST.translate(X, 0.f);

		Splitter->GetActorRenderTarget() = FRenderTarget(Shape, ST);
	}

	Enemy->AddTarget(BounceBall.Raw());

	BounceBall->OnBounce.Bind(this, &APGViewport::HandleBounce);
	BounceBall->OnPlayerScored.Bind(this, &APGViewport::HandlePlayerScored);
	BounceBall->OnEnemyScored.Bind(this, &APGViewport::HandleEnemyScored);

	ViewportActors = { LeftBG, RightBG, Player, Enemy, Splitter, BounceBall };

	ResetGameDifficulty();
}
