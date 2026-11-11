#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetSize.generated.h"

UCLASS()
class TP3SHOOT_API UBTTask_SetSize : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetSize();
	
	UPROPERTY(EditAnywhere, Category = "Size")
	bool bTargetIsGrand = false;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsGrandKey;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};