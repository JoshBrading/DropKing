#include "menu.h"

#include <iostream>
#include <raymath.h>

Menu::Menu()
{
    previous_menu = nullptr;
    background = nullptr;
    selector.icon = LoadTexture("assets/gui/selector.png");
    selector.position = {30,30};
    selector.target_position = selector.position;
    current_button_context.button = nullptr;
}

Menu::~Menu()
{
}

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

void Menu::add_button(const std::string& label_text, const Font& font, const Vector2 position, int width, int height,
    void(* action)(Menu* self, void* data), void* data)
{
    auto *button = new MenuButton;
    button->label.text = label_text;
    button->label.font = font;
    button->label.position = {25,5};
    button->position = position;
    button->icon_offset = {5, 3};
    button->background = LoadTexture("assets/gui/button.png");
    button->width = 200;
    button->height = 20;
    button->action = action;
    buttons.push_back(button);
    if( !current_button_context.button )
    {
        current_button_context.button = button;
        current_button_context.index = static_cast<int>(buttons.size()) - 1;
    }
}

void Menu::add_dropdown(const std::string& label_text, Font* font, const Vector2& position, Image* underlay)
{
}

void Menu::toggle()
{
    is_open = !is_open;
}

double LAST_MOVE_TIME = 0;
void Menu::update()
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
    for( int i = 0; i < buttons.size(); i++ )
    {
        const auto button = buttons[i];
        if( mx > button->position.x && mx < button->position.x + button->width
            && my > button->position.y && my < button->position.y + button->height)
        {
            current_button_context.button = button;
            current_button_context.index = i;
            if( IsMouseButtonPressed(0) )
            {
                labels[0].text = "CLICKED";
                current_button_context.button->action(this, current_button_context.button->data);
            }
        }
    }
    
    if( IsKeyPressed(KEY_ESCAPE))
    {
        toggle();
    }
}


void Menu::update_fixed()
{
    if(current_button_context.button)
    {
        const MenuButton *current_button = current_button_context.button;
        selector.target_position = Vector2Add(current_button->position, current_button->icon_offset);
        selector.position = Vector2Lerp( selector.position, selector.target_position, 0.15f);
    }

}

void Menu::draw() const
{
    if( !is_open ) return;
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), {0, 0, 0, 128});
    for(const auto& [text, font, size, position] : labels)
    {
        DrawText(text.c_str(), static_cast<int>(position.x), static_cast<int>(position.y), size, WHITE);
    }

    for(const auto& button : buttons)
    {
        if( !button ) continue;
        DrawTexture(button->background, static_cast<int>(button->position.x), static_cast<int>(button->position.y), WHITE);
        const Vector2 label_offset = Vector2Add( button->position, button->label.position );
        DrawText(button->label.text.c_str(), static_cast<int>(label_offset.x), static_cast<int>(label_offset.y), 10, WHITE);
    }

    DrawTexture(selector.icon, static_cast<int>(selector.position.x), static_cast<int>(selector.position.y), WHITE);
}

void Menu::set_background(Image* bg)
{
}

Menu* Menu::get_previous_menu() const
{
    return nullptr;
}
