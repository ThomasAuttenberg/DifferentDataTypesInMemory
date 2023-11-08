#include "Menu.h"
#include <conio.h>
#include <windows.h>

// Static fields
Menu* Console::currentMenu = nullptr;
Console Menu::console = Console::console();


//============================================================================
// MenuItem BLOCK: Menu fields & console manipulations
// directed on getting a correct behaivor (makes clear who's called the menu,
//										  sets the current menu in console
//										  executes Item actions				)
//============================================================================



Menu::Item::Item(const char* label, Menu& goToRef) 
{
	this->label = label;
	this->goToRef = &goToRef;
}

Menu::Item::Item(const char* label, std::function<void(void)> action) 
{
	this->label = label;
	this->action = action;
}

Menu::Item::Item(const Item& other)
{
	this->action = other.action;
	this->goToRef = other.goToRef;
	this->label = other.label;
	this->location = other.location;
}


void Menu::Item::go() { 
	
	if (action != nullptr) {
		action();
	}
	if (goToRef != nullptr) {
		if (location != nullptr) {
			if(goToRef != location->where_called_from)
				goToRef->where_called_from = location;
		}
		Console::console().setMenu(goToRef);
	}
}

//============================================================================
// Menu BLOCK: Buttons and text printing logic, text and functions storage
//============================================================================


void Menu::printButtons(clist<Menu::Item>::iterator& selected)
{
		std::cout << "\n";
		items.foreach([&](Item& item) -> void {
			if (&(*selected) == &item) {
				int label_length = strlen(item.label);
				std::cout << "\n"<<"\033[0;30;47m" << item.label << std::string(ITEM_LABEL_MAX_LENGTH - label_length, ' ') << "\033[0m";
			}
			else {
				std::cout << "\n" << item.label;
			}
			});
		std::cout << "\n";
}

void Menu::addKeyListener(char keycode, std::function<void(void)> handler, bool isServiceKey)
{
	Console::keyListener listener;
	listener.key = keycode;
	listener.handler = handler;
	listener.isServiceKey = isServiceKey;
	keyListeners.push_back(listener);
}


Menu::Menu(const char* text)
{
	this->text = text;
	printingFunction = [&]() -> void {
		std::cout << this->text;
	};

}

Menu::Menu(std::function<void(void)> printingFunction, bool isInputMenu)
{
	this->printingFunction = printingFunction;
	this->isInputMenu = isInputMenu;
}

void Menu::addItem(Item item) {
	if(where_called_from != nullptr) items.shift_forward();
	item.location = this;
	items.push_back(item);
	if(where_called_from != nullptr) items.shift_backward();
}

//============================================================================
// CONSOLE BLOCK: Direct console output | user interaction handler
//============================================================================

std::pair<bool,char> Console::getch()
{
	char n = _getch();
	if (n != 224 && n != -32) return {0,n};
	return {1,_getch()};
}

void Console::setMenu(Menu* ref)
{
	currentMenu = ref;
}

Console& Console::console()
{
	static Console entity;
	return entity;
}

void Console::show()
{
	if (currentMenu == nullptr) return;
	while (1) {
		Menu* menuOnIterationStart = currentMenu;
		if (currentMenu->isInputMenu == true) {
				currentMenu->printingFunction();
				if (menuOnIterationStart != currentMenu) continue;
				if (currentMenu->where_called_from != nullptr) {
					setMenu(currentMenu->where_called_from);
					continue;
				}
				else {
					break;
				}
		}
		else {
			std::pair<bool,char> pressedKey; // first value: is service symbol? second value: symbol code.
			pressedKey.first = 0;
			pressedKey.second = -1;
			if (currentMenu->where_called_from != nullptr && !currentMenu->backButtonSetted) {
				MenuItem newItem(BACKWARD_BUTTON_TEXT, *currentMenu->where_called_from);
				currentMenu->items.push_back(newItem);
				currentMenu->backButtonSetted = true;
			}
			auto selectedButton = currentMenu->items.begin();
			bool hasActionOccuredPageChanging = false;
			while (1) {
				do {
					
					if (pressedKey.first == 1) {
						switch (pressedKey.second) {
						case 80: selectedButton++; break; //down arrow
						case 72: selectedButton--; break; //up arrow
						}
					}

					// User KeyListeners setted to this menu page
					for (keyListener listener : currentMenu->keyListeners) {
						if (listener.key == pressedKey.second && listener.isServiceKey == pressedKey.first)
							listener.handler();
					}

					system("cls");
					currentMenu->printingFunction();
					currentMenu->printButtons(selectedButton);
					pressedKey = getch();
				} while (!(pressedKey.first == 0 && pressedKey.second == 13));

				if ((*selectedButton).action != nullptr) {
					(*selectedButton).action();
					if (currentMenu != menuOnIterationStart) {
						hasActionOccuredPageChanging = true;
						break;
					}
					system("cls");
				}
				else {
					if ((*selectedButton).goToRef != nullptr) {
						break;
					}
				}

			}
			if(!hasActionOccuredPageChanging) (*selectedButton).go();
		}
	}
}
