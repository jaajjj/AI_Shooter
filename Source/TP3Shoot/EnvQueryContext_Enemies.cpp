// Copyright Epic Games, Inc. All Rights Reserved.

#include "EnvQueryContext_Enemies.h"
#include "TP3ShootCharacter.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "GameFramework/Controller.h"
#include "EngineUtils.h"

void UEnvQueryContext_Enemies::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	// The querier is usually the pawn, but can be its controller
	UObject* Owner = QueryInstance.Owner.Get();
	if (const AController* Controller = Cast<AController>(Owner))
	{
		Owner = Controller->GetPawn();
	}

	// A querier without team (e.g. EQSTestingPawn) treats every character as an enemy
	const ATP3ShootCharacter* Querier = Cast<ATP3ShootCharacter>(Owner);

	TArray<AActor*> Enemies;
	for (TActorIterator<ATP3ShootCharacter> It(QueryInstance.World); It; ++It)
	{
		if (!Querier || (*It != Querier && !Querier->IsAlly(*It)))
		{
			Enemies.Add(*It);
		}
	}

	UEnvQueryItemType_Actor::SetContextHelper(ContextData, Enemies);
}
