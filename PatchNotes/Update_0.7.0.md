A bundle of UI improvements, bugfixes, and support for new sound features.




# Changes and New Features

- Suit now reboots instantly when joining the game and exiting vehicles.
  - This prevents you from dying from radiation after exiting a train, for example. In the past, it was possible to die as the suit was rebooting, since entering a vehicle destroys your pawn -> suit would reboot on exit since it was considered a fresh equip
- Suit UI sometimes disappearing when switching hand equipment should now be fixed
- Numerous internal improvements and changes for PowerSuit Modules functionality, as well as for some upcoming Modules.
- New EMC HoverMovementSpeedAdded and HoverMovementAccelSpeedAdded properties to allow for faster modification of flight speeds (can be changed without having to go through the stats refresh system, but you need to be careful with replication and such)
- Fixed Hoverpack Controllers hand mesh leaving your hands if you use an FOV other than 90
- You can now go directly to the Suit popup without having to close your inventory first.
- The inventory Configs tab will now take you directly to the Suit popup, avoiding any problems with the base-game inventory screwing with text field inputs for configs.
- Trying to open your inventory while the Suit popup is open will now close it and open your base game inventory instead.
- Modules can now specify install/uninstall sounds in the module descriptor.
- Sounds played when a module is disallowed from being installed.
- New inventory VFX for when your suit is rebooting that explains why Modules can't be installed at this time, and suggests ways to get out of reboot loops.
- Fixes to issues with remotes installing copies of a module they already have installed
- Fixed mixed up help tooltips in suit inventory screens.
- Fix suit allowed fuels problem where removing a module that granted the same fuels the suit does would disallow them until you re-equipped the suit
- A new easter egg!?!? Don't move too fast...
