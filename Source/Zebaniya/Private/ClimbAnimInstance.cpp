// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbAnimInstance.h"

bool UClimbAnimInstance::GetCanGrabLedge() const
{
	return bCanGrabLedge;
}

void UClimbAnimInstance::SetCanGrabLedge(bool bCanGrab)
{
	bCanGrabLedge = bCanGrab;
}

void UClimbAnimInstance::GrabLedge(bool bCanLedgeGrap)
{
	SetCanGrabLedge(bCanLedgeGrap);
	UE_LOG(LogTemp, Warning, TEXT("yes we can: %s"), (this->bCanGrabLedge ? TEXT("True") : TEXT("False")))
}