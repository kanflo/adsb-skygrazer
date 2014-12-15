#ifndef __UIMENU_H__
#define __UIMENU_H__

#include <UIGUI.h>
#include <UIButton.h>
#include <string>
#include <array>

using namespace std;
using std::string;

class UIMenu
{
	public:
		UIMenu(UIGUI *ui);
		~UIMenu();
		void addCmd(string name, int32_t cmd);
		void addSpacer();
		int32_t display();

	protected:
		UIGUI *mUI;
		UIScreen *mScreen;
		uint32_t mButtonCount;
};

#endif // __UIMENU_H__
