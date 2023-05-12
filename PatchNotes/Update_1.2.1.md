Recompile for Update 7 (CL211116) support, since one the player reach extending utility functions caused crashes when called.




This update brought to you by Robb#6731.

## Changed Stuff

- Made some more UI strings localizable in prep for SML3.5's localization system
  - As of release of this update, Crowdin hasn't been updated with this yet
  - The "movement states" that appear in the suit hud have more verbose names now
  - Thanks Warzonefury for bringing these to my attention

## Known Bugs

- Multiplayer clients that logged in wearing a suit may need to re-equip it upon joining
- Entering a vehicle while flying can very rarely and inconsistently kill your player without leaving a death crate
- Multiplayer clients don't get fuel fuse recovery sounds
