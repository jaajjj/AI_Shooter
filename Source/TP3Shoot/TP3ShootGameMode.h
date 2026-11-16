// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TP3ShootGameMode.generated.h"

class AController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameOverSignature, int32, WinningTeamID);

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
	
	void IncRedKills();
	void IncBlueKills();
	
	UPROPERTY(BlueprintAssignable, Category = "Game|Events")
	FOnGameOverSignature OnGameOver;

	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules", meta = (ClampMin = "1"))
	int32 KillsToWin = 10;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "State")
	bool bIsGameEnded = false;
		
private:
	void Respawn(AController* Controller, TSubclassOf<APawn> PawnClass, FTransform SpawnTransform, int32 TeamID);
	void CheckWinCondition();
	void TriggerGameOver(int32 WinningTeamID);
	
	int32 redKills= 0;
	int32 blueKills= 0;
};



