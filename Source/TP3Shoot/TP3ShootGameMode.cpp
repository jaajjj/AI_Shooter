#include "TP3ShootGameMode.h"
#include "TP3ShootCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "BrainComponent.h"

ATP3ShootGameMode::ATP3ShootGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ATP3ShootGameMode::RequestRespawn(AController* Controller, TSubclassOf<APawn> PawnClass, const FTransform& SpawnTransform, int32 TeamID)
{
	FTimerHandle UnusedHandle;
	FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &ATP3ShootGameMode::Respawn, Controller, PawnClass, SpawnTransform, TeamID);
	//wait 3s avan de respawn
	GetWorldTimerManager().SetTimer(UnusedHandle, RespawnDelegate, 3.0f, false);
}

void ATP3ShootGameMode::Respawn(AController* Controller, TSubclassOf<APawn> PawnClass, FTransform SpawnTransform, int32 TeamID)
{
	if (!Controller || !PawnClass) return;

	TArray<AActor*> SpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), SpawnPoints);
	//selec un spawn random dispo
	if (SpawnPoints.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
		SpawnTransform = SpawnPoints[RandomIndex]->GetActorTransform();
	}
	
	APawn* NewPawn = GetWorld()->SpawnActorDeferred<APawn>(
		PawnClass,
		SpawnTransform,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
	);

	if (NewPawn)
	{
		//équipe de base
		if (ATP3ShootCharacter* ShootChar = Cast<ATP3ShootCharacter>(NewPawn))
		{
			ShootChar->IDTeam = TeamID;
		}
		
		UGameplayStatics::FinishSpawningActor(NewPawn, SpawnTransform);

		//possess
		Controller->Possess(NewPawn);

		//restart du BT
		if (AAIController* AIC = Cast<AAIController>(Controller))
		{
			if (AIC->BrainComponent)
			{
				AIC->BrainComponent->RestartLogic();
			}
		}
	}
}