// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_Enemies.generated.h"

/**
 * EQS context: every character whose IDTeam differs from the querier's.
 * Used by the cover query to find points hidden from all enemies.
 */
UCLASS()
class UEnvQueryContext_Enemies : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
