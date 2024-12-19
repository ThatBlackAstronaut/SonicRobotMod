#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/loader/SettingV3.hpp>

using namespace geode::prelude;

class $modify(MyPauseLayer, PauseLayer) {
    void openModSettings(CCObject*) {
        // Open the mod settings popup
        geode::openSettingsPopup(Mod::get());
    }

    void customSetup() {
        // Call the original `customSetup` to ensure proper initialization
        PauseLayer::customSetup();

        // Get the window size and existing button menu
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto menu = this->getChildByID("left-button-menu");

        // If the menu is not found, create a new one
        if (!menu) {
            menu = CCMenu::create();
            menu->setLayout(
                ColumnLayout::create()
                    ->setGap(4.f)
                    ->setAxisAlignment(AxisAlignment::End)
                    ->setAxisReverse(true)
                    ->setCrossAxisOverflow(false)
            );
            menu->setPosition({36.f, winSize.height / 2});
            menu->setContentSize({40, winSize.height - 40.f});
            menu->setZOrder(10);
            this->addChild(menu);
        }

        // Create the settings button
        auto spr = CCSprite::create("modSettings.png"_spr); // Replace with your button's sprite
        auto btn = CCMenuItemSpriteExtra::create(
            spr,
            this,
            menu_selector(MyPauseLayer::openModSettings)
        );
        spr->setScale(0.7f);

        // Add the button to the menu and update layout
        btn->setPosition({menu->getContentSize().width / 2, btn->getContentSize().height / 2});
        btn->setID("sonic-robot-settings-shortcut"_spr);
        menu->addChild(btn);
        menu->updateLayout();
    }
};
