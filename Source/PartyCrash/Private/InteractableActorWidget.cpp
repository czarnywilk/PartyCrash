// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableActorWidget.h"

void UInteractableActorWidget::SetTextBlockText(FString TextToSet)
{
	MyTextBox->SetText(FText::FromString(TextToSet));
}

FString UInteractableActorWidget::GetTextBlockText()
{
	return MyTextBox->GetText().ToString();
}
