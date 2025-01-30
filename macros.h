#pragma once
#define D(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, TEXT(x));}
#define D_VAR(x, y) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, FString::Printf(TEXT(x), y));}
// return FString::Printf(TEXT("X=%3.3f Y=%3.3f Z=%3.3f"), X, Y, Z)
#define D_VEC(Color, VecNameName, VecName) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Color, FString::Printf(TEXT(VecNameName ": X=%3.3f Y=%3.3f Z=%3.3f"), VecName.X, VecName.Y, VecName.Z));}
#define D_ROT(Color, RotNameName, RotName) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Color, FString::Printf(TEXT(RotNameName ": X=%3.3f Y=%3.3f Z=%3.3f"), RotName.Pitch, RotName.Yaw, RotName.Roll));}
#define D_LINE(VectorStart, VectorEnd, Multiplier, Color, Thickness) DrawDebugLine(GetWorld(), VectorStart, VectorEnd * Multiplier, FColor::Color, false, 0, 0, Thickness);