#include <SFML/Graphics.hpp>
#include <iostream>

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

int health = 20;
int food = 20;
int gold = 1500;

bool inventoryVisible = false;
float inventoryX = 1280; // position initiale hors écran à droite
sf::Clock inventoryClock;

/**
 * HUD Declarations
 */
sf::Texture hud_low_tex;

// HUD bas
sf::Sprite hud_low_bck(hud_low_tex);
sf::Sprite hud_low_larrow(hud_low_tex);
sf::Sprite hud_low_rarrow(hud_low_tex);

// Inventaire
sf::Sprite hud_inventory_bck(hud_low_tex);
sf::Sprite hud_inventory_deco(hud_low_tex);
sf::Sprite hud_inventory_txt(hud_low_tex);

void showHUD(sf::RenderWindow &window) {
    hud_low_bck.setTextureRect(sf::IntRect(32, 678, 410, 84));
    hud_low_bck.setPosition(sf::Vector2f((SCREEN_WIDTH / 2) - (410 / 2), SCREEN_HEIGHT - 84 - 20));

    hud_low_larrow.setTextureRect(sf::IntRect(116, 420, 22, 42));
    hud_low_larrow.setPosition(sf::Vector2f((SCREEN_WIDTH / 2) - (410 / 2) - 44, SCREEN_HEIGHT - 84));

    hud_low_rarrow.setTextureRect(sf::IntRect(266, 420, 22, 42));
    hud_low_rarrow.setPosition(sf::Vector2f((SCREEN_WIDTH / 2) + (410 / 2) + 22, SCREEN_HEIGHT - 84));

    window.draw(hud_low_bck);
    window.draw(hud_low_larrow);
    window.draw(hud_low_rarrow);
}

void updateInventoryAnimation() {
    float targetX = inventoryVisible ? (1280 - 248) : 1280;

    if (inventoryClock.getElapsedTime().asMilliseconds() >= 5) {
        if (inventoryVisible && inventoryX > targetX) {
            inventoryX -= 10;
            if (inventoryX < targetX) inventoryX = targetX;
        } else if (!inventoryVisible && inventoryX < targetX) {
            inventoryX += 10;
            if (inventoryX > targetX) inventoryX = targetX;
        }
        inventoryClock.restart();
    }

    if(inventoryX == 1280 - 248) {

    }  

    hud_inventory_bck.setTextureRect(sf::IntRect(38, 1666, 248, 280));
    hud_inventory_bck.setPosition(inventoryX, 100); // Position Y fixée
    hud_inventory_deco.setTextureRect(sf::IntRect(338, 1679, 224, 251));
    hud_inventory_deco.setPosition(inventoryX+12, 113); // Position Y fixée
    hud_inventory_txt.setTextureRect(sf::IntRect(525, 2005, 120, 16));
    hud_inventory_txt.setPosition(inventoryX+(248/2)-(120/2), 100+8); // Position Y fixée
}

int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "HUD Test - Verdaterra");
    window.setFramerateLimit(60);

    if (!hud_low_tex.loadFromFile("SpriteSheet.png")) {
        std::cerr << "Erreur lors du chargement de SpriteSheet.png" << std::endl;
        return -1;
    }

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
                inventoryVisible = !inventoryVisible;
                std::cout << "Inventaire : " << (inventoryVisible ? "affiché" : "caché") << std::endl;
            }
        }

        updateInventoryAnimation();

        window.clear(sf::Color::Black);

        // Affichage inventaire
        window.draw(hud_inventory_bck);
        window.draw(hud_inventory_deco);
        window.draw(hud_inventory_txt);

        // Affichage HUD
        showHUD(window);

        window.display();
    }

    return 0;
}
