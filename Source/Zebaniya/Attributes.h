// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Components/ActorComponent.h"
#include "Attributes.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZEBANIYA_API UAttributes : public UActorComponent
{
	GENERATED_BODY()
	private : 
		UPROPERTY(EditAnywhere)
		int64 maxHealth;
		UPROPERTY(EditAnywhere)
		int64 currentHealth;
		UPROPERTY(EditAnywhere)
		float regenTime = 4.0;
		UPROPERTY(EditAnywhere)
		int64 healthRegen = 2;
		float TimeWhenDamageTaken = 0.0;

		void Regen();
public:	
	// Sets default values for this component's properties
	UAttributes();
	void TakeDamage(int64);
	UFUNCTION()
	int64 GetCurrentHealth();
	void SetCurrentHealth(int64);
	UFUNCTION()
	int64 GetMaxHealth();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
