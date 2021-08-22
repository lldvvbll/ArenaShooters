// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ASDamageComponent.h"
#include "Character/ASCharacter.h"
#include "Character/ASStatusComponent.h"
#include "Character/ASInventoryComponent.h"
#include "ItemActor/ASBullet.h"
#include "ASAssetManager.h"
#include "DataAssets/CharacterDataAssets/ASDamageDataAsset.h"

UASDamageComponent::UASDamageComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);

}

void UASDamageComponent::InitializeComponent()
{
	Super::InitializeComponent();

	ASChar = GetOwner<AASCharacter>();
	ASChar->OnTakeAnyDamage.AddDynamic(this, &UASDamageComponent::OnTakeDamage);

	ASStatus = ASChar->GetStatusComponent();
	ASInventory = ASChar->GetInventoryComponent();
	
	DamageDataAsset = UASAssetManager::Get().GetDataAsset<UASDamageDataAsset>(DamageAssetId);
}

void UASDamageComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void UASDamageComponent::TakeBulletDamage(AASBullet* InBullet, const FHitResult& InHit)
{
	if (!IsValid(InBullet))
	{
		AS_LOG_S(Error);
		return;
	}

	float Damage = InBullet->GetDamage();
	float TakenDamage = Damage;



	if (DamageDataAsset != nullptr)
	{
		Damage *= DamageDataAsset->GetDamageRateByBone(ASChar->GetMesh(), InHit.BoneName);
	}

	FPointDamageEvent DamageEvent(Damage, InHit, InBullet->GetActorForwardVector(), nullptr);
	ASChar->TakeDamage(Damage, DamageEvent, ASChar->GetController(), InBullet);
}

void UASDamageComponent::OnTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	bool bBeforeDead = ASChar->IsDead();

	ASStatus->ModifyCurrentHealth(-Damage);

	if (!bBeforeDead && ASChar->IsDead())
	{
		AS_LOG(Warning, TEXT("Char is Dead"));
	}
}
