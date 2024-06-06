#include "menu.h"

#include <iostream>
#include <ranges>
#include <raymath.h>

Menu::Menu()
{
    previous_menu = nullptr;
    background = nullptr;
    selector.icon = LoadTexture("assets/gui/selector.png");
    selector.position = {30,30};
    selector.target_position = selector.position;
    current_button_context.button = nullptr;
    is_focused = false;
}

Menu::~Menu()
= default;

void Menu::add_label(const std::string& text, const Font& font, const int size, const Vector2 position)
{
    labels.push_back({text, font, size, position});
}

void Menu::add_image(Image* image, const Vector2 position)
{
    MenuImage menu_image;
   // menu_image.image = image;
    menu_image.position = position;
    images.push_back(menu_image);
    
}

void Menu::set_base_offset(Vector2 offset)
{
    base_offset = offset;
}

MenuButton* MenuDropdown::add_button(const std::string& label_text, const Font& font, const Vector2 position, int width,
                                     int height,
                                     const std::function<void(Menu*, void*)>& action, void* data)
{
    auto *button = new MenuButton;
    button->label.text = label_text;
    button->label.font = font;
    button->label.position = {25,5};
    button->position = position;
    button->icon_offset = {5, 3};
    button->background = LoadTexture("assets/gui/button.png");
    button->background_selected = LoadTexture("assets/gui/button_selected.png");
    button->width = 200;
    button->height = 20;
    button->action = action;
    button->data = data;
    buttons.push_back(button);
    if( !current_button )
    {
        current_button = button;
        //current_button.index = static_cast<int>(buttons.size()) - 1;
    }
    return button;
}


MenuButton* Menu::add_button(const std::string& label_text, const Font& font, const Vector2 position, int width,
                             int height,
                             const std::function<void(Menu*, void*)>& action, void* data)
{
    auto *button = new MenuButton;
    button->label.text = label_text;
    button->label.font = font;
    button->label.position = {25,5};
    button->position = position;
    button->icon_offset = {5, 3};
    button->background = LoadTexture("assets/gui/button.png");
    button->background_selected = LoadTexture("assets/gui/button_selected.png");
    button->width = 200;
    button->height = 20;
    button->action = action;
    button->data = data;
    buttons.push_back(button);
    if( !current_button_context.button )
    {
        current_button_context.button = button;
        current_button_context.index = static_cast<int>(buttons.size()) - 1;
    }
    return button;
}

MenuDropdown* Menu::add_dropdown(const std::string& label_text, Font* font, const Vector2& position, Image* underlay)
{
    auto *dropdown = new MenuDropdown;
    dropdown->label.text = label_text;
    dropdown->offset = position;

    dropdowns.push_back(dropdown);
    return dropdown;
}

void Menu::toggle()
{
    is_open = !is_open;
    if( is_open )
    {
        is_focused = true;
    }
}

void Menu::close()
{
    is_open = false;
    is_focused = false;
}

void Menu::open()
{
    is_open = true;
    is_focused = true;
}

double LAST_MOVE_TIME = 0;
void Menu::update()
{
    if( is_open && is_focused )
    {
        const double current_time = GetTime();
        if( current_time - LAST_MOVE_TIME > (move_cooldown / 1000))
        {
            if(IsKeyDown(KEY_DOWN) || IsKeyPressed(KEY_DOWN))
            {
                if (!buttons.empty())
                {
                    if (current_button_context.index < buttons.size() - 1)
                    {
                        current_button_context.index += 1;
                        current_button_context.button = buttons[current_button_context.index];
                    }
                    else
                    {
                        current_button_context.index = 0;
                        current_button_context.button = buttons[0];
                    }
                }
                LAST_MOVE_TIME = current_time;

            }
        }

        auto [mx, my] = GetMousePosition();
        mx -= base_offset.x;
        my -= base_offset.y;
        for( int i = 0; i < buttons.size(); i++ )
        {
            const auto button = buttons[i];
            if( mx > button->position.x && mx < button->position.x + button->width
                && my > button->position.y && my < button->position.y + button->height)
            {
                current_button_context.button = button;
                current_button_context.index = i;
                if( IsMouseButtonDown(0) )
                {
                    if( !button->is_toggle )
                        button->is_selected = true;

                    if( IsMouseButtonPressed(0))
                    {
                        if( button->is_toggle )
                            button->is_selected = !button->is_selected;
                    
                        if( button->action )
                            button->action(this, button->data);
                    }
                }
                else if( !button->is_toggle )
                {
                    button->is_selected = false;
                }
            
            }
        }
    }
    
}


void Menu::update_fixed()
{
    if(current_button_context.button)
    {
        const MenuButton *current_button = current_button_context.button;
        selector.target_position = Vector2Add(current_button->position, current_button->icon_offset);
        selector.position = Vector2Lerp( selector.position, selector.target_position, 0.2f);
    }

}

void Menu::draw_button(const MenuButton *button, Vector2 offset) const
{
    if( !button ) return;
    if( button->is_selected )
    {
        DrawTexture(button->background_selected, static_cast<int>(button->position.x + offset.x + base_offset.x), static_cast<int>(button->position.y + offset.y + base_offset.y), WHITE);
        if( button->is_toggle )
        {
            DrawTexture(selector.icon, static_cast<int>(button->position.x + offset.x + button->width - 20 + base_offset.x), static_cast<int>(button->position.y + offset.y + 2 + base_offset.y), GREEN);
        }
    }else
    {
        DrawTexture(button->background, static_cast<int>(button->position.x + offset.x + base_offset.x), static_cast<int>(button->position.y + offset.y + base_offset.y), WHITE);
    }
    const Vector2 label_offset = Vector2Add( button->position, button->label.position );
    DrawText(button->label.text.c_str(), static_cast<int>(label_offset.x + base_offset.x), static_cast<int>(label_offset.y + base_offset.y), 10, WHITE);
}

void Menu::draw_dropdown(const MenuDropdown *dropdown) const
{
    if(!dropdown) return;
    for(const auto& button : dropdown->buttons)
    {
        draw_button(button, dropdown->offset);
    }
}

void Menu::draw()
{
    if( !is_open ) return;
    if( darken_background )
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 128});
    for(const auto& [text, font, size, position] : labels)
    {
        DrawText(text.c_str(), static_cast<int>(position.x + base_offset.x), static_cast<int>(position.y + base_offset.y), size, WHITE);
    }

    for(const auto& button : buttons)
    {
        draw_button(button);
    }

    for( const auto& dropdown : dropdowns)
    {
        draw_dropdown(dropdown);
    }

    DrawTexture(selector.icon, static_cast<int>(selector.position.x + base_offset.x), static_cast<int>(selector.position.y + base_offset.y), WHITE);
}

void Menu::set_background(Image* bg)
{
    //
}

Menu* Menu::get_previous_menu() const
{
    return previous_menu;
}
