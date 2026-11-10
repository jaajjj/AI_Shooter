#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ClearKey.generated.h"

UCLASS()
class TP3SHOOT_API UBTTask_ClearKey : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ClearKey();

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector KeyToClear;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};