registerNpc(402, {
  walk_speed        = 0,
  run_speed         = 260,
  scale             = 670,
  r_weapon          = 330,
  l_weapon          = 138,
  level             = 0,
  hp                = 66,
  attack            = 211,
  hit               = 352,
  def               = 223,
  res               = 241,
  avoid             = 147,
  attack_spd        = 103,
  is_magic_damage   = 110,
  ai_type           = 0,
  give_exp          = 72,
  drop_type         = 637,
  drop_money        = 302,
  drop_item         = 1,
  union_number      = 1,
  need_summon_count = 90,
  sell_tab0         = 90,
  sell_tab1         = 0,
  sell_tab2         = 0,
  sell_tab3         = 0,
  can_target        = 0,
  attack_range      = 0,
  npc_type          = 350,
  hit_material_type = 10,
  face_icon         = 0,
  summon_mob_type   = 0,
  quest_type        = 19,
  height            = 0
});
function OnInit(entity)
  return true
end

function OnCreate(entity)
  return true
end

function OnDelete(entity)
  return true
end

function OnDead(entity)
end

function OnDamaged(entity)
end