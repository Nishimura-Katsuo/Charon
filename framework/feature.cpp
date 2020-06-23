#include "../headers/feature.h"
#include <iostream>

HotkeyCallbackMap HotkeyCallbacks;
AutomapInfoCallbackList AutomapInfoHooks;
InputCallbackMap ChatInputCallbacks;
StateMap State;
Feature* Features;

Feature::Feature() {
	this->next = Features;
	Features = this;
}

Feature::~Feature() { }
void Feature::init() { }
void Feature::deinit() { }
void Feature::gameLoop() { }
void Feature::oogLoop() { }
void Feature::gameUnitPreDraw() { }
void Feature::gameUnitPostDraw() { }
void Feature::gameAutomapPreDraw() { }
void Feature::gameAutomapPostDraw() { }
void Feature::gamePostDraw() { }
void Feature::oogPostDraw() { }
