#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Gui.h"
#include "j1Scene.h"
#include <time.h>

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{

	tab_it = TabList.start;
	tabbing = false;

	if(App->map->Load("iso_walk.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if(App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	debug_tex = App->tex->Load("maps/path2.png");

	// WOW UI ---

	// background
	int bck_it = 0;
		
	background[bck_it++] = CREATEIMAGE("wow ui/login_background.png");
	background[bck_it++] = CREATEIMAGE("wow ui/login_background2.jpg");

	for (int i = 0; i < bck_it; i++) {
		background[i]->texture_rect.x = 0;
		background[i]->texture_rect.y = 0;
		background[i]->texture_rect.w = 1920;
		background[i]->texture_rect.h = 1080;
	}

	srand(time(NULL));

	background[rand()%2]->active = false;

	// wow banner
	WowBanner[0] = CREATEIMAGE("wow ui/COMMON/Glues-Logo-Left.png");
	WowBanner[1] = CREATEIMAGE("wow ui/COMMON/Glues-Logo-Right.png");

	for (int i = 0; i < 2; i++) {
		WowBanner[i]->texture_rect.x = 0;
		WowBanner[i]->texture_rect.y = 0;
		WowBanner[i]->texture_rect.w = 256;
		WowBanner[i]->texture_rect.h = 256;
	}
	WowBanner[0]->pos.x = 20;
	WowBanner[0]->pos.y = 10;
	WowBanner[1]->pos.x = WowBanner[0]->pos.x + 256;
	WowBanner[1]->pos.y = WowBanner[0]->pos.y;


	// blizzard logo
	BlizzLogo = CREATEIMAGE("wow ui/MainMenu/Glues-BlizzardLogo.png");
	BlizzLogo->texture_rect.w = 128;
	BlizzLogo->texture_rect.h = 128;
	BlizzLogo->pos.x = (1920 / 2) - BlizzLogo->texture_rect.w/2;
	BlizzLogo->pos.y = 950;
	
	// login button
	LoginBut = (GuiButton*)App->gui->CreateElement(GuiType::button);
	LoginBut->image[GuiButton::standard]->texture = App->tex->Load("wow ui/BUTTONS/UI-DialogBox-Button-Up.png");
	LoginBut->image[GuiButton::left_clicked]->texture = App->tex->Load("wow ui/BUTTONS/UI-DialogBox-Button-Down.png");
	LoginBut->image[GuiButton::hover]->texture = LoginBut->image[GuiButton::standard]->texture;
	LoginBut->image[GuiButton::disabled]->texture = App->tex->Load("wow ui/BUTTONS/UI-DialogBox-Button-Disabled.png");
	LoginBut->image[GuiButton::tabbed]->texture = LoginBut->image[GuiButton::left_clicked]->texture;
	
	for (int i = GuiButton::standard; i < GuiButton::Unknown; i++) {
		if (LoginBut != nullptr) {
			LoginBut->image[i]->texture_rect.x = 0;
			LoginBut->image[i]->texture_rect.y = 0;
			LoginBut->image[i]->texture_rect.w = 128;
			LoginBut->image[i]->texture_rect.h = 32;
		}
	}

	// start button
	StartBut = (GuiButton*)App->gui->CreateElement(GuiType::button);
	StartBut->image[GuiButton::standard]->texture = App->tex->Load("wow ui/BUTTONS/UI-DialogBox-Button-Up.png");
	StartBut->image[GuiButton::left_clicked]->texture = App->tex->Load("wow ui/BUTTONS/UI-DialogBox-Button-Down.png");
	StartBut->image[GuiButton::hover]->texture = LoginBut->image[GuiButton::standard]->texture;
	StartBut->image[GuiButton::disabled]->texture = App->tex->Load("wow ui/BUTTONS/UI-DialogBox-Button-Disabled.png");
	StartBut->image[GuiButton::tabbed]->texture = App->tex->Load("UI-DialogBox-Button-Highlight.png");

	for (int i = GuiButton::standard; i < GuiButton::Unknown; i++) {
		if (StartBut != nullptr) {
			StartBut->image[i]->texture_rect.x = 0;
			StartBut->image[i]->texture_rect.y = 0;
			StartBut->image[i]->texture_rect.w = 128;
			StartBut->image[i]->texture_rect.h = 32;
		}
	}

	LoginBut->pos.x = (1920 / 2) - LoginBut->image[GuiButton::standard]->texture_rect.w / 2;
	LoginBut->pos.y = 50;

	StartBut->pos.x = (1920 / 2) - StartBut->image[GuiButton::standard]->texture_rect.w / 2;
	StartBut->pos.y = 100;

	// cursor
	//Cursor = CREATEIMAGE("wow ui/CURSOR/Point.png");
	//Cursor->texture_rect.x = 0;
	//Cursor->texture_rect.y = 0;
	//Cursor->texture_rect.w = 32;
	//Cursor->texture_rect.h = 32;

	TabList.add(LoginBut);
	TabList.add(StartBut);

	// ----------

	// TODO 3: Create the image (rect {485, 829, 328, 103}) and the text "Hello World" as UI elements
	banner = (GuiImage*)App->gui->CreateElement(GuiType::image);
	banner->texture_rect.x = 485;
	banner->texture_rect.y = 829;
	banner->texture_rect.w = 328;
	banner->texture_rect.h = 103;

	banner->active = false;
	
	text = (GuiText*)App->gui->CreateElement(GuiType::text);
	text->pos.x = 250;
	text->pos.y = 50;
	text->string.create("Hello World");

	text->active = false;

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{

	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if(App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if(origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	// Gui ---
	
	//App->input->GetMousePosition(Cursor->pos.x, Cursor->pos.y);
	
	// Tab Button
	
	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
		tabbing = true;

	if (tabbing == true && App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN) {
		if (tab_it == nullptr)
			tab_it = TabList.start;
		else
			if (tab_it->next != NULL) {
				tab_it = tab_it->next;
			}
			else
				tab_it = TabList.start;
	}

	if (tabbing == true)
		tab_it->data->state = GuiButton::tabbed;

	// -------

	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= floor(200.0f * dt);

	App->map->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d MousePos: %d, %d LOGIN POS: %d %d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),
					map_coordinates.x, map_coordinates.y,
					x, y,
					LoginBut->pos.x, LoginBut->pos.y);

	App->win->SetTitle(title.GetString());

	// Debug pathfinding ------------------------------
	//int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(debug_tex, p.x, p.y);

	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for(uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(debug_tex, pos.x, pos.y);
	}

	if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN) {
		if (LoginBut->state == GuiButton::disabled)
			LoginBut->state = GuiButton::standard;
		else
		LoginBut->state = GuiButton::disabled;
	}

	switch (LoginBut->state) {
	case GuiButton::left_clicked:
		LoginBut->text->string.create("LOGIN LEFT CLICK");
		break;
	case GuiButton::right_clicked:
		LoginBut->text->string.create("LOGIN RIGHT CLICK");
		break;
	case GuiButton::hover:
		LoginBut->text->string.create("LOGIN HOVER");
		break;
	case GuiButton::disabled:
		LoginBut->text->string.create("LOGIN DISABLED");
		break;
	case GuiButton::tabbed:
		LoginBut->text->string.create("LOGIN TAB");
		break;
	case GuiButton::standard:
		LoginBut->text->string.create("LOGIN IDLE");
		break;
	default:
		LoginBut->text->string.create("LOGIN ERROR");
		break;
	}

	switch (StartBut->state) {
	case GuiButton::left_clicked:
		StartBut->text->string.create("START LEFT CLICK");
		break;
	case GuiButton::right_clicked:
		StartBut->text->string.create("START RIGHT CLICK");
		break;
	case GuiButton::hover:
		StartBut->text->string.create("START HOVER");
		break;
	case GuiButton::disabled:
		StartBut->text->string.create("START DISABLED");
		break;
	case GuiButton::tabbed:
		StartBut->text->string.create("START TAB");
		break;
	case GuiButton::standard:
		StartBut->text->string.create("START IDLE");
		break;
	default:
		StartBut->text->string.create("START ERROR");
		break;
	}

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
