// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbingComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ClimbAnimInstance.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UClimbingComponent::UClimbingComponent() : bCanTrace{false}, bIsClimbingLedge {false}
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	this->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	this->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel12, ECollisionResponse::ECR_Block);
	this->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);
	this->OnComponentBeginOverlap.AddDynamic(this, &UClimbingComponent::OnOverlapBegin);
	this->OnComponentEndOverlap.AddDynamic(this, &UClimbingComponent::OnOverlapEnd);
	// ...
}


// Called when the game starts
void UClimbingComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = dynamic_cast<ACharacter*>(GetOwner());
	if (Owner) {
		Animation = Cast<UClimbAnimInstance>(Owner->GetMesh()->GetAnimInstance());
		if (GetWorld()->GetFirstPlayerController() == Owner->Controller) {
			GetWorld()->GetFirstPlayerController()->PushInputComponent(SetUpClimbingControllerForPlayer());
		}
	}
}


// Called every frame
void UClimbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bCanTrace)
	{
		ForwardSphereTrace();
	}
}

void UClimbingComponent::ForwardSphereTrace()
{
	
	if (!Owner) { return; }

	auto Start {Owner->GetActorLocation()};
	auto End{ (Owner->GetActorForwardVector()) * ForwardTraceDistance + Start};
	// forward trace
	FHitResult ResultForwardTrace{};
	bool bHit = UKismetSystemLibrary::SphereTraceSingle(GetOwner(), Start, End, SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery16, true, TArray<AActor*>{}, EDrawDebugTrace::ForOneFrame, ResultForwardTrace, true);
	
	if(!bHit) { return;}
	auto StartVerticle{ ResultForwardTrace.ImpactPoint};
	auto StartVerticleNormal {ResultForwardTrace.ImpactNormal};
	StartVerticle.Z += HeightTrace;
	//downwards trace
	FHitResult ResultDownwardTrace{};
	bool bHit2 = UKismetSystemLibrary::SphereTraceSingle(GetOwner(), StartVerticle, ResultForwardTrace.ImpactPoint, SphereTraceRadius, ETraceTypeQuery::TraceTypeQuery16, true, TArray<AActor*>{}, EDrawDebugTrace::ForOneFrame, ResultDownwardTrace, true);
	
	if(!bHit2) { return; }
	auto LedgeHight {ResultDownwardTrace.ImpactPoint};

	auto HeadLocation {Owner->GetMesh()->GetBoneLocation("head")};
	auto JumpDistance{ ResultDownwardTrace.ImpactPoint.Z - HeadLocation.Z };
	if (!bIsClimbingLedge && (JumpDistance > 0.0f && JumpDistance < 80.0f ))
	{
		if (ClimbingInputController) {
			//ClimbingInputController->Priority = 100;
			ClimbingInputController->bBlockInput = true;
		}
		bIsClimbingLedge = true;
		if(! Animation) { return; }
		Animation->GrabLedge(true);
		Owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
		bIsHanging = true;
		FLatentActionInfo Info {};
		Info.Linkage = 0;
		Info.ExecutionFunction = "StopMovementImmediately";
		Info.CallbackTarget = Owner->GetCharacterMovement();
		auto Rotation { UKismetMathLibrary::FindLookAtRotation(Start, ResultForwardTrace.ImpactPoint) };
		Rotation.Pitch = 0;
		UKismetSystemLibrary::MoveComponentTo(
			Owner->GetCapsuleComponent(),
			FVector {
				ResultForwardTrace.ImpactPoint.X + (ResultForwardTrace.ImpactNormal.X * (Owner->GetCapsuleComponent()->GetScaledCapsuleRadius())),
				ResultForwardTrace.ImpactPoint.Y + (ResultForwardTrace.ImpactNormal.Y * (Owner->GetCapsuleComponent()->GetScaledCapsuleRadius())),
				(ResultDownwardTrace.ImpactPoint.Z - HeightOffset) },
			Rotation,
			false,
			false,
			0.2f,
			false,
			EMoveComponentAction::Move,
			Info);
	}
}

UInputComponent* UClimbingComponent::SetUpClimbingControllerForPlayer()
{
	static const FName InputComponentName(TEXT("PawnInputComponent1"));
	ClimbingInputController = NewObject<UInputComponent>(Owner, InputComponentName);
	//ClimbingInputController->bBlockInput = true;
	//ClimbingInputController->Priority = -1;
	ClimbingInputController->BindAction("CrouchAndLetGo", EInputEvent::IE_Released, this, &UClimbingComponent::LetGoOfLedge);
	return ClimbingInputController;
	
}

void UClimbingComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	bCanTrace = true;
}

void UClimbingComponent::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	bCanTrace = false;
}

void UClimbingComponent::LetGoOfLedge()
{
	if (bIsHanging)
	{
		Owner->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		Animation->GrabLedge(false);
		bIsHanging = false;
		bIsClimbingLedge = false;
		if (ClimbingInputController) {
			//ClimbingInputController->Priority = -100;
			ClimbingInputController->bBlockInput = false;
		}
	}
}

void UClimbingComponent::GrabLedge(bool bCanLedgeGrap)
{
}
