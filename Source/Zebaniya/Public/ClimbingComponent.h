// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "ClimbInterface.h"
#include "ClimbingComponent.generated.h"

class ACharacter;
class UClimbAnimInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZEBANIYA_API UClimbingComponent : public USphereComponent, public IClimbInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClimbingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnOverlapBeginLeft(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapBeginRight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnOverlapEndLeft(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UFUNCTION()
	void OnOverlapEndRight(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void LetGoOfLedge();
	void EnableLedgeGrabing();
	void ClimbUp();
	void FinishClimbUP();
	void ClimbLeft();
	void ClimbRight();

private:
	bool ForwardSphereTrace();
	bool DownwardSphereTrace();
	void AttemptToGrabLedge();
	void FinishClimbInteractions();
	void SetUpSideClimbingMovementDetection(USphereComponent*& Sphere, FVector LocalOffset, const FName Name);
	class UInputComponent* SetUpClimbingControllerForPlayer();
	
	bool bCanTrace;
	bool bIsClimbingLedge;
	bool bIsHanging;
	bool bCanMoveLeft;
	bool bCanMoveRight;
	FVector DownwardTraceOffset;
	FHitResult ForwardTraceResult;
	FHitResult DownwardTraceResult;
	AActor* LetGoOfActor;
	ACharacter* Owner;
	UClimbAnimInstance* Animation;
	UInputComponent* ClimbingInputController;
	USphereComponent* LeftSphere;
	USphereComponent* RightSphere;

	UPROPERTY(EditAnywhere)
	float ForwardTraceDistance = 40;
	UPROPERTY(EditAnywhere)
	float SphereTraceRadius = 30.0f;
	UPROPERTY(EditAnywhere)
	float HeightTrace = 100.0f;
	UPROPERTY(EditAnywhere)
	float HeightOffset = 100.0f;
	UPROPERTY(EditAnywhere)
	float SideSphereRadius = 20.0f;
	
};
