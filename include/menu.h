#ifndef MENU_H
#define MENU_H
#include <functional>
#include <raylib.h>
#include <string>
#include <vector>

class Menu;

struct MenuText
{
    std::string     text;
    Font            font;
    int             size;
    Vector2         position;
};

struct MenuImage
{
    Texture2D   image;
    Vector2     position;
};

struct MenuSelector
{
    Texture2D   icon;
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
    bool        is_selected = false;
    bool        is_toggle = false;
    MenuText    label;
    Vector2     position;
    Vector2     icon_offset;

    Texture2D   background;
    Texture2D   background_hover;
    Texture2D   background_selected;
    
    std::function<void(Menu*, void*)> action;
    void        *data;
    int         height;
    int         width;
};

struct MenuButtonContext
{
    MenuButton *button;
    int         index;
};

struct MenuDropdown
{
    bool                        is_active = false;
    bool                        right_sided = true;
    MenuText                    label;
    MenuButton* add_button(const std::string& label_text, const Font& font, const Vector2 position, int width, int height,
                    const std::function<void(Menu* = nullptr, void* = nullptr)>& action, void* data = nullptr);
    std::vector<MenuButton*>    buttons;
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


    void add_label(const std::string& text, const Font& font, const int size, const Vector2 position);
    void add_image(Image* image, const Vector2 position);
    void set_base_offset(Vector2 offset);
    MenuButton* add_button(const std::string& label_text, const Font& font, const Vector2 position, int width, int height,
                           const std::function<void(Menu* = nullptr, void* = nullptr)>& action, void* data = nullptr);
    MenuDropdown* add_dropdown(const std::string& label_text, Font* font, const Vector2& position, Image* underlay);
    void toggle();
    void close();
    void open();

    void update();
    void update_fixed();
    void draw();
    void set_background(Image* bg);
    Menu* get_previous_menu() const;

    bool darken_background = true;
    bool is_focused = false;
    
private:
    bool is_open = false;
    Vector2 base_offset = {0, 0};
    std::vector<MenuText>       labels;
    std::vector<MenuImage>      images;
    std::vector<MenuButton*>    buttons;
    std::vector<MenuDropdown*>   dropdowns;

    MenuSelector        selector;
    MenuButtonContext   current_button_context;
    Menu                *previous_menu; // Points to the previous menu for "go back", null if root
    
    double move_cooldown = 200; // Cooldown in ms
    
    Image   *background;

    void draw_button(const MenuButton* button, Vector2 offset = {0, 0}) const;
    void draw_dropdown(const MenuDropdown* dropdown) const;


};

#endif // MENU_H