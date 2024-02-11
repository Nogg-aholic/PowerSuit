SML3.7/server support. Bugfixes.




This update brought to you by Robb.
If you enjoy my work, please consider my [completely optional tip jar](https://ko-fi.com/robb4).

## Notes for the Future

Be sure to check the [PowerSuit Modules patch notes](https://ficsit.app/mod/ArmorModules) to hear about my plans regarding Satisfactory 1.0.

## New Stuff

- Added a note to the mod configs to remind users that keybinds are configured in the base game's screens

## Changed Stuff

- Removed the now unused "Enable 3rd person camera" config option
  - It's always on by default now
  - Thanks jhosenpai for bringing this to my attention!

## Bugfixes

- Fixed that equipment would be detached from the player's hands _after unequipping the suit_ with non-standard FOVs. Note that the issue still persists while wearing the suit, see Known Bugs below
  - Thanks Th3Fanbus for fixing this!

## Known Bugs

- [Wearing a suit causes handheld equipment to visually detach from your hands when using non-standard FOVs](https://github.com/budak7273/ArmorModules/issues/40)
- Multiplayer clients that logged in wearing a suit may need to re-equip it upon joining
- Entering a vehicle while flying can very rarely and inconsistently kill your player without leaving a death crate
- Multiplayer clients don't get fuel fuse recovery sounds
