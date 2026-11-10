#include "BTTask_SetSpeed.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_SetSpeed::UBTTask_SetSpeed()
{
	NodeName = TEXT("Set Speed");
}

EBTNodeResult::Type UBTTask_SetSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC) return EBTNodeResult::Failed;

	ACharacter* Char = Cast<ACharacter>(AIC->GetPawn());
	if (!Char || !Char->GetCharacterMovement()) return EBTNodeResult::Failed;

	Char->GetCharacterMovement()->MaxWalkSpeed = NewMaxWalkSpeed;
	return EBTNodeResult::Succeeded;
}