// Copyright (c) 2021 Computer Vision Center (CVC) at the Universitat Autonoma de Barcelona (UAB).
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Engine/LevelStreamingDynamic.h"

#include "Math/DVector.h"

#include "LargeMapManager.generated.h"


// World->RequestNewWorldOrigin - Force rebase ?
// WorldComposition->UpdateStreamingState - Force stream levels ?

USTRUCT()
struct FCarlaMapTile
{
  GENERATED_BODY()

#if WITH_EDITOR
  UPROPERTY(VisibleAnywhere, Category = "Carla Map Tile")
  FString Name; // Tile_{TileID_X}_{TileID_Y}
#endif // WITH_EDITOR

  // Absolute location, does not depend on rebasing
  UPROPERTY(VisibleAnywhere, Category = "Carla Map Tile")
  FVector Location{0.0f};

  UPROPERTY(VisibleAnywhere, Category = "Carla Map Tile")
  ULevelStreamingDynamic* StreamingLevel = nullptr;

  // Assets in tile waiting to be spawned
  // TODO: categorize assets type and prioritize roads
  UPROPERTY(VisibleAnywhere, Category = "Carla Map Tile")
  TArray<FAssetData> PendingAssetsInTile;

  bool TilesSpawned = false;
};

USTRUCT()
struct FActorToConsider
{
  GENERATED_BODY()

  FActorToConsider() {}

  FActorToConsider(AActor* InActor) : Actor(InActor) {}

  AActor* Actor = nullptr;

  // Absolute location = Current Origin + Relative Tile Location
  FDVector Location;
};

UCLASS()
class CARLA_API ALargeMapManager : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  ALargeMapManager();

  ~ALargeMapManager();

protected:
  // Called when the game starts or when spawned
  virtual void BeginPlay() override;

  void PreWorldOriginOffset(UWorld* InWorld, FIntVector InSrcOrigin, FIntVector InDstOrigin);
  void PostWorldOriginOffset(UWorld* InWorld, FIntVector InSrcOrigin, FIntVector InDstOrigin);

  void OnLevelAddedToWorld(ULevel* InLevel, UWorld* InWorld);
  void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld);

public:
  // Called every frame
  virtual void Tick(float DeltaTime) override;

  UFUNCTION(BlueprintCallable, Category = "Large Map Manager")
  void GenerateMap(FString InAssetsPath);

  UFUNCTION(BlueprintCallable, Category = "Large Map Manager")
  void AddNewClientToConsider(AActor* InActor);

  UFUNCTION(BlueprintCallable, Category = "Large Map Manager")
  FIntVector GetNumTilesInXY() const;

  UFUNCTION(BlueprintCallable, Category = "Large Map Manager")
  bool IsLevelOfTileLoaded(FIntVector InTileID) const;

protected:

  FIntVector GetTileVectorID(FVector TileLocation) const;

  FIntVector GetTileVectorID(FDVector TileLocation) const;

  FIntVector GetTileVectorID(uint64 TileID) const;

  /// From a given location it retrieves the TileID that covers that area
  uint64 GetTileID(FVector TileLocation) const;

  uint64 GetTileID(FIntVector TileVectorID) const;

  FCarlaMapTile& GetCarlaMapTile(FVector Location);

  FCarlaMapTile& GetCarlaMapTile(ULevel* InLevel);

  FCarlaMapTile* GetCarlaMapTile(FIntVector TileVectorID);

  ULevelStreamingDynamic* AddNewTile(FString TileName, FVector TileLocation);

  void UpdateActorsToConsiderPosition();

  void UpdateTilesState();

  void UpdateTilesState(const FActorToConsider& ActorToConsider);

  void SpawnAssetsInTile(FCarlaMapTile& Tile);

  void OnActorSpawned(AActor *Actor);

  TMap<uint64, FCarlaMapTile> MapTiles;

  TArray<FActorToConsider> ActorsToConsider;

  FDelegateHandle ActorSpawnedDelegate;

  // Current Origin after rebase
  FIntVector CurrentOriginInt{0};
  FDVector CurrentOriginD;

  UPROPERTY(EditAnywhere, Category = "Large Map Manager")
  float LayerStreamingDistance = 13.0f * 1000.0f * 100.0f;

  UPROPERTY(EditAnywhere, Category = "Large Map Manager")
  float RebaseOriginDistance = 2.0f * 1000.0f * 100.0f;

  UPROPERTY(EditAnywhere, Category = "Large Map Manager")
  float TileSide = 9.0f * 1000.0f * 100.0f; // 2km

  UPROPERTY(EditAnywhere, Category = "Large Map Manager")
  bool ShouldTilesBlockOnLoad = false;

#if WITH_EDITOR

  UFUNCTION(BlueprintCallable, CallInEditor, Category = "Large Map Manager")
  void GenerateMap_Editor()
  {
    if(!AssetsPath.IsEmpty()) GenerateMap(AssetsPath);
  }

  FString GenerateTileName(uint64 TileID);

  void DumpTilesTable() const;

  void PrintMapInfo();

  UPROPERTY(EditAnywhere, Category = "Large Map Manager")
  FString AssetsPath = "";

  FColor PositonMsgColor = FColor::Purple;

  UPROPERTY(EditAnywhere, Category = "Large Map Manager")
  bool bPrintMapInfo = true;

  UPROPERTY(EditAnywhere, Category = "Large Map Manager")
  bool bPrintErrors = false;

#endif // WITH_EDITOR

};
