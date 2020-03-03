// Fill out your copyright notice in the Description page of Project Settings.


#include "Attributes.h"

// Sets default values for this component's properties
UAttributes::UAttributes()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}
//Deals damage to player
void UAttributes::TakeDamage(int64 damage)
{
	TimeWhenDamageTaken = GetWorld()->GetTimeSeconds();
	SetCurrentHealth(currentHealth - damage);
	if (currentHealth < 0)
	{
		currentHealth = 0;
		//TODO call die/gameover method
		UE_LOG(LogTemp, Warning, TEXT("Game Over")); //End the game
	}
}

int64 UAttributes::GetMaxHealth()
{
	return maxHealth;
}

int64 UAttributes::GetCurrentHealth()
{
	return currentHealth;
}

void UAttributes::SetCurrentHealth(int64 newHealthValue)
{
	if (newHealthValue > maxHealth)
	{
		newHealthValue = maxHealth;
	}
	else if (newHealthValue < 0)
	{
		return;
	}

	//Enable and Disable Timer Tick for optimization
	if (newHealthValue < maxHealth)
	{
		PrimaryComponentTick.bCanEverTick = true;
	}
	else if (newHealthValue >= maxHealth)
	{
		PrimaryComponentTick.bCanEverTick = false;
	}
	currentHealth = newHealthValue;
}

void UAttributes::IncreaseMaxHealth(int64 addedHealth)
{
	maxHealth += addedHealth;
	SetCurrentHealth(maxHealth);
}

// Called when the game starts
void UAttributes::BeginPlay()
{
	Super::BeginPlay();
	if (currentHealth > maxHealth) {
		currentHealth = maxHealth;
	}
	// ...
	
}

void UAttributes::Regen()
{
	if (currentHealth < maxHealth)
	{
		if (GetWorld()->GetTimeSeconds() >= (TimeWhenDamageTaken + regenTime))
		{
			SetCurrentHealth(currentHealth + healthRegen);
			TimeWhenDamageTaken = GetWorld()->GetTimeSeconds();
		}
	}
}

// Called every frame
void UAttributes::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	Regen();
	// ...
}

