// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TP3ShootGameMode.generated.h"

class AController;

UCLASS(minimalapi)
class ATP3ShootGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATP3ShootGameMode();
	void RequestRespawn(AController* Controller, TSubclassOf<APawn> PawnClass, const FTransform& SpawnTransform, int32 TeamID);
	
	UFUNCTION(BlueprintPure, Category = Stats)
	int32 GetRedKills() const { return redKills; }

	UFUNCTION(BlueprintPure, Category = Stats)
	int32 GetBlueKills() const { return blueKills; }
	
	void IncRedKills() { redKills++; }
	void IncBlueKills() { blueKills++; }

private:
	void Respawn(AController* Controller, TSubclassOf<APawn> PawnClass, FTransform SpawnTransform, int32 TeamID);
	int32 redKills= 0;
	int32 blueKills= 0;
};



