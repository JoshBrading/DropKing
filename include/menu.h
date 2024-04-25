#ifndef MENU_H
#define MENU_H
#include <raylib.h>
#include <string>
#include <vector>

class Menu;

struct MenuText
{
    std::string     text;
    Font*           font;
    Vector2         position;
};

struct MenuImage
{
    Image       *image;
    Vector2     position;
};

struct MenuSelector
{
    MenuImage   icon;
    Vector2     position;
    Vector2     target_position;
};

struct MenuSounds
{
    Sound   *button_hover;
    Sound   *button_click;
    
    Sound   *dropdown_open;
    Sound   *dropdown_close;
    
    Sound   *menu_open;
    Sound   *menu_close;
};

struct MenuButton
{
    bool        is_selected;
    MenuText    label;
    Vector2     position;
    Vector2     icon_offset;

    Image       background;
    Image       background_selected;
    
    void        (*action)(Menu* self, void* data);
    void        *data;
    int         height;
    int         width;
};

struct MenuDropdown
{
    bool                        is_active;
    MenuText                    label;
    std::vector<MenuButton>     *buttons;
    MenuButton                  *current_button;
    MenuButton                  *previous_button;
    Vector2                     offset;

    Image                       underlay;
};

class Menu
{
public:
    Menu();
    Menu(Menu&&) = delete;
    Menu(Menu&) = delete;
    ~Menu();


    void add_label(const std::string& text, Font* font, const Vector2& position);
    void add_image(Image* image, const Vector2& position);
    void add_button(const std::string& label_text, Font* font, const Vector2& position, int width, int height, void (*action)(Menu* self, void* data), void* data = nullptr);
    void add_dropdown(const std::string& label_text, Font* font, const Vector2& position, Image* underlay);
    
    void draw();
    void set_background(Image* bg);
    Menu* get_previous_menu() const;
    
private:
    std::vector<MenuText>       *labels;
    std::vector<MenuImage>      *images;
    std::vector<MenuButton>     *buttons;
    std::vector<MenuDropdown>   *dropdowns;

    MenuButton  *current_button;
    Menu        *previous_menu; // Points to the previous menu for "go back", null if root

    Image   background;

};

#endif // MENU_H