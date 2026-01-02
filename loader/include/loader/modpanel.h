#pragma once

#include "loader/loader.h"

struct modpanel
{
	modpanel() : visible(true) {};

	bool visible; //< flag for if panel will be rendered or not

	virtual void render() = 0;
};

void LOADER_DLL add_panel(modpanel* new_panel);
void LOADER_DLL render_mod_panels();