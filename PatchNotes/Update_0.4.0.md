Early Release 2

- This release comes with the debugging symbols, so crash reports should be a lot more helpful now.

- Gravity flight in Multiplayer should be less jumpy now. If you still have problems with this, it is likely because your host's FPS is low compared to the client.
- Now only Modules can be stored in a PowerSuit's inventory. This is to prevent you from storing your suit within itself, making it permanently inaccessible.
- New system of widgets that modules can control
- Overlay Module Widgets
	- These can render above the PowerSuit HUD
- Attachment Module Widgets
	- These already existed, but now have a name
	- Attachments can set an AMW to appear inside the Suit Inventory's 'Module Configuration' section
- Inventory Module Widgets
	- Show up on the right side of the inventory pane

- By default, you can no longer open the Suit Inventory window with your regular inventory open. The new Inventory Module Widget section takes up a lot of space. (TODO) You can re-enable this in the Config if you'd like.
- By default, if you try to open your regular inventory with the Suit Inventory window open, it will open out to the side of the regular Inventory window.

- Fuel unlocks should work more consistently in multiplayer and be faster to calculate


## Changes and New Features

- ![](https://i.imgur.com/xLJKY0B.png)
- You can install modules in your Suit directly from your main inventory in addition to the usual suit inventory window. This can be turned off in the config.
- ![](https://i.imgur.com/82ypQeT.png)
- You can now configure the default size and position of the inventory widget from the Mods Menu config.
- More descriptive names and tooltips for the keybinds in the options screen. Hover over them for more info.
- You can now enable experimental automatic tooltips for all modules from config. Expect this to change some in the future.
- ![](https://i.imgur.com/wkFL8wO.png)
- Suit window now also displays fuel amount and has icons for power/fuel
- Spacing and section name changes in the suit inventory UI. Most things also have tool tips now that explain what they are for.
- You can hover over a labeled region of the screen to see what fuels your Suit accepts
- New suit material coloration, also - colors for the hoverpack on your back!

## Bugfixes

- Numerous multiplayer client related bugfixes
- Fixed loss of suit inventory/possible suit corruption on death. Suit inventories are no longer owned by the pawn, which is deleted on death. Instead, they are owned by the suit actor.
- Fixed flight timer logic for multiplayer
- Fixed MP clients being unable to wear suits that the host has used already
- Unequipping a suit no longer leaves the equipment hud widget unusually large.
- Potential rare hypertube crash fixed, but might still be possible in multiplayer
- Mod icon now shows in the in-game mods list.
- Health regen stat now uses correct units
- Resistances all show in the correct units
- Fuel HUD display now actually matches stored fuel
- Air Refuel flag works again
- Fixed display of flags in Debug Stats
- Fixed suits not being able to receive fuel produced by Modules until they had refueled at least once from inventory items.

## For Developers

- **Renamed** 'Parent' to 'Parent Module' for clarity
- Suit equip's mCostToUse is now used to set up Allowed Fuels as well (item count ignored)
- **New custom properties functionality**. Attachments can map a name to a modmult property and, in effect, create a custom Suit Property. These named properties will not do anything to the suit by default, but you can use them to do whatever you want via attachments.
- Lots more updated editor tooltips.
- **ModMult property handling change**. Now the total result for a stat is `Mult * (BaseGame + Mod)`. This means that multipliers can affect base game values for a stat even if you don't have a modifier set for it. For example, you can now make a module that reduces walk speed by 10%. See editor tooltip for more.
- **HasNoGravityMode renamed to AllowsToggleGravityMode** for clarity. Function remains the same.
- A couple internal BP functions have had their names changes and/or outputs renamed
- New 'Advanced Suit Flags' for more suit flags in the future