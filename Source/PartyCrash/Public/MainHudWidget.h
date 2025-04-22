// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "MainHudWidget.generated.h"

/**
 * 
 */
UCLASS()
class PARTYCRASH_API UMainHudWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* HungerBar;

	UFUNCTION()
	void SetPercent(float PercentToSet);

	UFUNCTION()
	float GetPercent();
};
