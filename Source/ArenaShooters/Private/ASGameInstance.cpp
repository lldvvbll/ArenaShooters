// Fill out your copyright notice in the Description page of Project Settings.


#include "ASGameInstance.h"

float UASGameInstance::GetDamageRateByBone(const USkinnedMeshComponent* MeshComp, const FName& BoneName) const
{
	if (MeshComp != nullptr)
	{
		for (auto& Pair : DamageRateByBoneMap)
		{
			if (BoneName == Pair.Key || MeshComp->BoneIsChildOf(BoneName, Pair.Key))
			{
				return Pair.Value;
			}
		}
	}	

	return 1.0f;
}