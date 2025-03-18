#include "EditorGizmos.h"

#include "Object/Actor/Camera.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Object/World/World.h"
#include "Static/FEditorManager.h"
#include "Core/Input/PlayerInput.h"

AEditorGizmos::AEditorGizmos()
{
	bIsGizmo = true;
	
	UCylinderComp* ZArrow = AddComponent<UCylinderComp>();
	ZArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	ZArrow->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
	
	UCylinderComp* XArrow = AddComponent<UCylinderComp>();
	XArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 90.0f, 0.0f), FVector(1, 1, 1)));
	XArrow->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));

	UCylinderComp* YArrow = AddComponent<UCylinderComp>();
	YArrow->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(90.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	YArrow->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));

	XArrow->SetupAttachment(ZArrow);
	YArrow->SetupAttachment(ZArrow);

	axisComponents.Add(ZArrow);
	axisComponents.Add(XArrow);
	axisComponents.Add(YArrow);

	RootComponent = ZArrow;

	ZArrow->SetDepth(1001);
	YArrow->SetDepth(1001);
	XArrow->SetDepth(1001);
}

void AEditorGizmos::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);
	
	AActor* SelectedActor  = FEditorManager::Get().GetSelectedActor();
	if (SelectedActor != nullptr && RootComponent)
	{
		FTransform GizmoTransform = RootComponent->GetComponentTransform();
		GizmoTransform.SetPosition(SelectedActor->GetActorTransform().GetPosition());
		FVector EulerRotation = SelectedActor->GetActorTransform().GetEulerRotation();
		GizmoTransform.SetRotation(EulerRotation);
		SetActorTransform(GizmoTransform);
	}	

	// if (SelectedAxis != ESelectedAxis::None)
	// {
	// 	if (AActor* Actor = FEditorManager::Get().GetSelectedActor())
	// 	{
	// 		// 마우스의 커서 위치를 가져오기
	// 		POINT pt;
	// 		GetCursorPos(&pt);
	// 		ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);
	//
	// 		RECT Rect;
	// 		GetClientRect(UEngine::Get().GetWindowHandle(), &Rect);
	// 		int ScreenWidth = Rect.right - Rect.left;
	// 		int ScreenHeight = Rect.bottom - Rect.top;
	//
	// 		// 커서 위치를 NDC로 변경
	// 		float PosX = 2.0f * pt.x / ScreenWidth - 1.0f;
	// 		float PosY = -2.0f * pt.y / ScreenHeight + 1.0f;
	// 		
	// 		// Projection 공간으로 변환
	// 		FVector4 RayOrigin {PosX, PosY, 1.0f, 1.0f};
	// 		
	// 		// View 공간으로 변환
	// 		FMatrix InvProjMat = UEngine::Get().GetRenderer()->GetProjectionMatrix().Inverse();
	// 		RayOrigin = RayOrigin * InvProjMat;
	// 		if (RayOrigin.W != 0) {
	// 			RayOrigin.X /= RayOrigin.W;
	// 			RayOrigin.Y /= RayOrigin.W;
	// 			RayOrigin.Z /= RayOrigin.W;
	// 			RayOrigin.W = 1;
	// 		}
	// 		
	// 		// 마우스 포인터의 월드 위치와 방향
	// 		FMatrix InvViewMat = FEditorManager::Get().GetCamera()->GetViewMatrix().Inverse();
	// 		RayOrigin = RayOrigin * InvViewMat;
	//
	// 		FVector4 RayDir = (RayOrigin - prevMousePos);
	// 		
	// 		FTransform AT = Actor->GetActorTransform();
	// 		float Result = 0;
	// 		switch (SelectedAxis)
	// 		{
	// 		case ESelectedAxis::X:
	// 			Result = RayDir.Dot(actorXAxis);
	// 			break;
	// 		case ESelectedAxis::Y:
	// 			Result = RayDir.Dot(actorYAxis);
	// 			break;
	// 		case ESelectedAxis::Z:
	// 			Result = RayDir.Dot(actorZAxis);
	// 			break;
	// 		default:
	// 			break;
	// 		}
	// 		
	// 		Result *= 0.005f;
	//
	// 		DoTransform(AT, Result, Actor);
	// 	}
	// }
	//
	// if (APlayerInput::Get().GetKeyDown(EKeyCode::Space))
	// {
 // 		int type = static_cast<int>(GizmoType);
	// 	type = (type + 1) % static_cast<int>(EGizmoType::Max);
	// 	GizmoType = static_cast<EGizmoType>(type);
	// }

}

void AEditorGizmos::SetScaleByDistance()
{
	FTransform MyTransform = GetActorTransform();
	
	// 액터의 월드 위치
	FVector actorWorldPos = MyTransform.GetPosition();

	FTransform CameraTransform = FEditorManager::Get().GetCamera()->GetActorTransform();
	
	// 카메라의 월드 위치
	FVector cameraWorldPos = CameraTransform.GetPosition();

	// 거리 계산
	float distance = (actorWorldPos - cameraWorldPos).Length();

	float baseScale = 1.0f;    // 기본 스케일
	float scaleFactor = distance * 0.1f; // 거리에 비례하여 스케일 증가

	// float minScale = 1.0f;     // 최소 스케일
	// float maxScale = 1.0f;     // 최대 스케일
	// float scaleFactor = clamp(1.0f / distance, minScale, maxScale);

	MyTransform.SetScale(scaleFactor, scaleFactor, scaleFactor);
}

ESelectedAxis AEditorGizmos::IsAxis(UCylinderComp* axis)
{
	if (axis == axisComponents[0]) {
		SelectedAxis = ESelectedAxis::Z;
	}
	else if (axis == axisComponents[1]) {
		SelectedAxis = ESelectedAxis::X;
	}
	else if (axis == axisComponents[2]) {
		SelectedAxis = ESelectedAxis::Y;
	}
	else {
		SelectedAxis = ESelectedAxis::None;
	}
	return SelectedAxis;
}

const char* AEditorGizmos::GetTypeName()
{
	return "GizmoHandle";
}

void AEditorGizmos::DoTransform(FTransform& AT, float Result, AActor* Actor )
{
	const FVector& AP = AT.GetPosition();

	if (SelectedAxis == ESelectedAxis::X)
	{
		switch (GizmoType)
		{
		case EGizmoType::Translate:
			AT.SetPosition({ AP + actorXAxis * Result });
			break;
		case EGizmoType::Rotate:
			AT.RotateRoll(Result * 15.0f);
			break;
		case EGizmoType::Scale:
			AT.AddScale({ Result * 0.5f, 0, 0 });
			break;
		}
	}
	else if (SelectedAxis == ESelectedAxis::Y)
	{
		switch (GizmoType)
		{
		case EGizmoType::Translate:
			AT.SetPosition({ AP + actorYAxis * Result });
			break;
		case EGizmoType::Rotate:
			AT.RotatePitch(Result * 15.0f);
			break;
		case EGizmoType::Scale:
			AT.AddScale({ 0, Result * 0.5f, 0 });
			break;
		}
	}
	else if (SelectedAxis == ESelectedAxis::Z)
	{
		switch (GizmoType)
		{
		case EGizmoType::Translate:
			AT.SetPosition({ AP + actorZAxis * Result });
			break;
		case EGizmoType::Rotate:
			AT.RotateYaw(-Result * 15.0f);
			break;
		case EGizmoType::Scale:
			AT.AddScale({0, 0, Result * 0.5f });
			break;
		}
	}
	Actor->SetActorTransform(AT);
}

