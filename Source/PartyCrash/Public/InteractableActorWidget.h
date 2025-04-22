// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "InteractableActorWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class PARTYCRASH_API UInteractableActorWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(BindWidget))
	UTextBlock* MyTextBox;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowTextBox();

	UFUNCTION(BlueprintImplementableEvent)
	void HideTextBox();

	UFUNCTION()
	void SetTextBlockText(FString TextToSet);

	UFUNCTION()
	FString GetTextBlockText();
};
