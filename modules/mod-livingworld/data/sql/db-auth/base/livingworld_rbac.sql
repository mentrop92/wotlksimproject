INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
  (19000, 'LivingWorld: view status'),
  (19001, 'LivingWorld: inspect profiles'),
  (19002, 'LivingWorld: control runtime state'),
  (19003, 'LivingWorld: control population policy'),
  (19004, 'LivingWorld: destructive world seeding')
ON DUPLICATE KEY UPDATE `name` = VALUES(`name`);

-- GameMasters may inspect the simulation but cannot change global policy.
INSERT IGNORE INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
  (197, 19000),
  (197, 19001);

-- Administrators receive operational controls. Destructive seeding remains
-- intentionally unlinked and must be granted explicitly per account later.
INSERT IGNORE INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
  (196, 19000),
  (196, 19001),
  (196, 19002),
  (196, 19003);
