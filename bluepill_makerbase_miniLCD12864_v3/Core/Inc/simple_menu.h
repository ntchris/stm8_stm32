/*
 * simple_menu.h
 *
 *  Created on: Oct 25, 2022
 *      Author: chris
 */

#ifndef INC_SIMPLE_MENU_H_
#define INC_SIMPLE_MENU_H_

#include<string>
#include <U8g2lib.h>


//#define DebugPrint

#define MAX_ITEM_VALUE 255
class Item_Value {
public:
    bool _is_active;
    std::string _item_name;
    uint16_t _value;
    uint8_t _step = 1;
    Item_Value(std::string itemname, uint8_t v = 0, uint8_t step = 1) {
        _item_name = itemname;
        _value = v;
        _is_active = false;
        _step = step;
    }

    uint8_t increase() {
        _value += _step;
        if (_value > MAX_ITEM_VALUE) {
            _value = MAX_ITEM_VALUE;
        }
        return _value;
    }

    uint8_t decrease() {

        if (_value >= _step) {
            _value -= _step;
        }
        return _value;
    }

    void set_active() {
        _is_active = true;
#ifdef DebugPrint
        printf("sub item %s is now active: %u, value: %u: \n\r",_item_name.c_str(), _is_active, _value);
#endif
    }
    void set_inactive() {
        _is_active = false;
#ifdef DebugPrint
        printf("sub item %s is now active: %u, value: %u: \n\r",_item_name.c_str(), _is_active, _value);
#endif

    }

};

class Simple_Menu_Item_Value {
    const uint8_t MAX_MENU_ITEMS = 20;
    U8G2 *_u8g2p;
    Item_Value **_menu_itemp;
    uint8_t _menu_items_count = 0;
    uint8_t selected_index = 0;
    // when choosing menu item A, B, C..., it's active.
    // when user press a button, we enter A or enter B or enter C...it's inactive.
    bool _menu_active = true;
    const uint8_t  *_active_font=u8g_font_helvR08r, *_normal_font=u8g_font_helvR08;
public:
    void set_font(const uint8_t *normal_font, const uint8_t *active_font)
    {
        _active_font = active_font;
        _normal_font = normal_font;


    };

    Simple_Menu_Item_Value(U8G2 *u8g2);
    void add_menu_item(Item_Value *menuitemp);
    void show_menu(uint16_t x, uint16_t y);

    uint16_t get_active_menuitem_value()
    {
        return get_active_item()->_value;
    }
    uint16_t  get_menuitem_value(uint8_t index )
    {
        // to do : check index > count
        Item_Value *menuitemp = _menu_itemp[index];
        return menuitemp->_value;
    }
    void main_menu_next() {
        if ((selected_index + 1) < _menu_items_count) {
            selected_index++;
        }
    }
    void main_menu_prev() {
        if (selected_index >= 1) {
            selected_index--;
        }
    }

    // if in main menu, move to next item, if in submenu item, increase value
    void active_item_next() {
        if (_menu_active) {
            main_menu_next();
        }else
        {
            get_active_item()->increase();
        }
    }
    // if in main menu, move to prev item, if in submenu item, decrease value
    void active_item_prev() {
        if (_menu_active) {
            main_menu_prev();
        }else
        {
            get_active_item()->decrease();
        }
    }
    void set_active_item(uint8_t index) {
        // to do : check index vs _menu_items_count
        selected_index = index;
    }
    Item_Value* get_active_item() {
        Item_Value *menuitemp = _menu_itemp[selected_index ];
        return menuitemp;

    }
    // enter the current selected item
    void click_on_current_menu_item() {
#ifdef DebugPrint
        printf("click_on_current_menu_item, active was  %u\n\r", _menu_active);
#endif
        _menu_active = !_menu_active;
#ifdef DebugPrint
        printf("active is now %u\n\r", _menu_active);
#endif

        Item_Value *item_p = get_active_item();

        if (!_menu_active) {
            // entered the sub menu item
            item_p->set_active();
        } else {
            // already in sub menu item

            item_p->set_inactive();
        }

    }

};

#endif /* INC_SIMPLE_MENU_H_ */
