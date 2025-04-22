// Fill out your copyright notice in the Description page of Project Settings.


#include "MainHudWidget.h"

void UMainHudWidget::SetPercent(float PercentToSet)
{
	
	if (HungerBar->IsValidLowLevelFast()) HungerBar->SetPercent(PercentToSet);
}

float UMainHudWidget::GetPercent()
{
	return HungerBar->GetPercent();
}
