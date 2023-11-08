#pragma once
#include "ConsoleFrame.h"
#include "clist.h"


// Use this vars to change buttons length or 'back' button label.
const char BACKWARD_BUTTON_TEXT[] = "<- Back";
const int ITEM_LABEL_MAX_LENGTH = 50;


class Menu;

// Serve to provide menu pages transitions and user interaction logics.
class Console {
private:
    static Menu* currentMenu;
    Console() = default;
    static std::pair<bool,char> getch();
public:
    struct keyListener {
        char key = 0;
        std::function<void(void)> handler = nullptr;
        bool isServiceKey = false;
    };
    static Console& console();
    static void setMenu(Menu*);
    static void show();    
};

// Represents the menu page
class Menu
{
public:
    struct Item;
    //Abstraction over the console to work with menus. Use this instance to set the current menu or to display setted.
    static Console console;
private:
    friend class Console;
    friend struct Item;
    clist<Menu::Item> items;
    std::list<Console::keyListener> keyListeners;
    bool backButtonSetted = false;
    const char* text  = nullptr;
    std::function<void(void)> printingFunction = nullptr;
    Menu* where_called_from = nullptr;
    bool isInputMenu = false;
    void printButtons(clist<Menu::Item>::iterator&);
public:
    // Represents the menu buttons
    struct Item {

        friend class Menu;
        Menu* goToRef = nullptr;
        Menu* location = nullptr;
        std::function<void()> action = nullptr;
        const char* label;
    
        Item() = default;
        //MenuItem that makes transition to other menu
        Item(const char* label , Menu& goToRef);
        // MenuItem that makes an action described in recieved function
        Item(const char* label, std::function<void(void)> action);
        Item(const Item& other);
        // Transition function (Generally calls by Console)
        void go(); 
    };
    // Menu, using text as an own content.
    Menu(const char* text);
    // Menu, calls received function as a printing method and/or in other purposes like user input.
    Menu(std::function<void(void)> printingFunction, bool isInputMenu = false);
    Menu(const Menu& other) = default;
    // Embeds the button in menu.
    void addItem(Item item);
    // Sets the keylistener to menu. Handler calls when user presses a char with the stated keycode. IsServiceKey sets is it necessary to ignore the first key code, usually provided with a service key 
    void addKeyListener(char keycode, std::function<void(void)> handler, bool isServiceKey = false);
};

typedef Menu::Item MenuItem;

