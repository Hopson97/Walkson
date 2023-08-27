#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include <array>
#include <cmath>
#include <iostream>

#include "Entity.h"
#include "Map.h"
#include "MapPreviewRenderer.h"
#include "Maths.h"

int main()
{
    sf::RenderWindow window({1280, 720}, "WalksoN", sf::Style::Default);
    window.setVerticalSyncEnabled(true);

    Entity player;
    player.position = {MAP_X_SIZE / 2, MAP_Y_SIZE / 2};

    Map map;
    PreviewRender preview_renderer(map);

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

        player.direction = clamp_angle(player.direction);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            player.position.x += std::sin(rads(player.direction)) * 0.1f;
            player.position.y += std::cos(rads(player.direction)) * 0.1f;
            // std::printf("dx: %.2f dy: %.2f\n", std::sin(player.direction),
            //             std::cos(player.direction));
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            player.direction += 5.0f;
        }

        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            player.direction -= 5.0f;
        }

        window.clear();

        preview_renderer.render_preview(window, player);

        float ray_direction = clamp_angle(player.direction - FIELD_OF_VIEW / 2);
        for (int x = 0; x < window.getSize().x; x++)
        {
            // int gridX =
        }

        window.display();
    }
}