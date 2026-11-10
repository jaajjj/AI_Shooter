// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP3ShootCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"


//////////////////////////////////////////////////////////////////////////
// ATP3ShootCharacter

ATP3ShootCharacter::ATP3ShootCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create SK_Gun
	SK_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	SK_Gun->SetupAttachment(GetMesh(), TEXT("GripPoint"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void ATP3ShootCharacter::BeginPlay()
{
	Super::BeginPlay();
	Health = MaxHealth;
}


//dégat
float ATP3ShootCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
									 AController* EventInstigator, AActor* DamageCauser)
{
	if (IsAlly(DamageCauser)) return 0.0f;
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bIsDead || ActualDamage <= 0.0f)
	{
		return 0.0f;
	}

	// Déduction des points de vie
	Health = FMath::Clamp(Health - ActualDamage, 0.0f, MaxHealth);

	// Déclenchement de la délégation pour l'IA, le HUD, etc.
	OnHealthChanged.Broadcast(Health, ActualDamage, EventInstigator);

	// Gestion de la mort
	if (Health <= 0.0f)
	{
		bIsDead = true;
		
		GetCharacterMovement()->DisableMovement();
		GetCharacterMovement()->StopMovementImmediately();

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//ragdoll
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);

		DetachFromControllerPendingDestroy();
	}

	return ActualDamage;
}

void ATP3ShootCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ATP3ShootCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ATP3ShootCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ATP3ShootCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ATP3ShootCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ATP3ShootCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ATP3ShootCharacter::TouchStopped);

	// Aiming 
	PlayerInputComponent->BindAction("Aiming", IE_Pressed, this, &ATP3ShootCharacter::Aim);
	PlayerInputComponent->BindAction("Aiming", IE_Released, this, &ATP3ShootCharacter::StopAiming);

	// Fire
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ATP3ShootCharacter::Fire);

	// Boost Speed
	PlayerInputComponent->BindAction("BoostSpeed", IE_Pressed, this, &ATP3ShootCharacter::BoostSpeed);
	PlayerInputComponent->BindAction("BoostSpeed", IE_Released, this, &ATP3ShootCharacter::RemoveSpeedBoost);
	
	// ChangeSize
	PlayerInputComponent->BindAction("ChangeSize", IE_Pressed, this, &ATP3ShootCharacter::ChangeSize);
}

void ATP3ShootCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ATP3ShootCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ATP3ShootCharacter::Aim()
{
	IsAiming = true;
}

void ATP3ShootCharacter::StopAiming()
{
	IsAiming = false;
}

void ATP3ShootCharacter::Fire()
{
	if (bIsDead) return;
	//point de départ : caméra en visée, canon du fusil sinon
	FVector Start = IsAiming ? FollowCamera->GetComponentLocation() 
							 : (SK_Gun ? SK_Gun->GetSocketLocation(TEXT("MuzzleFlash")) : GetActorLocation());

	FVector ForwardVector = FollowCamera->GetForwardVector();
	FVector LineTraceEnd = Start + (ForwardVector * 10000.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); //ne pas se tirer dessus

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, LineTraceEnd, ECC_Visibility, Params);

	if (bHit)
	{
		// Debug visuel : ligne verte vers l'impact + sphère rouge au point de collision
		DrawDebugLine(GetWorld(), Start, Hit.ImpactPoint, FColor::Green, false, 2.0f, 0, 1.5f);
		DrawDebugPoint(GetWorld(), Hit.ImpactPoint, 10.0f, FColor::Red, false, 2.0f);

		AActor* HitActor = Hit.GetActor();

		//dmg
		if (HitActor && !IsAlly(HitActor))
		{
			UGameplayStatics::ApplyDamage(
				HitActor,
				2.0f,
				GetController(), // Transmet l'InstigatorController
				this,
				UDamageType::StaticClass()
			);
		}

		//particule
		FireParticle(Start, Hit.ImpactPoint);
	}
	else
	{
		//Debug point
		DrawDebugLine(GetWorld(), Start, LineTraceEnd, FColor::Red, false, 2.0f, 0, 1.5f);

		// Particule vers le point max
		FireParticle(Start, LineTraceEnd);
	}
}

bool ATP3ShootCharacter::IsAlly(const AActor* Other) const
{
	const ATP3ShootCharacter* OtherCharacter = Cast<ATP3ShootCharacter>(Other);
	return OtherCharacter && OtherCharacter->IDTeam == IDTeam;
}

void ATP3ShootCharacter::BoostSpeed()
{
	// Set Max walking speed to 800
	GetCharacterMovement()->MaxWalkSpeed = 800.f;

	GetWorld()->GetTimerManager().SetTimer(BoostSpeedTimer, [this]()
		{
			// Set Max walking speed to 500
			GetCharacterMovement()->MaxWalkSpeed = 500.f;
			
			// Clear existing timer boost speed
			GetWorldTimerManager().ClearTimer(BoostSpeedTimer);

		}, 4, false);
}

void ATP3ShootCharacter::RemoveSpeedBoost()
{
	// Set Max walking speed to 500
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
}

void ATP3ShootCharacter::ChangeSize()
{
	bIsGrand = !bIsGrand;

	const float Scale = bIsGrand ? ScaleGrand : ScalePetit;
	GetCapsuleComponent()->SetWorldScale3D(FVector(Scale));

	// Smaller is slower
	GetCharacterMovement()->MaxWalkSpeed = bIsGrand ? 500.f : 300.f;
}


void ATP3ShootCharacter::FireParticle(FVector Start, FVector Impact)
{
	if (!ParticleStart || !ParticleImpact) return;

	FTransform ParticleT;

	ParticleT.SetLocation(Start);

	ParticleT.SetScale3D(FVector(0.25, 0.25, 0.25));

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleStart, ParticleT, true);

	// Spawn particle at impact point
	ParticleT.SetLocation(Impact);

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleImpact, ParticleT, true);

}

void ATP3ShootCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ATP3ShootCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ATP3ShootCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATP3ShootCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
