/*
 * simple_menu.cpp
 *
 *  Created on: Oct 25, 2022
 *      Author: chris
 */

#include <simple_menu.h>

Simple_Menu_Item_Value::Simple_Menu_Item_Value(U8G2 *u8g2) {
    _u8g2p = u8g2;
    _menu_active = true;
    _menu_itemp = new Item_Value*[MAX_MENU_ITEMS];
}

void Simple_Menu_Item_Value::add_menu_item(Item_Value *menuitemp) {
    // to do  check add too many
    _menu_itemp[_menu_items_count] = menuitemp;
    _menu_items_count++;
}

void Simple_Menu_Item_Value::show_menu(uint16_t x, uint16_t y) {
    uint16_t i, screen_width, step;
    screen_width = _u8g2p->getDisplayWidth();
    step = screen_width / _menu_items_count;
    const uint8_t delta_x = 6;
    std::string item;
    const uint8_t value_y_delta = 12;
    for (i = 0; i < _menu_items_count; i++) {
        // draw each menu item, and their values
        Item_Value *p = _menu_itemp[i];
        if (p->_is_active) {
            _u8g2p->setFont(_active_font);
        } else {
            _u8g2p->setFont(_normal_font);
        }
        if (i == this->selected_index) {
             item = "[ " + p->_item_name + " ]";

            _u8g2p->drawStr(x, y, item.c_str());
            // draw menu item value
            item = std::to_string(p->_value);
            _u8g2p->drawStr(x + delta_x, y+value_y_delta,  item.c_str() );

        } else {
            _u8g2p->drawStr(x + delta_x, y, p->_item_name.c_str());
            item = std::to_string(p->_value);
            // draw menu item value
            _u8g2p->drawStr(x + delta_x, y+value_y_delta,  item.c_str() );
        }
        x += step;

    }

}
