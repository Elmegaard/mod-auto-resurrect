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
        if (sConfigMgr->GetOption<bool>("AutoResurrect.Enable", false) && sConfigMgr->GetOption<bool>("AutoResurrect.Notification", false))
        {
            ChatHandler(player->GetSession()).SendSysMessage("Auto Resurrect module enabled");
        }
    }

    void OnPlayerReleasedGhost(Player* player) override {
        if (!sConfigMgr->GetOption<bool>("AutoResurrect.Enable", false))
        {
            return;
        }

        bool dungeons = sConfigMgr->GetOption<bool>("AutoResurrect.Dungeon", false);
        bool heroics = sConfigMgr->GetOption<bool>("AutoResurrect.HeroicDungeon", false);
        bool raids = sConfigMgr->GetOption<bool>("AutoResurrect.Raid", false);
        bool heroicRaids = sConfigMgr->GetOption<bool>("AutoResurrect.HeroicRaid", false);

        if ((dungeons && !player->GetMap()->IsHeroic() && player->GetMap()->IsDungeon()) ||
            (heroics && player->GetMap()->IsHeroic() && player->GetMap()->IsDungeon()) ||
            (raids && !player->GetMap()->IsHeroic() && player->GetMap()->IsRaid()) ||
            (heroicRaids && player->GetMap()->IsHeroic() && player->GetMap()->IsRaid())) {
            AreaTriggerTeleport const* at = sObjectMgr->GetMapEntranceTrigger(player->GetMapId());

            player->ResurrectPlayer(1.0f);
            player->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation);
        }
    }
};

// Add all scripts in one
void AddAutoResurrectScripts()
{
    new AutoResurrect();
}
