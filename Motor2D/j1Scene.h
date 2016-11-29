#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "p2DynArray.h"

struct SDL_Texture;
class UIElement;
class GuiImage;
class GuiText;
class GuiButton;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

private:
	SDL_Texture* debug_tex;
	GuiImage* banner;
	GuiText* text;

	// WOW UI

	GuiImage* background[2];
	GuiImage* WowBanner[2];
	GuiImage* Cursor;
	GuiImage* BlizzLogo;

	GuiButton* LoginBut;
	GuiButton* StartBut;

	p2List<GuiButton*> TabList;
	p2List_item<GuiButton*>* tab_it;
	bool tabbing;

	// !WOW UI 
};

#endif // __j1SCENE_H__