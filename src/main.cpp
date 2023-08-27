#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <array>

constexpr int MAP_X_SIZE = 5;
constexpr int MAP_Y_SIZE = 5;

using Tile = sf::Uint16;

constexpr std::array<Tile, MAP_X_SIZE* MAP_Y_SIZE> map = {
    1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1,
};

Tile get_map_tile(int x, int y)
{
    return map.at(y * MAP_X_SIZE + x);
}

struct Entity
{
    sf::Vector2f position;
    float direction = 0;
};

struct PreviewRender
{
    constexpr static float QUAD_SIZE = 16.0f;

    sf::Vertex m_player_v1;
    sf::Vertex m_player_v2;
    sf::RectangleShape quad;


    PreviewRender()
    {
        quad.setSize({QUAD_SIZE, QUAD_SIZE});
        quad.setOutlineColor(sf::Color::Black);
        quad.setOutlineThickness(2.0f);
    }

    void render_preview(sf::RenderTarget& render_target, const Entity& camera)
    {
        for (int y = 0; y < MAP_Y_SIZE; y++)
        {
            for (int x = 0; x < MAP_Y_SIZE; x++)
            {
                quad.setPosition(x * QUAD_SIZE, y * QUAD_SIZE);

                auto tile = get_map_tile(x, y);
                switch (tile)
                {
                    case 0:
                        quad.setFillColor(sf::Color::White);
                        break;

                    case 1:
                        quad.setFillColor(sf::Color::Yellow);
                        break;

                    default:
                        quad.setFillColor(sf::Color::Magenta);
                }



                render_target.draw(quad);
            }
        }
    }
};

int main()
{
    sf::RenderWindow window({1600, 900}, "WalksoN", sf::Style::Default);
    window.setVerticalSyncEnabled(true);

    Entity player;
    player.position = {2.0f, 2.0f};

    PreviewRender preview_renderer;

    while (window.isOpen())
    {
        sf::Event e;

        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear();

        preview_renderer.render_preview(window, player);

        window.display();
    }
}