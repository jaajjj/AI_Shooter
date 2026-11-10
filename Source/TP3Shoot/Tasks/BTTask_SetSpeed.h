#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetSpeed.generated.h"

UCLASS()
class TP3SHOOT_API UBTTask_SetSpeed : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetSpeed();

	UPROPERTY(EditAnywhere, Category = "Movement")
	float NewMaxWalkSpeed = 800.0f;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};