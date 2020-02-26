// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ClimbInterface.h"
#include "ClimbAnimInstance.generated.h"

/**
 * 
 */
UCLASS(transient, Blueprintable, hideCategories = AnimInstance, BlueprintType)
class ZEBANIYA_API UClimbAnimInstance : public UAnimInstance, public IClimbInterface
{
	GENERATED_BODY()
	
	public:
	UFUNCTION(Category = Climb)
	void SetCanGrabLedge(bool bCanGrab);
	UFUNCTION(Category = Climb)
	bool GetCanGrabLedge() const;

	void GrabLedge(bool bCanLedgeGrap);

	protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Climb)
	bool bCanGrabLedge;
};
