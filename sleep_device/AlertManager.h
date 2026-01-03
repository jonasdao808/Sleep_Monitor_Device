#pragma once
#include <Arduino.h>

void Alert_Init();
void Alert_Update();
bool Alert_Send();

bool Alert_CanSend();
unsigned long Alert_TimeSinceLast();
