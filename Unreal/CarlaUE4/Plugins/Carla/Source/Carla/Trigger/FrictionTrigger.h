// Copyright (c) 2019 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB). This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Carla/Game/CarlaEpisode.h"
#include "BaseTrigger.h"

#include "FrictionTrigger.generated.h"

UCLASS()
class CARLA_API AFrictionTrigger : public ABaseTrigger
{
  GENERATED_BODY()

private:

  void UpdateWheelsFriction(AActor *OtherActor, TArray<float>& NewFriction);

public:

  virtual void OnTriggerBeginOverlap(
      UPrimitiveComponent *OverlappedComp,
      AActor *OtherActor,
      UPrimitiveComponent *OtherComp,
      int32 OtherBodyIndex,
      bool bFromSweep,
      const FHitResult &SweepResult) override;

  virtual void OnTriggerEndOverlap(
      UPrimitiveComponent *OverlappedComp,
      AActor *OtherActor,
      UPrimitiveComponent *OtherComp,
      int32 OtherBodyIndex) override;

  void SetFriction(float NewFriction)
  {
    Friction = NewFriction;
  }

  virtual void SetAttributes(const FActorDescription& Description) override;

protected:

  // Save old frictions
  TArray<float> OldFrictionValues;

public:

  UPROPERTY(EditAnywhere)
  float Friction = 0.0f;

};
