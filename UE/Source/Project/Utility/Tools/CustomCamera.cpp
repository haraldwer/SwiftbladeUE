#include "CustomCamera.h"
#include "../Utility.h"
#include "Components/WidgetComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Project/UI/Widgets/CustomWidgetComponent.h"
#include "Project/UI/Widgets/WidgetBase.h"

UCustomCamera::UCustomCamera()
{
	PrimaryComponentTick.bCanEverTick = true;

	myWidgetInteractionComponent = CreateDefaultSubobject<UWidgetInteractionComponent>("WidgetInteraction");
	CHECK_ASSERT(!myWidgetInteractionComponent, "Failed to create widget interaction");
	myWidgetInteractionComponent->SetupAttachment(this);
	myWidgetInteractionComponent->InteractionSource = EWidgetInteractionSource::Mouse;
	myWidgetInteractionComponent->bShowDebug = true;

	for (int i = 0; i < 3; i++)
	{
		auto comp = CreateDefaultSubobject<UCustomWidgetComponent>(*(FString("WidgetComponent") + FString::FromInt(i)));
		CHECK_CONTINUE_LOG(!comp, "Unable to create widget component")
		comp->SetupAttachment(this);
		comp->SetActive(false);
		comp->SetReceiveHardwareInput(true);
		comp->SetTickMode(ETickMode::Automatic);
		comp->SetWindowVisibility(EWindowVisibility::Visible);
		myWidgetComponents.Add(comp);
		myUnusedComponents.Add(comp);
	}
}

void UCustomCamera::BeginPlay()
{
	Super::BeginPlay();

	for (const auto c : myWidgetComponents)
	{
		CHECK_CONTINUE(!c);
		c->SetRelativeTransform(myWidgetTransform);
	}
}

void UCustomCamera::AddWidget(UWidgetBase* aWidget, const int32 aZOrder)
{
	CHECK_RETURN_LOG(!aWidget, "Invalid ptr");
	const auto find = myWidgets.FindByPredicate([&](const FWidgetEntry& anEntry)
	{
		return anEntry.myWidget.Get() == aWidget;
	});
	CHECK_RETURN_LOG(find, "Widget already added to camera");
	const auto comp = GetWidgetComponent();
	CHECK_RETURN_LOG(!comp, "Failed to create widget component");
	myWidgets.Emplace(FWidgetEntry{aWidget, comp});
	comp->SetInitialLayerZOrder(aZOrder);
	comp->SetWidgetClass(aWidget->GetClass());
	comp->SetWidget(aWidget);
	comp->SetRelativeTransform(myWidgetTransform);
}

void UCustomCamera::RemoveWidget(UWidgetBase* aWidget)
{
	CHECK_RETURN_LOG(!aWidget, "Invalid ptr");
	for (int i = 0; i < myWidgets.Num(); i++)
	{
		auto& widget = myWidgets[i];
		CHECK_CONTINUE(widget.myWidget.Get() != aWidget);
		// Found widget
		ReturnWidgetComponent(widget.myComponent.Get());
		widget.myComponent.Reset();
		myWidgets.RemoveAt(i);
		return;
	}
	LOG("Widget does not exist on camera");
}

UCustomWidgetComponent* UCustomCamera::GetWidgetComponent()
{
	CHECK_RETURN_LOG(myUnusedComponents.Num() <= 0, "No unused components", nullptr);
	const auto comp = myUnusedComponents.Last();
	comp->SetActive(true);
	myUnusedComponents.RemoveAt(myUnusedComponents.Num() - 1);
	return comp;
}

void UCustomCamera::ReturnWidgetComponent(UCustomWidgetComponent* aComp)
{
	CHECK_RETURN_LOG(!aComp, "Tried to return null comp");
	CHECK_RETURN_LOG(myUnusedComponents.Contains(aComp), "Component already returned")
	aComp->SetWidgetClass(nullptr);
	aComp->SetWidget(nullptr);
	aComp->SetActive(false);
	aComp->SetWindowVisibility(EWindowVisibility::SelfHitTestInvisible);
	aComp->SetInitialLayerZOrder(0);
	myUnusedComponents.Add(aComp);
}

