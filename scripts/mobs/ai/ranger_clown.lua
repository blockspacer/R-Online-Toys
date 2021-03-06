registerNpc(185, {
  walk_speed        = 175,
  run_speed         = 700,
  scale             = 95,
  r_weapon          = 203,
  l_weapon          = 0,
  level             = 48,
  hp                = 26,
  attack            = 200,
  hit               = 122,
  def               = 101,
  res               = 106,
  avoid             = 93,
  attack_spd        = 102,
  is_magic_damage   = 0,
  ai_type           = 321,
  give_exp          = 46,
  drop_type         = 194,
  drop_money        = 22,
  drop_item         = 53,
  union_number      = 53,
  need_summon_count = 0,
  sell_tab0         = 0,
  sell_tab1         = 0,
  sell_tab2         = 0,
  sell_tab3         = 0,
  can_target        = 0,
  attack_range      = 2500,
  npc_type          = 5,
  hit_material_type = 1,
  face_icon         = 0,
  summon_mob_type   = 0,
  quest_type        = 0,
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