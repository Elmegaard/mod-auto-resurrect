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
    AutoResurrect()
        : PlayerScript("AutoResurrect", {
            PLAYERHOOK_ON_LOGIN,
            PLAYERHOOK_ON_PLAYER_RELEASED_GHOST,
            PLAYERHOOK_ON_MAP_CHANGED,
        }) { }
    std::map<uint32, const AreaTriggerTeleport*> playerLocation = {};

    void DebugLog(Player* player, std::string log) {
        if (sConfigMgr->GetOption<bool>("AutoResurrect.Enable", false) && sConfigMgr->GetOption<bool>("AutoResurrect.Notification", false)) {
            ChatHandler(player->GetSession()).SendSysMessage(log);
        }
    }

    void OnPlayerLogin(Player* player) override
    {
        if (sConfigMgr->GetOption<bool>("AutoResurrect.Enable", false) && sConfigMgr->GetOption<bool>("AutoResurrect.Notification", false))
        {
            DebugLog(player, "Auto Resurrect module enabled");
        }
    }

    void OnPlayerReleasedGhost(Player* player) override {
        if (!sConfigMgr->GetOption<bool>("AutoResurrect.Enable", false)) {
            return;
        }

        Map* map = player->GetMap();

        bool dungeons = sConfigMgr->GetOption<bool>("AutoResurrect.Dungeon", false);
        bool heroics = sConfigMgr->GetOption<bool>("AutoResurrect.HeroicDungeon", false);
        bool raids = sConfigMgr->GetOption<bool>("AutoResurrect.Raid", false);
        bool heroicRaids = sConfigMgr->GetOption<bool>("AutoResurrect.HeroicRaid", false);

        if ((dungeons && !map->IsHeroic() && map->IsDungeon()) ||
            (heroics && map->IsHeroic() && map->IsDungeon()) ||
            (raids && !map->IsHeroic() && map->IsRaid()) ||
            (heroicRaids && map->IsHeroic() && map->IsRaid())) {
            AreaTriggerTeleport const* at = sObjectMgr->GetMapEntranceTrigger(player->GetMapId());
            playerLocation[player->GetGUID().GetCounter()] = at;
        }
    }

    virtual void OnPlayerMapChanged(Player* player) override {
        if (!sConfigMgr->GetOption<bool>("AutoResurrect.Enable", false)) {
            return;
        }

        if (!playerLocation.count(player->GetGUID().GetCounter())) {
            return;
        }

        AreaTriggerTeleport const* at = playerLocation[player->GetGUID().GetCounter()];
        if (at == NULL) {
            return;
        }

        player->ResurrectPlayer(1.0f);
        player->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation);
        player->SaveToDB(false, false);

        playerLocation[player->GetGUID().GetCounter()] = NULL;
    }
};

// Add all scripts in one
void AddAutoResurrectScripts()
{
    new AutoResurrect();
}
