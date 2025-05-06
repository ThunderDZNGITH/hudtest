#include <SFML/Graphics.hpp>
#include <iostream>

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

int health = 20;
int food = 20;
int gold = 1500;

bool inventoryVisible = false;
float inventoryX = 1280; // position initiale hors écran
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

// Inventory Slots
const int INVENTORY_COLS = 5;
const int INVENTORY_ROWS = 5;

struct Item {
    int id;
    sf::IntRect textureRect;
};

struct inventorySlot {
    sf::Sprite backgroundSprite; // slot vide
    sf::Sprite itemSprite;       // item par-dessus
    int x, y;
    int id;
};

inventorySlot inventorySlots[INVENTORY_ROWS][INVENTORY_COLS];

void initInventorySlots() {
    int slotSize = 36; // taille d’un slot
    int padding = 4;   // espacement
    int startX = 27;   // offset dans l'inventaire
    int startY = 40;

    for (int row = 0; row < INVENTORY_ROWS; ++row) {
        for (int col = 0; col < INVENTORY_COLS; ++col) {
            inventorySlot &slot = inventorySlots[row][col];
            slot.x = startX + col * (slotSize + padding);
            slot.y = startY + row * (slotSize + padding);
            slot.id = -1; // slot vide

            slot.backgroundSprite.setTexture(hud_low_tex);
            slot.backgroundSprite.setTextureRect(sf::IntRect(103, 550, 34, 36)); // Texture d’un slot vide

            slot.itemSprite.setTexture(hud_low_tex);
            slot.itemSprite.setTextureRect(sf::IntRect(0, 0, 0, 0)); // Vide par défaut
        }
    }
}

bool addItemToInventory(const Item &item) {
    for (int row = 0; row < INVENTORY_ROWS; ++row) {
        for (int col = 0; col < INVENTORY_COLS; ++col) {
            inventorySlot &slot = inventorySlots[row][col];
            if (slot.id == -1) { // Slot vide
                slot.id = item.id;
                slot.itemSprite.setTextureRect(item.textureRect);
                slot.itemSprite.setTexture(hud_low_tex);
                return true;
            }
        }
    }
    return false; // Pas de place
}

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

    hud_inventory_bck.setTextureRect(sf::IntRect(38, 1666, 248, 280));
    hud_inventory_bck.setPosition(inventoryX, 100);

    hud_inventory_deco.setTextureRect(sf::IntRect(338, 1679, 224, 251));
    hud_inventory_deco.setPosition(inventoryX + 12, 113);

    hud_inventory_txt.setTextureRect(sf::IntRect(525, 2005, 120, 16));
    hud_inventory_txt.setPosition(inventoryX + (248 / 2) - (120 / 2), 100 + 8);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "HUD Test - Verdaterra");
    window.setFramerateLimit(60);

    if (!hud_low_tex.loadFromFile("SpriteSheet.png")) {
        std::cerr << "Erreur lors du chargement de SpriteSheet.png" << std::endl;
        return -1;
    }

    initInventorySlots();

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
                inventoryVisible = !inventoryVisible;
                std::cout << "Inventaire : " << (inventoryVisible ? "affiché" : "caché") << std::endl;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
                // Exemple d'ajout d'un item
                Item potion;
                potion.id = 1;
                potion.textureRect = sf::IntRect(496, 177, 16, 16); // Exemple de sprite item

                if (addItemToInventory(potion)) {
                    std::cout << "Item ajouté à l'inventaire." << std::endl;
                } else {
                    std::cout << "Inventaire plein." << std::endl;
                }
            }
        }

        updateInventoryAnimation();

        window.clear(sf::Color::Black);

        // Affichage inventaire
        window.draw(hud_inventory_bck);
        window.draw(hud_inventory_deco);
        window.draw(hud_inventory_txt);

        // Affichage des slots
        for (int row = 0; row < INVENTORY_ROWS; ++row) {
            for (int col = 0; col < INVENTORY_COLS; ++col) {
                inventorySlot &slot = inventorySlots[row][col];
                float slotX = inventoryX + slot.x;
                float slotY = 100 + slot.y;

                // Dessiner d’abord le fond
                slot.backgroundSprite.setPosition(slotX, slotY);
                window.draw(slot.backgroundSprite);

                // Puis l’item s’il existe
                if (slot.id != -1) {
                    slot.itemSprite.setPosition(slotX+10, slotY+11);
                    window.draw(slot.itemSprite);
                }
            }
        }

        // Affichage HUD
        showHUD(window);

        window.display();
    }

    return 0;
}
