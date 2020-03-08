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
	void SetCanGrabLedge(const bool bCanGrab);
	UFUNCTION(Category = Climb)
	bool GetCanGrabLedge() const;
	UFUNCTION(Category = Climb)
	void SetIsClimbingUP(const bool bIsClimbing);
	UFUNCTION(Category = Climb)
	bool GetIsClimbingUP() const;
	UFUNCTION(Category = Climb)
	void SetIsMovingLeft(const bool bIsClimbing);
	UFUNCTION(Category = Climb)
	bool GetIsMovingLeft() const;
	UFUNCTION(Category = Climb)
	void SetIsMovingRight(const bool bIsClimbing);
	UFUNCTION(Category = Climb)
	bool GetIsMovingRight() const;

	void GrabLedge(bool bCanLedgeGrap);
	
	UFUNCTION(Category = "Climb", BlueprintNativeEvent, BlueprintCallable)
	void ClimbUp();
	virtual void ClimbUp_Implementation();
	UFUNCTION(Category = "Climb", BlueprintNativeEvent, BlueprintCallable)
	void ClimbRight();
	virtual void ClimbRight_Implementation();
	UFUNCTION(Category = "Climb", BlueprintNativeEvent, BlueprintCallable)
	void ClimbLeft();
	virtual void ClimbLeft_Implementation();

	UFUNCTION(Category = "Climb", BlueprintCallable)
	void FinishClimbing();
	UFUNCTION(Category = "Climb", BlueprintCallable)
	void FinishMoving();

	protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Climb)
	bool bCanGrabLedge;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Climb)
	bool bIsClimbingUp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Climb)
	bool bIsMovingRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Climb)
	bool bIsMovingLeft;
};
