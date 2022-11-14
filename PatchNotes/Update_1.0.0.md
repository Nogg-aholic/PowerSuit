Update 6 Support. UI Improvements and bugfixes. Configurable suit HUD position. New 'Fuel Recovery Period' mechanic.




This update brought to you by Robb#6731.

## New Stuff

- Update 6 support
- Suit HUD overlay can now be adjusted from the mod config screen
  - Position, scale, and transparency
- Suit HUD 'extra info' overlay can now be adjusted from the mod config screen
  - Also tied to the position of the Suit HUD
  - Position, scale, and transparency
- You can configure the position of the base game's "Item Pickup Notification" from the mod config screen
- Modules are now told what damage type, and damage causer, incoming damage is so they can decide what to do further based on that
- **Fuel Recovery Period** mechanic introduced
  - Introduced to fix bugs relating to fuel-consuming modules continuing to function despite the suit being out of fuel
  - Similar to your suit fuse shorting, but not as severe
  - If you entirely drain your suit of Suit Fuel and a module still wants to use fuel, the suit will enter a 5-second fuel recovery period to allow suit functions to refill the fuel tanks
  - During this time, all fuel consuming modules will cease to function, a (mod options configurable) sound will play at the start of the timeframe, and the suit UI will indicate the lack of fuel through various
    - Hoverpack controllers have blinking out of fuel indicators
    - Notice above the suit item slots in the inventory screen
    - Suit HUD blinking out of fuel icon, gray progress bar indicates how much time remains in the recovery period
    - Suit HUD fuse indicator turns yellow
  - Once fuel is obtained again, the fuel-starved modules will resume functioning again
  - If you don't obtain more fuel, the warnings will continue displaying until fuel is obtained
  - Should not affect suits that aren't consuming fuel

## Changed Stuff

- With the addition of multiple body slots to the base-game, **all PowerSuits are now a Back slot item** due to extending AFGHoverPack
  - This may change in a future version of the mod, but don't expect anything soon
  - You can safely wear a Suit in combination with any non-back-slot items
- Relocated the internal inventory slots to be further down and smaller (now that body slots take up more space)
- Renamed the Suit displays' tabs, and added a new one
  - Suit 'Modules' tab renamed to 'Overview'
  - Suit 'Stats' tab keeps its name
  - Suit 'Config' tab renamed to 'Extended'
  - New tab - 'Config' only visible in the Suit Popup (not enough space in the inventory screen side panel for it :( )
- Suit Popup Window improvements
  - You can now configure the mod configuration options for PowerSuit and any sub-mods directly from the PowerSuit Popup (right alt by default)
  - Now broken up into 2 scrollable regions
  - The installed modules region and top tabs are always in view (no longer scrolling)
  - Debug stats section is correctly expandable now
  - Prevent module item slots from displaying stretched - instead scales them down
  - Minimum window width now slightly smaller
- Suit Fuse HUD indicator now blinks red when the suit is rebooting, was solid red
- Suit Controllers now display Fuel below the Power bar instead of an Overdraw progress bar. Now the fuel info is present on both controllers.
- Increased the size of the suit HUD fuel bar to keep it from becoming invisible at smaller resolutions. If you still have this problem, use the mod options to configure its scale.
- Renamed config option 'Module Overlay Widgets' to 'Enable Custom HUD Overlay Widgets'
- Made it easier for suits to display their own widgets in the HUD. No changes visible for the end user.
- Significant internal file structure cleanup - basically every asset in every sub mod, except descriptors, has moved around. This should help me start releasing source code publicly again - previously I had to stop because of copyrighted sounds, meshes, and textures being included.

## Bugfixes

- Fixed bug that would cause multiplayer hosts to crash when exiting a loaded save file while a client wearing a suit was still connected
- Fixed suit HUD sometimes disappearing after dying or using a vehicle at any point during the game session.
- Fixed long-standing bug preventing multiplayer hosts from seeing the color of the suits that multiplayer clients are wearing
- Fixed bug causing the "fake player" rendered in vehicles to not retain the suit color of the player driving it.
- Adjusted spacing in the inventory side panel widget to correct a slight gap on the top and left sides of the tab control
- Shifted the default suit HUD position slightly right (like 5 pixels) to prevent the side of the indicator light glow from being cut off
- Fixed that 'Hover' was lowercased in the suit HUD movement state indicator
- Now on version `1.0.0` so other mods can list this as a dependency without the pain caused by `^` special behavior for major versions less than one

## Known Bugs

- Multiplayer clients that logged in wearing a suit may need to re-equip it upon joining
- Entering a vehicle while flying can very rarely and inconsistently kill your player without leaving a death crate
- Multiplayer clients don't get fuel fuse recovery sounds
