// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbAnimInstance.h"
#include "ClimbingComponent.h"

bool UClimbAnimInstance::GetCanGrabLedge() const
{
	return bCanGrabLedge;
}

void UClimbAnimInstance::SetIsClimbingUP(const bool bIsClimbing)
{
	bIsClimbingUp = bIsClimbing;
}

bool UClimbAnimInstance::GetIsClimbingUP() const
{
	return bIsClimbingUp;
}

void UClimbAnimInstance::SetCanGrabLedge(bool bCanGrab)
{
	bCanGrabLedge = bCanGrab;
}

void UClimbAnimInstance::GrabLedge(const bool bCanLedgeGrap)
{
	SetCanGrabLedge(bCanLedgeGrap);
}

void UClimbAnimInstance::ClimbUp_Implementation()
{
	SetIsClimbingUP(true);
}

void UClimbAnimInstance::FinishClimbing()
{
	UClimbingComponent* Climb = Cast<UClimbingComponent>(TryGetPawnOwner()->GetComponentByClass(UClimbingComponent::StaticClass()));
	bIsClimbingUp = false;
	bCanGrabLedge = false;
	Climb->FinishClimbUP();
}

void UClimbAnimInstance::SetIsMovingLeft(const bool bIsLeft)
{
	bIsMovingLeft = bIsLeft;
}

bool UClimbAnimInstance::GetIsMovingLeft() const
{
	return bIsMovingLeft;
}

void UClimbAnimInstance::SetIsMovingRight(const bool bIsRight)
{
	bIsMovingRight = bIsRight;
}

bool UClimbAnimInstance::GetIsMovingRight() const
{
	return bIsMovingRight; 
}

void UClimbAnimInstance::ClimbRight_Implementation()
{
	SetIsMovingRight(true);
	SetIsMovingLeft(false);
}

void UClimbAnimInstance::ClimbLeft_Implementation()
{
	SetIsMovingLeft(true);
	SetIsMovingRight(false);
}

void UClimbAnimInstance::FinishMoving()
{
	SetIsMovingLeft(false);
	SetIsMovingRight(false);
}