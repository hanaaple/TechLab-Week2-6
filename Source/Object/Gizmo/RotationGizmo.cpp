#include "RotationGizmo.h"
#include "Static/FEditorManager.h"

ARotationGizmo::ARotationGizmo()
{
	bIsGizmo = true;

	UTorusComp* ZRotation = AddComponent<UTorusComp>();
	ZRotation->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f), FVector(1.0f, 1.0f, 1.0f)));
	ZRotation->SetCustomColor(FVector4(0.0f, 0.0f, 1.0f, 1.0f));
	axisComponents.Add(ZRotation);

	UTorusComp* XRotation = AddComponent<UTorusComp>();
	XRotation->SetupAttachment(ZRotation);
	XRotation->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(0.0f, -90.0f, 0.0f), FVector(1, 1, 1)));
	XRotation->SetCustomColor(FVector4(1.0f, 0.0f, 0.0f, 1.0f));
	axisComponents.Add(XRotation);

	UTorusComp* YRotation = AddComponent<UTorusComp>();
	YRotation->SetupAttachment(ZRotation);
	YRotation->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 0.0f), FVector(90.0f, 0.0f, 0.0f), FVector(1, 1, 1)));
	YRotation->SetCustomColor(FVector4(0.0f, 1.0f, 0.0f, 1.0f));
	axisComponents.Add(YRotation);

	RootComponent = ZRotation;

	ZRotation->SetDepth(1001);
	XRotation->SetDepth(1001);
	YRotation->SetDepth(1001);

	SetActorVisibility(false);
}

void ARotationGizmo::Tick(float DeltaTime)
{
	AActor* SelectedActor = FEditorManager::Get().GetSelectedActor();
	if (SelectedActor != nullptr && RootComponent)
	{
		FTransform GizmoTransform = RootComponent->GetComponentTransform();
		GizmoTransform.SetPosition(SelectedActor->GetActorTransform().GetPosition());
		SetActorTransform(GizmoTransform);
	}
	AActor::Tick(DeltaTime);

	if (SelectedAxis != ESelectedAxis::None)
	{
		if (AActor* Actor = FEditorManager::Get().GetSelectedActor())
		{
			// 마우스의 커서 위치를 가져오기
			POINT pt;
			GetCursorPos(&pt);
			ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);
			RECT Rect;
			GetClientRect(UEngine::Get().GetWindowHandle(), &Rect);
			int ScreenWidth = Rect.right - Rect.left;
			int ScreenHeight = Rect.bottom - Rect.top;

			// 커서 위치를 NDC로 변경
			float PosX = 2.0f * pt.x / ScreenWidth - 1.0f;
			float PosY = -2.0f * pt.y / ScreenHeight + 1.0f;

			// Projection 공간으로 변환
			FVector4 RayOrigin{ PosX, PosY, 1.0f, 1.0f };

			FVector4 ndcOrigin{ 1.0f, PosX, PosY, 1.0f };

			FVector4 RayDir = (ndcOrigin - prevMousePos).GetSafeNormal();

			prevMousePos = ndcOrigin;

			// View 공간으로 변환
			FMatrix InvProjMat = UEngine::Get().GetRenderer()->GetProjectionMatrix().Inverse();
			RayOrigin = RayOrigin * InvProjMat;
			if (RayOrigin.W != 0) {
				RayOrigin.X /= RayOrigin.W;
				RayOrigin.Y /= RayOrigin.W;
				RayOrigin.Z /= RayOrigin.W;
				RayOrigin.W = 1;
			}

			// 마우스 포인터의 월드 위치와 방향
			FMatrix InvViewMat = FEditorManager::Get().GetCamera()->GetViewMatrix().Inverse();
			RayOrigin = RayOrigin * InvViewMat;

			FTransform AT = Actor->GetActorTransform();

			ACamera* cam = FEditorManager::Get().GetCamera();
			FVector GizmoCenter = Actor->GetActorTransform().GetPosition();
			FVector camX = cam->GetForward();
			FVector camY = cam->GetRight();
			FVector camZ = cam->GetUp();

			FVector RadialVector = (RayOrigin - GizmoCenter).GetSafeNormal();

			FVector EffectiveMovement = FVector::CrossProduct(RadialVector, RayDir).GetSafeNormal();

			float Result = 0;

			switch (SelectedAxis)
			{
			case ESelectedAxis::X:
				UE_LOG("X Axis Selected");
				Result = -FVector::DotProduct(EffectiveMovement, camX);
				break;
			case ESelectedAxis::Y:
				UE_LOG("Y Axis Selected");
				Result = FVector::DotProduct(EffectiveMovement, camY);
				break;
			case ESelectedAxis::Z:
				UE_LOG("Z Axis Selected");
				Result = -FVector::DotProduct(EffectiveMovement, camZ);
				break;
			default:
				break;
			}

			FVector CameraToObject = (GizmoCenter - cam->GetActorTransform().GetPosition()).GetSafeNormal();

			if (Result > 0.1f)
				Result = 0.1f;
			if (Result < -0.1f)
				Result = -0.1f;
			if (FMath::Abs(Result) < 0.005f) {
				if (Result > 0) {
					Result = 0.005f;
				}
				if (Result < 0) {
					Result = -0.005f;
				}
			}

			DoTransform(AT, Result, Actor);
		}
	}
}

void ARotationGizmo::SetScaleByDistance()
{
	FTransform MyTransform = GetActorTransform();

	// 액터의 월드 위치
	FVector actorWorldPos = MyTransform.GetPosition();

	FTransform CameraTransform = FEditorManager::Get().GetCamera()->GetActorTransform();

	// 카메라의 월드 위치
	FVector cameraWorldPos = CameraTransform.GetPosition();

	// 거리 계산
	float distance = (actorWorldPos - cameraWorldPos).Length();

	float baseScale = 0.1f;    // 기본 스케일
	float scaleFactor = distance * 0.1f; // 거리에 비례하여 스케일 증가

	// float minScale = 1.0f;     // 최소 스케일
	// float maxScale = 1.0f;     // 최대 스케일
	// float scaleFactor = clamp(1.0f / distance, minScale, maxScale);

	MyTransform.SetScale(scaleFactor, scaleFactor, scaleFactor);
}

void ARotationGizmo::SetActorVisibility(bool bNewActive)
{
	if (RootComponent != nullptr)
		RootComponent->SetVisibility(bNewActive);
}

ESelectedAxis ARotationGizmo::IsAxis(UTorusComp* axis)
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

void ARotationGizmo::SetPrevMousePos(FVector4 mouse)
{
	prevMousePos = mouse;
}

const char* ARotationGizmo::GetTypeName()
{
	return "RotationGizmo";
}

void ARotationGizmo::DoTransform(FTransform& AT, float Result, AActor* Actor)
{
	const FVector& AP = AT.GetPosition();
	if (SelectedAxis == ESelectedAxis::X)
	{
		AT.RotateRoll(Result * 15.0f);
	}
	else if (SelectedAxis == ESelectedAxis::Y)
	{
		AT.RotatePitch(Result * 15.0f);
	}
	else if (SelectedAxis == ESelectedAxis::Z)
	{
		AT.RotateYaw(-Result * 15.0f);
	}
	Actor->SetActorTransform(AT);
}
