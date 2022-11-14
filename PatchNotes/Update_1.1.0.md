New config option to change how the module slots display in the inventory.




This update brought to you by Robb#6731.

Be sure to read the 1.0.0 changelog - lots of stuff changed!

## New Stuff

- New configuration option: Suit Contents Display Mode
  - Thanks Dovahkin for the idea
  - > Switch between fixed-width (true) and auto-scale (false) display of the suit contents you see in your player equipment panel in the inventory.
    > In auto-scale mode, suits with fewer slots will display larger slot icons.
    > Default true.
  - Examples on Suit Mk1:
    - Fixed-width mode (checked, default)

      ![True](https://i.imgur.com/NFPJgHt.png)
    - Auto-scale mode (unchecked)

      ![False](https://i.imgur.com/kUfxsVW.png)
- Added tooltip to allowed fuels that explains how the fuel priority system works (for now, I plan to change it):

  ![TooltipInGame](https://i.imgur.com/MtWdZfQ.png)

## Bugfixes

- Will now try to re-create the suit HUD if another mod has somehow removed or re-created the player HUD

## Internal

- Changes to library calls needed to fix bugs in LogisticModules
- Changes to internal handling of suit HUD widget

## Known Bugs

- Multiplayer clients that logged in wearing a suit may need to re-equip it upon joining
- Entering a vehicle while flying can very rarely and inconsistently kill your player without leaving a death crate
- Multiplayer clients don't get fuel fuse recovery sounds
