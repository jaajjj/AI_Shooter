// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TP3ShootCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChangedSignature, float, CurrentHealth, float, Damage, AController*, InstigatedBy);

UCLASS(config=Game)
class ATP3ShootCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;


public:
	ATP3ShootCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;
	
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return bIsDead; }

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	float MaxHealth = 20.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	float Health = 20.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;

	virtual void BeginPlay() override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
							 class AController* EventInstigator, AActor* DamageCauser) override;

	// Add a gun skeletal mesh component
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* SK_Gun;

	// Particle Start
	UPROPERTY(EditAnywhere, Category = Gameplay)
	class UParticleSystem* ParticleStart;

	// Particle Impact
	UPROPERTY(EditAnywhere, Category = Gameplay)
	class UParticleSystem* ParticleImpact;

	// Fire animation
	UPROPERTY(EditAnywhere, Category = Gameplay)
	class UAnimMontage* FireAnimation;
	
	// Team identifier (characters with the same IDTeam are allies)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Team)
	int32 IDTeam;
	
	// Size
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Size)
	bool bIsGrand = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
	float ScaleGrand = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Size)
	float ScalePetit = 0.5f;
	

	// Timer for Boost Speed
	FTimerHandle BoostSpeedTimer;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	// Aiming function
	void Aim();

	void StopAiming();
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns IDTeam **/
	FORCEINLINE int32 GetIDTeam() const { return IDTeam; }

	// Returns true if Other is a character of the same team
	UFUNCTION(BlueprintPure, Category = Team)
	bool IsAlly(const AActor* Other) const;
	
	void Fire();

	void BoostSpeed();

	void RemoveSpeedBoost();

	void FireParticle(FVector Start, FVector Impact);
	
	UFUNCTION(BlueprintCallable, Category = "Size")
	void ChangeSize();

public:

	// Is Aiming
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aiming")
	bool IsAiming;

	// Is Firing
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Firing")
	bool IsFiring;

};

