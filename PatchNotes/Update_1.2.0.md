Recompile for Update 7 (CL208572) support.




This update brought to you by Robb#6731.

Be sure to read the 1.0.0 changelog - lots of stuff changed!

## New Features

- Suit text stats summary (visible in Stats and suit popup, and with advanced module stats tooltips enabled) can now display Advanced Flags
  - Mostly useful for debugging

## Changed Stuff

- Made the background color of the Fuel Fuse Reboot bar invisible so it doesn't leave a dark line in the hud

## Bugfixes

- Fixed oversight that made the "open suit UI" hotkey instead open your player inventory until you had opened the suit UI at least once via the in-inventory tab
- Fixed that module attachment fuel consumption/generation calculation was entirely skipped while the fuel fuse was blown (meaning only the base refuel code or custom implementations could modify fuel levels)

## Known Bugs

- Multiplayer clients that logged in wearing a suit may need to re-equip it upon joining
- Entering a vehicle while flying can very rarely and inconsistently kill your player without leaving a death crate
- Multiplayer clients don't get fuel fuse recovery sounds
