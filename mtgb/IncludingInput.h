#pragma once

// DIRECTINPUT_VERSION が再定義されています警告を消すための

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>
#include <Xinput.h>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")
#pragma comment(lib, "Xinput.lib")
