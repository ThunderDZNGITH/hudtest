#include <SFML/Graphics.hpp>
#include <iostream>

int SCREEN_WIDTH = 1280;
int SCREEN_HEIGHT = 720;

int health = 20;
int food = 20;
int gold = 1500;

bool inventoryVisible = false;
float inventoryX = 1280;
sf::Clock inventoryClock;

sf::Texture hud_low_tex;
sf::Font inventoryFont;
sf::Text selectedItemNameText;

sf::Sprite hud_low_bck(hud_low_tex);
sf::Sprite hud_low_larrow(hud_low_tex);
sf::Sprite hud_low_rarrow(hud_low_tex);
sf::Sprite hud_low_slots[5];
sf::Sprite hud_low_slot_items[5];

sf::Sprite hud_inventory_bck(hud_low_tex);
sf::Sprite hud_inventory_deco(hud_low_tex);
sf::Sprite hud_inventory_txt(hud_low_tex);

const int INVENTORY_COLS = 5;
const int INVENTORY_ROWS = 5;

struct Item {
    int id;
    std::string name;
    sf::IntRect textureRect;
};

struct inventorySlot {
    sf::Sprite backgroundSprite;
    sf::Sprite itemSprite;
    int x, y;
    int id;
    int count;
    std::string name;
    sf::Text countText;
};

inventorySlot inventorySlots[INVENTORY_ROWS][INVENTORY_COLS];
inventorySlot hudSlots[5];

int selectedSlotRow = 0;
int selectedSlotCol = 0;
int selectedSlotIndex = 0;

sf::Sprite selector;

sf::Sprite hud_item_info_bck(hud_low_tex); // Panneau d'information de l'item sélectionné
float itemInfoX = 1280; // Position horizontale de l'élément info, initialement hors de l'écran.
sf::Text selectedItemNameTextInfo;

void initInventorySlots() {
    int slotSize = 36;
    int padding = 4;
    int startX = 27;
    int startY = 40;

    for (int row = 0; row < INVENTORY_ROWS; ++row) {
        for (int col = 0; col < INVENTORY_COLS; ++col) {
            inventorySlot &slot = inventorySlots[row][col];
            slot.x = startX + col * (slotSize + padding);
            slot.y = startY + row * (slotSize + padding);
            slot.id = -1;
            slot.count = 0;
            slot.name = "";

            slot.backgroundSprite.setTexture(hud_low_tex);
            slot.backgroundSprite.setTextureRect(sf::IntRect(103, 550, 34, 36));

            slot.itemSprite.setTexture(hud_low_tex);
            slot.itemSprite.setTextureRect(sf::IntRect(0, 0, 0, 0));

            slot.countText.setFont(inventoryFont);
            slot.countText.setCharacterSize(12);
            slot.countText.setFillColor(sf::Color::White);
            slot.countText.setStyle(sf::Text::Bold);
        }
    }
}

void initHudLowSlots() {
    int slotSize = 36;
    int padding = 4;
    int startX = 450;
    int startY = SCREEN_HEIGHT - 80;

    for (int i = 0; i < 5; ++i) {
        hud_low_slots[i].setTexture(hud_low_tex);
        hud_low_slots[i].setTextureRect(sf::IntRect(103, 550, 34, 36));
        hud_low_slots[i].setPosition(startX + i * (slotSize + padding), startY);

        hud_low_slot_items[i].setTexture(hud_low_tex);
        hud_low_slot_items[i].setTextureRect(sf::IntRect(0, 0, 0, 0));
        hud_low_slot_items[i].setPosition(startX + i * (slotSize + padding), startY);

        hudSlots[i].x = 0;
        hudSlots[i].y = 0;
        hudSlots[i].id = -1;
        hudSlots[i].count = 0;
        hudSlots[i].name = "";

        hudSlots[i].itemSprite.setTexture(hud_low_tex);
        hudSlots[i].itemSprite.setTextureRect(sf::IntRect(0, 0, 0, 0));

        hudSlots[i].countText.setFont(inventoryFont);
        hudSlots[i].countText.setCharacterSize(12);
        hudSlots[i].countText.setFillColor(sf::Color::White);
        hudSlots[i].countText.setStyle(sf::Text::Bold);
    }
}

bool addItemToSelectedSlot(const Item &item) {
    inventorySlot *slot = inventoryVisible ? &inventorySlots[selectedSlotRow][selectedSlotCol] : &hudSlots[selectedSlotIndex];

    if (slot->id == item.id && slot->count < 16) {
        slot->count++;
        slot->countText.setString("x" + std::to_string(slot->count));
        return true;
    }

    if (slot->id == -1) {
        slot->id = item.id;
        slot->name = item.name;
        slot->count = 1;
        slot->itemSprite.setTextureRect(item.textureRect);
        slot->itemSprite.setTexture(hud_low_tex);
        slot->countText.setString("x1");
        return true;
    }

    return false;
}

bool addItemToSpecifiedSlot(const Item &item, int row, int col, bool isInventory) {
    inventorySlot *slot;

    if (isInventory) {
        if (row < 0 || row >= INVENTORY_ROWS || col < 0 || col >= INVENTORY_COLS)
            return false;
        slot = &inventorySlots[row][col];
    } else {
        if (col < 0 || col >= 5)
            return false;
        slot = &hudSlots[col];
    }

    if (slot->id == item.id && slot->count < 16) {
        slot->count++;
        slot->countText.setString("x" + std::to_string(slot->count));
        return true;
    }

    if (slot->id == -1) {
        slot->id = item.id;
        slot->name = item.name;
        slot->count = 1;
        slot->itemSprite.setTextureRect(item.textureRect);
        slot->itemSprite.setTexture(hud_low_tex);
        slot->countText.setString("x1");
        return true;
    }

    return false;
}

bool removeItemFromSelectedSlot() {
    inventorySlot *slot = inventoryVisible ? &inventorySlots[selectedSlotRow][selectedSlotCol] : &hudSlots[selectedSlotIndex];

    if (slot->count > 0) {
        slot->count--;
        if (slot->count > 0) {
            slot->countText.setString("x" + std::to_string(slot->count));
        } else {
            slot->id = -1;
            slot->name = "";
            slot->itemSprite.setTextureRect(sf::IntRect(0, 0, 0, 0));
            slot->countText.setString("");
        }
        return true;
    }

    return false;
}

bool removeItemFromSpecifiedSlot(int row, int col, bool isInventory) {
    inventorySlot *slot;

    if (isInventory) {
        if (row < 0 || row >= INVENTORY_ROWS || col < 0 || col >= INVENTORY_COLS)
            return false;
        slot = &inventorySlots[row][col];
    } else {
        if (col < 0 || col >= 5)
            return false;
        slot = &hudSlots[col];
    }

    if (slot->count > 0) {
        slot->count--;
        if (slot->count > 0) {
            slot->countText.setString("x" + std::to_string(slot->count));
        } else {
            slot->id = -1;
            slot->name = "";
            slot->itemSprite.setTextureRect(sf::IntRect(0, 0, 0, 0));
            slot->countText.setString("");
        }
        return true;
    }

    return false;
}

void showHUD(sf::RenderWindow &window) {
    hud_low_bck.setTextureRect(sf::IntRect(32, 678, 410, 84));
    hud_low_bck.setPosition((SCREEN_WIDTH / 2) - (410 / 2), SCREEN_HEIGHT - 84 - 20);

    hud_low_larrow.setTextureRect(sf::IntRect(116, 420, 22, 42));
    hud_low_larrow.setPosition((SCREEN_WIDTH / 2) - (410 / 2) - 44, SCREEN_HEIGHT - 84);

    hud_low_rarrow.setTextureRect(sf::IntRect(266, 420, 22, 42));
    hud_low_rarrow.setPosition((SCREEN_WIDTH / 2) + (410 / 2) + 22, SCREEN_HEIGHT - 84);

    window.draw(hud_low_bck);
    window.draw(hud_low_larrow);
    window.draw(hud_low_rarrow);

    for (int i = 0; i < 5; ++i) {
        window.draw(hud_low_slots[i]);
        window.draw(hud_low_slot_items[i]);

        if (hudSlots[i].id != -1) {
            hudSlots[i].itemSprite.setPosition(hud_low_slots[i].getPosition().x + 4, hud_low_slots[i].getPosition().y + 4);
            window.draw(hudSlots[i].itemSprite);

            hudSlots[i].countText.setPosition(hud_low_slots[i].getPosition().x + 4, hud_low_slots[i].getPosition().y + 20);
            hudSlots[i].countText.setFillColor(sf::Color::Black); 
            hudSlots[i].countText.setCharacterSize(16);
            window.draw(hudSlots[i].countText);
        }
    }

    if (inventoryVisible) {
        float slotX = inventoryX + inventorySlots[selectedSlotRow][selectedSlotCol].x;
        float slotY = 100 + inventorySlots[selectedSlotRow][selectedSlotCol].y;
        selector.setPosition(slotX - 3, slotY - 3);
        window.draw(selector);
    } else {
        float slotX = 450 + selectedSlotIndex * (36 + 4);
        float slotY = SCREEN_HEIGHT - 80;
        selector.setPosition(slotX - 3, slotY - 3);
        window.draw(selector);
    }
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

void updateKeyboardNavigation(sf::Event event) {
    if (event.type == sf::Event::KeyPressed) {
        if (inventoryVisible) {
            // Navigation dans l'inventaire
            if (event.key.code == sf::Keyboard::Right) {
                selectedSlotCol = (selectedSlotCol + 1) % INVENTORY_COLS;
                if (selectedSlotCol == 0) selectedSlotRow = (selectedSlotRow + 1) % INVENTORY_ROWS;
            } else if (event.key.code == sf::Keyboard::Left) {
                selectedSlotCol = (selectedSlotCol - 1 + INVENTORY_COLS) % INVENTORY_COLS;
                if (selectedSlotCol == INVENTORY_COLS - 1) selectedSlotRow = (selectedSlotRow - 1 + INVENTORY_ROWS) % INVENTORY_ROWS;
            } else if (event.key.code == sf::Keyboard::Down) {
                selectedSlotRow = (selectedSlotRow + 1) % INVENTORY_ROWS;
            } else if (event.key.code == sf::Keyboard::Up) {
                selectedSlotRow = (selectedSlotRow - 1 + INVENTORY_ROWS) % INVENTORY_ROWS;
            }

            // Si un item est sélectionné dans l'inventaire, afficher ses informations
            if (inventorySlots[selectedSlotRow][selectedSlotCol].id != -1) {
                itemInfoX = 1280 - 340; // Dérouler le panneau d'information
            } else {
                itemInfoX = 1280; // Réduire le panneau quand l'item est vide
            }

        } else {
            // Navigation dans la hotbar
            if (event.key.code == sf::Keyboard::Right) {
                selectedSlotIndex = (selectedSlotIndex + 1) % 5; // Vous pouvez ajuster cela selon le nombre de slots
            } else if (event.key.code == sf::Keyboard::Left) {
                selectedSlotIndex = (selectedSlotIndex - 1 + 5) % 5;
            }
        }
    }
}



void drawSelectedItemName(sf::RenderWindow &window) {
    std::string nameToDisplay = "vide";  // Valeur par défaut

    if (inventoryVisible) {
        // Affichage dans l'inventaire
        inventorySlot &slot = inventorySlots[selectedSlotRow][selectedSlotCol];
        if (slot.id != -1 && slot.count > 0) {
            nameToDisplay = slot.name;
        }
    } else {
        // Affichage dans la hotbar
        inventorySlot &slot = hudSlots[selectedSlotIndex];  // Utilisation de selectedSlotIndex pour la hotbar
        if (slot.id != -1 && slot.count > 0) {
            nameToDisplay = slot.name;
        }
    }

    selectedItemNameText.setString(nameToDisplay);
    float textWidth = selectedItemNameText.getLocalBounds().width;
    selectedItemNameText.setPosition((SCREEN_WIDTH / 2.0f) - (textWidth / 2.0f), SCREEN_HEIGHT - 130);
    window.draw(selectedItemNameText);
}



void displayItemInfo(sf::RenderWindow &window) {
    hud_item_info_bck.setTextureRect(sf::IntRect(368, 32, 81, 90)); // Définir les coordonnées du sprite pour l'info de l'item
    hud_item_info_bck.setPosition(itemInfoX, 100); // Position de l'info de l'item
    window.draw(hud_item_info_bck);
        if (selectedSlotRow >= 0 && selectedSlotCol >= 0) {
            inventorySlot &slot = inventorySlots[selectedSlotRow][selectedSlotCol];
    
            if (slot.id != -1 && slot.count > 0) {
                // Affichage du nom de l'item sélectionné dans l'info
                selectedItemNameTextInfo.setFont(inventoryFont);
                selectedItemNameTextInfo.setString(slot.name);
                selectedItemNameTextInfo.setCharacterSize(25);
                selectedItemNameTextInfo.setPosition(itemInfoX + 10, 120);
                selectedItemNameTextInfo.setFillColor(sf::Color::Black);
                window.draw(selectedItemNameTextInfo);
    
                // Affichage de la quantité
                sf::Text countText("Quantité: x" + std::to_string(slot.count), inventoryFont, 14);
                countText.setPosition(itemInfoX + 10, 140);
                window.draw(countText);
            }
        }
}



int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "HUD Test - Verdaterra");
    window.setFramerateLimit(60);

    if (!hud_low_tex.loadFromFile("SpriteSheet.png")) {
        std::cerr << "Erreur lors du chargement de SpriteSheet.png" << std::endl;
        return -1;
    }

    if (!inventoryFont.loadFromFile("arial.ttf")) {
        std::cerr << "Erreur lors du chargement de la police arial.ttf" << std::endl;
        return -1;
    }

    selectedItemNameText.setFont(inventoryFont);
    selectedItemNameText.setCharacterSize(18);
    selectedItemNameText.setFillColor(sf::Color::White);
    selectedItemNameText.setStyle(sf::Text::Bold);

    initInventorySlots();
    initHudLowSlots();

    selector.setTexture(hud_low_tex);
    selector.setTextureRect(sf::IntRect(627, 211, 40, 43));

    Item potion;
    potion.name = "Potion";
    potion.id = 1;
    potion.textureRect = sf::IntRect(496, 177, 16, 16);
    Item book;
    book.name = "Book";
    book.id = 2;
    book.textureRect = sf::IntRect(496, 145, 16, 16);
    addItemToSpecifiedSlot(potion, 5, 0, false);

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::E) {
                inventoryVisible = !inventoryVisible;
                if(!inventoryVisible){
                    itemInfoX = 1280;
                } 
                std::cout << "Inventaire : " << (inventoryVisible ? "affiché" : "caché") << std::endl;
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
                if (addItemToSelectedSlot(book)) {
                    std::cout << "Item ajouté à la case sélectionnée." << std::endl;
                } else {
                    std::cout << "Impossible d'ajouter l'item." << std::endl;
                }
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z) {
                if (removeItemFromSelectedSlot()) {
                    std::cout << "Item retiré de la case sélectionnée." << std::endl;
                } else {
                    std::cout << "Aucun item à retirer." << std::endl;
                }
            }

            updateKeyboardNavigation(event);
        }

        updateInventoryAnimation();

        window.clear(sf::Color::Black);
        window.draw(hud_inventory_bck);
        window.draw(hud_inventory_deco);
        window.draw(hud_inventory_txt);

        for (int row = 0; row < INVENTORY_ROWS; ++row) {
            for (int col = 0; col < INVENTORY_COLS; ++col) {
                inventorySlot &slot = inventorySlots[row][col];
                float slotX = inventoryX + slot.x;
                float slotY = 100 + slot.y;

                slot.backgroundSprite.setPosition(slotX, slotY);
                window.draw(slot.backgroundSprite);

                if (slot.id != -1) {
                    slot.itemSprite.setPosition(slotX + 4, slotY + 4);
                    window.draw(slot.itemSprite);
                }

                slot.countText.setPosition(slotX + 4, slotY + 20);
                slot.countText.setFillColor(sf::Color::Black);
                window.draw(slot.countText);
            }
        }
        displayItemInfo(window);

        showHUD(window);
        drawSelectedItemName(window);

        window.display();
    }

    return 0;
}
