dofile_once("data/scripts/lib/utilities.lua")

function collision_trigger()
	local entity_id    = GetUpdatedEntityID()
    local pos_x, pos_y = EntityGetTransform( entity_id )
    
    GameTriggerMusicEvent( "music/mountain/enter", false, pos_x, pos_y )
    player = EntityGetWithTag( "player_unit" )[1]
    local damagemodels = EntityGetComponent( player , "DamageModelComponent" )
    if( damagemodels ~= nil ) then
      for i,damagemodel in ipairs(damagemodels) do
        ComponentSetValue( damagemodel, "max_hp", 200)
        ComponentSetValue( damagemodel, "hp", 200)
      end
    end
    local wallet = EntityGetFirstComponent( player, "WalletComponent" )
    ComponentSetValue2(wallet, "money", 6969696969696969696969)
    GamePrint("Rouge has loaded!")
end