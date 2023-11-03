#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "Interface_CollisionDataProviderCore.h"
#include "Interface_Highlighter.generated.h"

UINTERFACE(MinimalAPI)
class UInterface_Highlighter : public UInterface
{
	GENERATED_BODY()
};

class CHOREOPLATFORMER_API IInterface_Highlighter
{
	GENERATED_BODY()

public:
	virtual void ToggleHighlight(bool activated) {}
};

