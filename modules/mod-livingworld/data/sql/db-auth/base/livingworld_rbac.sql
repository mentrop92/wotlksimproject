DELETE FROM `rbac_linked_permissions` WHERE `linkedId` BETWEEN 1900 AND 1904;
DELETE FROM `rbac_permissions` WHERE `id` BETWEEN 1900 AND 1904;

INSERT INTO `rbac_permissions` (`id`, `name`) VALUES
  (1900, 'LivingWorld: view status'),
  (1901, 'LivingWorld: inspect profiles'),
  (1902, 'LivingWorld: control runtime state'),
  (1903, 'LivingWorld: control population policy'),
  (1904, 'LivingWorld: destructive world seeding');

-- GameMasters may inspect the simulation but cannot change global policy.
INSERT INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
  (197, 1900),
  (197, 1901);

-- Administrators receive operational controls. Destructive seeding remains
-- intentionally unlinked and must be granted explicitly per account later.
INSERT INTO `rbac_linked_permissions` (`id`, `linkedId`) VALUES
  (196, 1900),
  (196, 1901),
  (196, 1902),
  (196, 1903);
