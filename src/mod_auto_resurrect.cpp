/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"

// Add player scripts
class AutoResurrect : public PlayerScript
{
public:
    AutoResurrect() : PlayerScript("AutoResurrect") { }

    void OnLogin(Player* player) override
    {
        if (sConfigMgr->GetOption<bool>("MyModule.Enable", false))
        {
            ChatHandler(player->GetSession()).SendSysMessage("Hello World from Skeleton-Module!");
        }
    }
};

// Add all scripts in one
void AddAutoResurrectScripts()
{
    new AutoResurrect();
}
