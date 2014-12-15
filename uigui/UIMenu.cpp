#include <UIGUI.h>
#include <UIMenu.h>
#include <UIScreen.h>
#include <UICaption.h>
#include <UIButton.h>
#include <iostream>
#include <stdio.h>

#define BUTTON_LIST_TOP  (200)

#define BUTTON_WIDTH  (200)
#define BUTTON_HEIGHT  (40)
#define BUTTON_VSPACE  (20)

UIMenu::UIMenu(UIGUI *ui)
{
	mUI = ui;
	mButtonCount = 0;
	mScreen = new UIScreen(mUI);
}

UIMenu::~UIMenu()
{
}

void UIMenu::addCmd(string name, int32_t cmd)
{
	SDL_Point *screenSize = UIGUI::getWindowSize();
	mScreen->AddElement((UIElement *) new UIButton(id_Any, screenSize->x/2-BUTTON_WIDTH/2, BUTTON_LIST_TOP+(BUTTON_HEIGHT+BUTTON_VSPACE)*mButtonCount, BUTTON_WIDTH, BUTTON_HEIGHT, cmd, name, "impact", 24, UIGUI::White));
	mButtonCount++;
}

void UIMenu::addSpacer()
{
	mButtonCount++;
}

int32_t UIMenu::display()
{
	int32_t cmd = 0;
	SDL_Event event;
	do {
		mUI->run(mScreen, &event);
		if (event.type == mScreen->getEventType()) {
			cmd = event.user.code;
			break;
		}
	} while(1);
	delete mScreen;
	UIScreen::setNeedRedraw();
	return cmd;
}
