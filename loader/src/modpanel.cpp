#include "loader/modpanel.h"

#include <vector>

std::vector<modpanel*> panels;

void LOADER_DLL add_panel(modpanel* new_panel)
{
	panels.emplace_back(new_panel);
	loader_log_debug("new panel added at {}", panels.size());
}

void LOADER_DLL render_mod_panels()
{
	for (modpanel* panel : panels)
	{
		if (panel->visible)
		{
			panel->render();
		}
	}
}
