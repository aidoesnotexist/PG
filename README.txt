UObject	: Base class which all game instances are inherited from   	../Public/Core
├── UInstance	: Base class for non visual/non rendered instances 		../Public/Instance
│   ├── UGameInstance : Base class, which is the top level brain, runs from main
│   ├── UPGGameInstanceSubsystem : Global classes that are used for specific roles ../Public/Instance/Subsystems
│   │   ├── UInputHandler
│   │   ├── USaveGameManager
│   │   └── USoundManager
│   └── AActor	: Base class for everything that can be rendered and placed on scene ../Public/Actor
│       ├── APlayerController : Redistributes hardware inputs
│       └── APawn : Object that can move
│           ├── AWidget : UI
│           └── ACharacter : Movable character with custom logic
│               ├── AEnemyCharacter
│               ├── ABounceBall
│               └── AControlledCharacter
Utilities:				   				../Public/Core/Components
├── TStrongPtr : Custom implementation of shared pointer
├── TDelegate : Functions' callback implementation
└── TTicker : Frame delta clock, used by actors
|
BasicTypes : enums, structs				   		../Public