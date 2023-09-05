Update 8 support.




This update brought to you by Robb#6731.

## New Stuff

- Integration for suit fuel preferences using the system introduced by U8 for use in the jetpack.
  The mod adding the suit must implement custom refueling logic that makes use of this field.
- New advanced suit flag - Allow Exit Pipes Early. See the Bugfixes section for more info.

## Changed Stuff

- Numerous internal systems for input handling reworked to use the new UE5 input system.
- The Toggle 3rd Person and Show Mouse Cursor keybinds now work even when you aren't wearing a suit

## Bugfixes

- The "Instant Start Flight Keybind" used to allow you to exit Hypertubes if pressed during travel. This is no longer the case unless you have the Allow Exit Pipes Early advanced suit flag.
- Upon restart, automatically fixes the base-game bug that causes your selected language to not format commas and decimals correctly. Note that you will have to re-launch if you change your selected language during gameplay for the auto-fix to take effect again.
- Adjusted how the "transfer item" library call works to fix issues with mods that modify stack sizes
  - This has not been tested extensively, so please report any issues you encounter with it. They would appear when using the Logistic Modules mod.

## Known Bugs

- Multiplayer clients that logged in wearing a suit may need to re-equip it upon joining
- Entering a vehicle while flying can very rarely and inconsistently kill your player without leaving a death crate
- Multiplayer clients don't get fuel fuse recovery sounds
