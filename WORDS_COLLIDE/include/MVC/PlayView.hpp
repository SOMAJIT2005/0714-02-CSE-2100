#pragma once
#include "MVC/PlayModel.hpp"
#include "Systems/Renderer.hpp"
#include "Systems/Audio.hpp"
#include "Core/Config.hpp"      
#include "UI/Button.hpp"
#include <memory>

class PlayView {
private:
    PlayModel* model;
    Renderer& renderer;
    Audio& audio;
    AppContext& ctx;

    std::shared_ptr<Button> giveUpBtn;

public:
    PlayView(PlayModel* m, Renderer& r, Audio& a, AppContext& c);

    void initUI();
    void draw();

    std::shared_ptr<Button> getGiveUpButton() const;
};