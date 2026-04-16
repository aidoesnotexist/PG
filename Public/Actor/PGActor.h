#pragma once

#include "Core/Object.h"
#include "BasicTypes.h"

/** Base class for everything that can be rendered, tick and have its independent logic 
	inside world(viewport) 
*/
class APGActor : public UPGObject
{
public:
	APGActor(const APGActor&) = delete;
	APGActor& operator=(const APGActor&) = delete;

	virtual ~APGActor() = default;

	virtual void BeginPlay() {}

	virtual void Tick(const float Delta) {}

	virtual FRenderTarget& GetActorRenderTarget() { return RenderTarget; }
	virtual FRenderTarget& GetActorRenderText() { return RenderText; }

protected:
	friend TStrongPtr<APGActor>;
	APGActor() = default;

	FRenderTarget RenderTarget;
	FRenderTarget RenderText;
};